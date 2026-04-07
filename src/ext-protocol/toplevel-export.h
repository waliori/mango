/*
 * hyprland-toplevel-export-v1 — server-side implementation for mango.
 *
 * Allows clients (e.g. Quickshell) to capture the contents of any toplevel
 * window via screen capture, including windows on inactive tags. Used to
 * implement the workspace overview live previews.
 *
 * Protocol XML: protocols/hyprland-toplevel-export-v1.xml
 * Generated header: hyprland-toplevel-export-v1-protocol.h
 *
 * Resources used from the surrounding mango.c translation unit:
 *   dpy       — wl_display *
 *   drw       — wlr_renderer *
 *   clients   — wl_list of Client.link
 *   client_surface(c) — returns wlr_surface * for a Client *
 *   Client    — struct definition (forward visible from mango.c)
 */

#include "hyprland-toplevel-export-v1-protocol.h"
#include <time.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wlr/render/pass.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_foreign_toplevel_management_v1.h>

static struct wl_global *toplevel_export_manager_global;

/* All live frames, used to invalidate frames when their captured Client dies. */
static struct wl_list toplevel_export_frames;

struct toplevel_export_frame {
	struct wl_resource *resource;
	Client *client;
	bool overlay_cursor;
	bool buffer_sent;
	uint32_t width;
	uint32_t height;
	struct wl_list link; /* toplevel_export_frames */
};

/* Forward declarations for the interface vtables. */
static const struct hyprland_toplevel_export_manager_v1_interface
	toplevel_export_manager_impl;
static const struct hyprland_toplevel_export_frame_v1_interface
	toplevel_export_frame_impl;

/* ----- Helpers -------------------------------------------------------- */

/*
 * Resolve a zwlr_foreign_toplevel_handle_v1 wl_resource back to the Client
 * that owns it. Iterates the global clients list and walks each
 * foreign_toplevel handle's resources list.
 */
static Client *
client_from_zwlr_handle_resource(struct wl_resource *handle_res) {
	if (!handle_res)
		return NULL;
	Client *c;
	wl_list_for_each(c, &clients, link) {
		if (!c->foreign_toplevel)
			continue;
		struct wl_resource *r;
		wl_resource_for_each(r, &c->foreign_toplevel->resources) {
			if (r == handle_res)
				return c;
		}
	}
	return NULL;
}

/* Returns the dimensions of the client's content area in surface coords. */
static void
client_get_capture_size(Client *c, uint32_t *out_w, uint32_t *out_h) {
	struct wlr_surface *s = client_surface(c);
	if (s && s->current.width > 0 && s->current.height > 0) {
		*out_w = (uint32_t)s->current.width;
		*out_h = (uint32_t)s->current.height;
	} else if (c->geom.width > 0 && c->geom.height > 0) {
		*out_w = (uint32_t)c->geom.width;
		*out_h = (uint32_t)c->geom.height;
	} else {
		*out_w = 1;
		*out_h = 1;
	}
}

/* ----- Rendering: walk client's surface tree, blit each surface -------- */

struct render_ctx {
	struct wlr_render_pass *pass;
	int dst_w;
	int dst_h;
};

static void
render_surface_iter(struct wlr_surface *surface, int sx, int sy, void *data) {
	struct render_ctx *ctx = (struct render_ctx *)data;
	struct wlr_texture *tex = wlr_surface_get_texture(surface);
	if (!tex)
		return;

	struct wlr_box dst = {
		.x = sx,
		.y = sy,
		.width = surface->current.width,
		.height = surface->current.height,
	};

	/* Skip surfaces that fall completely outside the capture area. */
	if (dst.x >= ctx->dst_w || dst.y >= ctx->dst_h ||
	    dst.x + dst.width <= 0 || dst.y + dst.height <= 0) {
		return;
	}

	struct wlr_render_texture_options opts = {
		.texture = tex,
		.dst_box = dst,
		.transform = surface->current.transform,
		.filter_mode = WLR_SCALE_FILTER_BILINEAR,
		.blend_mode = WLR_RENDER_BLEND_MODE_PREMULTIPLIED,
	};
	wlr_render_pass_add_texture(ctx->pass, &opts);
}

static bool
render_client_to_buffer(Client *c, struct wlr_buffer *target) {
	struct wlr_surface *root = client_surface(c);
	if (!root || !wlr_surface_has_buffer(root))
		return false;

	struct wlr_render_pass *pass =
		wlr_renderer_begin_buffer_pass(drw, target, NULL);
	if (!pass)
		return false;

	/* Clear to transparent black first. */
	struct wlr_render_rect_options clear = {
		.box = { .x = 0, .y = 0, .width = target->width, .height = target->height },
		.color = { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f },
		.blend_mode = WLR_RENDER_BLEND_MODE_NONE,
	};
	wlr_render_pass_add_rect(pass, &clear);

	struct render_ctx ctx = {
		.pass = pass,
		.dst_w = target->width,
		.dst_h = target->height,
	};
	wlr_surface_for_each_surface(root, render_surface_iter, &ctx);

	return wlr_render_pass_submit(pass);
}

/* ----- Frame lifetime ------------------------------------------------- */

static void
toplevel_export_frame_destroy_resource(struct wl_resource *resource) {
	struct toplevel_export_frame *f =
		(struct toplevel_export_frame *)wl_resource_get_user_data(resource);
	if (!f)
		return;
	wl_list_remove(&f->link);
	free(f);
}

/*
 * Invalidate any frames captured against `c` (called when c is being destroyed).
 * Frames have their client pointer cleared and a failed event sent.
 */
static void
toplevel_export_invalidate_client(Client *c) {
	struct toplevel_export_frame *f, *tmp;
	wl_list_for_each_safe(f, tmp, &toplevel_export_frames, link) {
		if (f->client == c) {
			f->client = NULL;
			if (f->resource)
				hyprland_toplevel_export_frame_v1_send_failed(f->resource);
		}
	}
}

static void
toplevel_export_frame_handle_copy(struct wl_client *wl_client,
                                  struct wl_resource *frame_res,
                                  struct wl_resource *buffer_res,
                                  int32_t ignore_damage) {
	(void)wl_client;
	(void)ignore_damage;
	struct toplevel_export_frame *f =
		(struct toplevel_export_frame *)wl_resource_get_user_data(frame_res);
	if (!f) {
		hyprland_toplevel_export_frame_v1_send_failed(frame_res);
		return;
	}

	if (!f->client || f->client->iskilling) {
		hyprland_toplevel_export_frame_v1_send_failed(frame_res);
		return;
	}

	struct wlr_buffer *dst = wlr_buffer_try_from_resource(buffer_res);
	if (!dst) {
		hyprland_toplevel_export_frame_v1_send_failed(frame_res);
		return;
	}

	if ((uint32_t)dst->width != f->width || (uint32_t)dst->height != f->height) {
		wl_resource_post_error(frame_res,
		                       HYPRLAND_TOPLEVEL_EXPORT_FRAME_V1_ERROR_INVALID_BUFFER,
		                       "buffer size %dx%d does not match frame %dx%d",
		                       dst->width, dst->height, f->width, f->height);
		wlr_buffer_unlock(dst);
		return;
	}

	bool ok = render_client_to_buffer(f->client, dst);
	wlr_buffer_unlock(dst);

	if (!ok) {
		hyprland_toplevel_export_frame_v1_send_failed(frame_res);
		return;
	}

	/* flags = 0: GPU output is already top-down for our renderer. If we see
	 * upside-down previews in testing, change to ..._FLAGS_Y_INVERT. */
	hyprland_toplevel_export_frame_v1_send_flags(frame_res, 0);

	/* Always advertise full damage — keeps the protocol simple and matches
	 * the polling cadence of Quickshell's screencopy. */
	hyprland_toplevel_export_frame_v1_send_damage(frame_res, 0, 0, f->width, f->height);

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint32_t tv_sec_hi = (uint32_t)((uint64_t)ts.tv_sec >> 32);
	uint32_t tv_sec_lo = (uint32_t)ts.tv_sec;
	hyprland_toplevel_export_frame_v1_send_ready(
		frame_res, tv_sec_hi, tv_sec_lo, (uint32_t)ts.tv_nsec);
}

static void
toplevel_export_frame_handle_destroy(struct wl_client *wl_client,
                                     struct wl_resource *frame_res) {
	(void)wl_client;
	wl_resource_destroy(frame_res);
}

static const struct hyprland_toplevel_export_frame_v1_interface
	toplevel_export_frame_impl = {
		.copy = toplevel_export_frame_handle_copy,
		.destroy = toplevel_export_frame_handle_destroy,
};

/* ----- Frame creation ------------------------------------------------- */

static void
toplevel_export_create_frame(struct wl_client *wl_client, uint32_t mgr_version,
                             uint32_t frame_id, int32_t overlay_cursor,
                             Client *c) {
	struct toplevel_export_frame *f = calloc(1, sizeof(*f));
	if (!f) {
		wl_client_post_no_memory(wl_client);
		return;
	}

	uint32_t version = mgr_version;
	if (version > 2)
		version = 2;
	f->resource = wl_resource_create(
		wl_client, &hyprland_toplevel_export_frame_v1_interface,
		(int)version, frame_id);
	if (!f->resource) {
		free(f);
		wl_client_post_no_memory(wl_client);
		return;
	}
	wl_resource_set_implementation(f->resource, &toplevel_export_frame_impl,
	                               f, toplevel_export_frame_destroy_resource);

	f->client = c;
	f->overlay_cursor = overlay_cursor != 0;
	wl_list_insert(&toplevel_export_frames, &f->link);

	/* If the client target is invalid right now, fail immediately but keep
	 * the resource alive so the client can destroy it cleanly. */
	if (!c || c->iskilling || !client_surface(c) ||
	    !wlr_surface_has_buffer(client_surface(c))) {
		f->client = NULL;
		f->width = 1;
		f->height = 1;
		hyprland_toplevel_export_frame_v1_send_buffer(
			f->resource, WL_SHM_FORMAT_ARGB8888, 1, 1, 4);
		hyprland_toplevel_export_frame_v1_send_buffer_done(f->resource);
		hyprland_toplevel_export_frame_v1_send_failed(f->resource);
		return;
	}

	uint32_t w, h;
	client_get_capture_size(c, &w, &h);
	f->width = w;
	f->height = h;

	/* SHM is mandatory per protocol. */
	hyprland_toplevel_export_frame_v1_send_buffer(
		f->resource, WL_SHM_FORMAT_ARGB8888, w, h, w * 4);

	/* DMA-BUF: advertise the renderer's preferred ARGB format if available. */
	const struct wlr_drm_format_set *dmabuf_formats =
		wlr_renderer_get_texture_formats(drw, WLR_BUFFER_CAP_DMABUF);
	if (dmabuf_formats && dmabuf_formats->len > 0) {
		hyprland_toplevel_export_frame_v1_send_linux_dmabuf(
			f->resource, dmabuf_formats->formats[0].format, w, h);
	}

	hyprland_toplevel_export_frame_v1_send_buffer_done(f->resource);
	f->buffer_sent = true;
}

/* ----- Manager request handlers --------------------------------------- */

static void
toplevel_export_manager_handle_capture_toplevel(struct wl_client *wl_client,
                                                struct wl_resource *mgr_res,
                                                uint32_t frame_id,
                                                int32_t overlay_cursor,
                                                uint32_t handle) {
	/* The "handle by address" variant — we don't support it (it's used by
	 * hyprctl-style tools that have direct compositor address knowledge).
	 * Create the frame, immediately fail. */
	(void)handle;
	uint32_t version = (uint32_t)wl_resource_get_version(mgr_res);
	toplevel_export_create_frame(wl_client, version, frame_id, overlay_cursor, NULL);
}

static void
toplevel_export_manager_handle_capture_toplevel_with_wlr_toplevel_handle(
	struct wl_client *wl_client, struct wl_resource *mgr_res,
	uint32_t frame_id, int32_t overlay_cursor,
	struct wl_resource *toplevel_handle_res) {
	Client *c = client_from_zwlr_handle_resource(toplevel_handle_res);
	uint32_t version = (uint32_t)wl_resource_get_version(mgr_res);
	toplevel_export_create_frame(wl_client, version, frame_id, overlay_cursor, c);
}

static void
toplevel_export_manager_handle_destroy(struct wl_client *wl_client,
                                       struct wl_resource *mgr_res) {
	(void)wl_client;
	wl_resource_destroy(mgr_res);
}

static const struct hyprland_toplevel_export_manager_v1_interface
	toplevel_export_manager_impl = {
		.capture_toplevel = toplevel_export_manager_handle_capture_toplevel,
		.destroy = toplevel_export_manager_handle_destroy,
		.capture_toplevel_with_wlr_toplevel_handle =
			toplevel_export_manager_handle_capture_toplevel_with_wlr_toplevel_handle,
};

/* ----- Global binding ------------------------------------------------- */

static void
toplevel_export_manager_bind(struct wl_client *wl_client, void *data,
                             uint32_t version, uint32_t id) {
	(void)data;
	uint32_t v = version;
	if (v > 2)
		v = 2;
	struct wl_resource *res = wl_resource_create(
		wl_client, &hyprland_toplevel_export_manager_v1_interface, (int)v, id);
	if (!res) {
		wl_client_post_no_memory(wl_client);
		return;
	}
	wl_resource_set_implementation(res, &toplevel_export_manager_impl, NULL, NULL);
}

/* ----- Public entry point --------------------------------------------- */

void
hyprland_toplevel_export_v1_create(struct wl_display *display) {
	wl_list_init(&toplevel_export_frames);
	toplevel_export_manager_global = wl_global_create(
		display, &hyprland_toplevel_export_manager_v1_interface, 2, NULL,
		toplevel_export_manager_bind);
}

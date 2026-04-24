#include <wlr/types/wlr_ext_foreign_toplevel_list_v1.h>

static struct wlr_ext_foreign_toplevel_list_v1 *ext_foreign_toplevel_list;

void add_ext_foreign_toplevel(Client *c) {
	if (!c || !c->mon || !c->mon->wlr_output || !c->mon->wlr_output->enabled)
		return;
	struct wlr_ext_foreign_toplevel_handle_v1_state state = {
		.title = client_get_title(c),
		.app_id = client_get_appid(c),
	};
	c->ext_foreign_toplevel =
		wlr_ext_foreign_toplevel_handle_v1_create(ext_foreign_toplevel_list,
												  &state);
}

void update_ext_foreign_toplevel(Client *c) {
	if (!c || !c->ext_foreign_toplevel)
		return;
	struct wlr_ext_foreign_toplevel_handle_v1_state state = {
		.title = client_get_title(c),
		.app_id = client_get_appid(c),
	};
	wlr_ext_foreign_toplevel_handle_v1_update_state(c->ext_foreign_toplevel,
													&state);
}

// Create the handle if missing, otherwise update state. Mirrors
// reset_foreign_tolevel call sites without re-creating the handle
// (ext-foreign-toplevel-list-v1 is output-agnostic, unlike the older
// foreign-toplevel-management-v1 which re-enters outputs on reset).
void sync_ext_foreign_toplevel(Client *c) {
	if (c && c->ext_foreign_toplevel)
		update_ext_foreign_toplevel(c);
	else
		add_ext_foreign_toplevel(c);
}

void remove_ext_foreign_toplevel(Client *c) {
	if (!c || !c->ext_foreign_toplevel)
		return;
	wlr_ext_foreign_toplevel_handle_v1_destroy(c->ext_foreign_toplevel);
	c->ext_foreign_toplevel = NULL;
}

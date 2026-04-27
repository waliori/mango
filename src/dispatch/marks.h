/*
 * Sway-style window marks: name a window, jump back to it later.
 *
 *   mark="work"      — tag the focused client with the name "work".
 *   focus_mark="work" — focus whichever client currently holds that mark.
 *
 * Names are unique; marking a second client with a name automatically
 * unmarks the first. Marks are dropped when their client is destroyed.
 */

#define MARK_NAME_MAX 63

struct mango_mark {
	char name[MARK_NAME_MAX + 1];
	Client *c;
	struct wl_list link;
};

static struct wl_list marks = {&marks, &marks}; /* pre-initialised head */

static struct mango_mark *mark_find(const char *name) {
	struct mango_mark *m;
	wl_list_for_each(m, &marks, link) {
		if (strcmp(m->name, name) == 0)
			return m;
	}
	return NULL;
}

/* Forward decl — defined later in this file once dumpmarks_to_path exists. */
static inline void auto_dump_marks_maybe(void);

/* Drop every mark that references this client (called on client destroy). */
void mark_drop_client(Client *c) {
	struct mango_mark *m, *tmp;
	bool changed = false;
	wl_list_for_each_safe(m, tmp, &marks, link) {
		if (m->c == c) {
			wl_list_remove(&m->link);
			free(m);
			changed = true;
		}
	}
	if (changed)
		auto_dump_marks_maybe();
}

int32_t mark(const Arg *arg) {
	if (!selmon || !selmon->sel || !arg->v)
		return -1;
	const char *name = arg->v;
	if (*name == '\0' || strlen(name) > MARK_NAME_MAX)
		return -1;

	struct mango_mark *existing = mark_find(name);
	if (existing) {
		if (existing->c == selmon->sel)
			return 0; /* idempotent */
		existing->c = selmon->sel;
		auto_dump_marks_maybe();
		return 0;
	}

	struct mango_mark *m = ecalloc(1, sizeof(*m));
	snprintf(m->name, sizeof(m->name), "%s", name);
	m->c = selmon->sel;
	wl_list_insert(&marks, &m->link);
	auto_dump_marks_maybe();
	return 0;
}

int32_t focus_mark(const Arg *arg) {
	if (!arg->v)
		return -1;
	struct mango_mark *m = mark_find(arg->v);
	if (!m || !m->c || !m->c->mon)
		return -1;

	Client *c = m->c;
	/* Make the mark's workspace visible if it isn't. */
	if (!(c->tags & c->mon->tagset[c->mon->seltags])) {
		uint32_t target_tag = get_tags_first_tag(c->tags);
		view_in_mon(&(Arg){.ui = target_tag}, true, c->mon, true);
	}
	/* Switch to the mark's monitor if we're elsewhere. */
	if (c->mon != selmon) {
		selmon = c->mon;
	}
	focusclient(c, 1);
	/* Warp the pointer onto the focused window so cursor follows focus.
	 * Mirrors the destroydragicon pattern (focusclient → motionnotify) so
	 * pointer enter/leave fires and the right cursor shape gets set. */
	wlr_cursor_warp(cursor, NULL,
					c->geom.x + c->geom.width / 2.0,
					c->geom.y + c->geom.height / 2.0);
	motionnotify(0, NULL, 0, 0, 0, 0);
	return 0;
}

int32_t unmark(const Arg *arg) {
	if (!arg->v)
		return -1;
	struct mango_mark *m = mark_find(arg->v);
	if (!m)
		return -1;
	wl_list_remove(&m->link);
	free(m);
	auto_dump_marks_maybe();
	return 0;
}

/*
 * Swap the focused window with the window holding the given mark.
 *
 * The two windows trade tag + monitor. Focus stays on our current
 * monitor+tag; after the swap we see the mark's old client in our place,
 * and our old client is now on the mark's former tag/monitor.
 */
int32_t swap_with_mark(const Arg *arg) {
	if (!arg->v || !selmon || !selmon->sel)
		return -1;
	struct mango_mark *m = mark_find(arg->v);
	if (!m || !m->c || !m->c->mon)
		return -1;

	Client *a = selmon->sel;	  /* our window */
	Client *b = m->c;			  /* the marked window */
	if (a == b)
		return 0;

	uint32_t a_tags = a->tags;
	Monitor *a_mon = a->mon;
	uint32_t b_tags = b->tags;
	Monitor *b_mon = b->mon;

	if (a_mon == b_mon) {
		/* Same-monitor swap: setmon() would early-return without doing
		 * anything, so update tags directly. */
		a->tags = b_tags;
		b->tags = a_tags;
		arrange(a_mon, false, false);
	} else {
		/* Cross-monitor swap: use setmon on both. Clear m->sel first so
		 * setmon's "oldmon->sel==c" branch doesn't null them out mid-swap. */
		if (a_mon->sel == a)
			a_mon->sel = NULL;
		if (b_mon->sel == b)
			b_mon->sel = NULL;

		uint32_t a_w = a_mon->w.width, a_h = a_mon->w.height;
		uint32_t b_w = b_mon->w.width, b_h = b_mon->w.height;

		setmon(a, b_mon, b_tags, false);
		reset_foreign_tolevel(a);
		sync_ext_foreign_toplevel(a);
		if (a->isfloating) {
			a->float_geom.width =
				(int32_t)((int64_t)a->float_geom.width * b_w / a_w);
			a->float_geom.height =
				(int32_t)((int64_t)a->float_geom.height * b_h / a_h);
			a->float_geom =
				setclient_coordinate_center(a, b_mon, a->float_geom, 0, 0);
			a->geom = a->float_geom;
		}

		setmon(b, a_mon, a_tags, false);
		reset_foreign_tolevel(b);
		sync_ext_foreign_toplevel(b);
		if (b->isfloating) {
			b->float_geom.width =
				(int32_t)((int64_t)b->float_geom.width * a_w / b_w);
			b->float_geom.height =
				(int32_t)((int64_t)b->float_geom.height * a_h / b_h);
			b->float_geom =
				setclient_coordinate_center(b, a_mon, b->float_geom, 0, 0);
			b->geom = b->float_geom;
		}

		arrange(a_mon, false, false);
		arrange(b_mon, false, false);
	}

	/* Focus the mark's old client — it's now in our original slot. */
	selmon = a_mon;
	focusclient(b, 1);
	auto_dump_marks_maybe(); /* monitor/tags on the marked client may have changed */
	return 0;
}

/*
 * Dump every mark to a JSON array. Mirrors dumpclients shape so external tools
 * (rofi, Quickshell, etc.) can build a mark-jumping UI. Pair with
 * auto_dump_marks=1 + Quickshell FileView { watchChanges: true } for an
 * inotify-driven, zero-poll UI.
 */
static void dumpmarks_to_path(const char *filepath) {
	if (!filepath || filepath[0] == '\0')
		filepath = "/tmp/noir_marks.json";

	FILE *f = fopen(filepath, "w");
	if (!f)
		return;

	struct mango_mark *m;
	int first = 1;
	fprintf(f, "[");
	wl_list_for_each(m, &marks, link) {
		if (!m->c)
			continue;
		if (!first)
			fprintf(f, ",");
		fprintf(f, "{\"name\":");
		json_escape_string(f, m->name);
		fprintf(f, ",\"appid\":");
		json_escape_string(f, client_get_appid(m->c));
		fprintf(f, ",\"icon\":");
		json_escape_string(f, (m->c->icon && m->c->icon->name) ? m->c->icon->name : "");
		fprintf(f, ",\"title\":");
		json_escape_string(f, client_get_title(m->c));
		fprintf(f, ",\"tags\":%u", m->c->tags);
		fprintf(f, ",\"monitor\":");
		json_escape_string(f, m->c->mon ? m->c->mon->wlr_output->name : "");
		fprintf(f, "}");
		first = 0;
	}
	fprintf(f, "]\n");
	fclose(f);
}

int32_t dumpmarks(const Arg *arg) {
	dumpmarks_to_path(arg->v);
	return 0;
}

static inline void auto_dump_marks_maybe(void) {
	if (config.auto_dump_marks)
		dumpmarks_to_path("/tmp/noir_marks.json");
}

/*
 * Toggle dispatcher: if a mark with this name exists, focus it (jump);
 * otherwise create the mark on the focused client (save). One bind = save+jump.
 */
int32_t focus_mark_or_set(const Arg *arg) {
	if (!arg->v)
		return -1;
	struct mango_mark *m = mark_find(arg->v);
	if (m && m->c)
		return focus_mark(arg);
	return mark(arg);
}

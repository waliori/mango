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

/* Drop every mark that references this client (called on client destroy). */
void mark_drop_client(Client *c) {
	struct mango_mark *m, *tmp;
	wl_list_for_each_safe(m, tmp, &marks, link) {
		if (m->c == c) {
			wl_list_remove(&m->link);
			free(m);
		}
	}
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
		return 0;
	}

	struct mango_mark *m = ecalloc(1, sizeof(*m));
	snprintf(m->name, sizeof(m->name), "%s", name);
	m->c = selmon->sel;
	wl_list_insert(&marks, &m->link);
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
	return 0;
}

/*
 * Swap the focused window with the window holding the given mark.
 * The two windows trade tag + monitor. If either is the other, no-op.
 */
int32_t swap_with_mark(const Arg *arg) {
	if (!arg->v || !selmon || !selmon->sel)
		return -1;
	struct mango_mark *m = mark_find(arg->v);
	if (!m || !m->c || !m->c->mon)
		return -1;

	Client *a = selmon->sel;
	Client *b = m->c;
	if (a == b)
		return 0;

	uint32_t a_tags = a->tags;
	Monitor *a_mon = a->mon;
	uint32_t b_tags = b->tags;
	Monitor *b_mon = b->mon;

	a->tags = b_tags;
	if (a_mon != b_mon)
		client_change_mon(a, b_mon);

	b->tags = a_tags;
	if (b_mon != a_mon)
		client_change_mon(b, a_mon);

	arrange(a_mon, false, false);
	if (b_mon != a_mon)
		arrange(b_mon, false, false);
	focusclient(a, 1);
	return 0;
}

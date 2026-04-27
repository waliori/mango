---
title: Marks
description: Vim-style window marks — bind windows to names, jump back, swap, focus-or-set.
---

NoirWM has Vim-style window marks. Bind a focused window to a name, jump back to it later, swap places with it, or use a single bind that saves-or-jumps depending on whether the mark exists.

Marks are runtime-only — they don't persist across compositor restarts.

## Dispatchers

| Dispatch | Args | Description |
| :--- | :--- | :--- |
| `mark` | `name` | Bind the focused client to `name`. Re-using a name re-binds. |
| `focus_mark` | `name` | Focus the client bound to `name`. Cursor warps onto the window. No-op if the mark doesn't exist. |
| `focus_mark_or_set` | `name` | If `name` exists, focus it; otherwise, bind it to the focused window. One bind = save spot if empty / jump if set. |
| `swap_with_mark` | `name` | Swap the focused window's tile slot with the marked window's slot (works across monitors). |

Marks are removed automatically when their target window is destroyed.

## Recommended binds

```ini
# Quick "back-and-forth" — mark current, jump back later
bind=SUPER+SHIFT,m,mark,last
bind=SUPER+SHIFT,n,focus_mark,last

# Swap with the "last" marked window
bind=SUPER+SHIFT,x,swap_with_mark,last

# Named marks: dedicated slots
bind=SUPER+CTRL,1,mark,one
bind=SUPER+CTRL,2,mark,two
bind=SUPER+CTRL,3,mark,three
bind=SUPER+CTRL+SHIFT,1,focus_mark,one
bind=SUPER+CTRL+SHIFT,2,focus_mark,two
bind=SUPER+CTRL+SHIFT,3,focus_mark,three

# One-key save-or-jump
bind=SUPER+CTRL,4,focus_mark_or_set,four
```

## Cursor follows focus

`focus_mark` and `focus_mark_or_set` warp the cursor onto the centered window after focus. This avoids the awkward "focus jumped to a window the cursor isn't on" state — clicks land where you expect.

## External UIs

Marks are also exposed via `/tmp/noir_marks.json` when `auto_dump_marks=1` is set in the config. External tools (rofi, Quickshell) can read this to render a quick-switcher with previews. See [Auto-dump JSON](/docs/configuration/auto-dump).

A reference Quickshell-based marks overlay (live thumbnails of every marked window) lives in [waliori/WalioriOS](https://github.com/waliori/WalioriOS) under `home-manager/dotfiles/quickshell/MarksOverlay.qml`.

## Mental model

Think of marks as a tiny, name-keyed pin board: the WM lets you pin a window under a name, jump to a pin, swap with a pin, or save-or-jump in one key. Combined with `focus_mark_or_set`, this is enough for a "scratchpad-but-by-window" workflow without touching the actual scratchpad pile.

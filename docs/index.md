---
title: Introduction
description: NoirWM — a Wayland compositor tuned for external shells, fork of MangoWC, based on dwl.
---


**NoirWM** is a Wayland compositor tuned for external shells. It's a fork of [MangoWC](https://github.com/mangowm/mango) — which itself derives from [dwl](https://codeberg.org/dwl/dwl/) — with marks, IPC state dumps, live toplevel previews, and a handful of new protocols. Pairs with [Quickshell](https://quickshell.outfoxxed.me/).

The binary is `noir`. The dispatcher CLI is `mmsg` (kept from upstream). Configuration lives in `~/.config/noir/`.

> **Philosophy:** The compositor is the substrate, not the surface. NoirWM stays small and fast, and exposes its state cleanly so external tools — Quickshell, rofi, scripts — can build the rest.

## What's carried over from MangoWC

- **[Animations](/docs/visuals/animations)** — smooth, customizable animations for windows, tags, layers.
- **[Layouts](/docs/window-management/layouts)** — Scroller, Master-Stack, Monocle, Grid, Deck, and more, per-tag.
- **[Visual effects](/docs/visuals/effects)** — blur, shadows, corner radius, opacity (via [scenefx](https://github.com/wlrfx/scenefx)).
- **[IPC](/docs/ipc)** — `mmsg` for full external control of compositor state.
- **XWayland** — solid compatibility for legacy X11 apps.
- **Tags** instead of workspaces, with separate window layouts per tag.
- **Rich window states** — swallow, minimize, maximize, fullscreen, overlay, global.
- **Hot-reload** — config reloads without restart.
- **Scratchpads** — Sway-like + named.

## What NoirWM adds on top of MangoWC

- **[Window marks](/docs/window-management/marks)** — Vim-style marks: bind a window to a name, jump back later, swap, focus-or-set toggle. Cursor follows focus.
- **[Auto-dumped JSON state](/docs/configuration/auto-dump)** — `/tmp/noir_clients.json` and `/tmp/noir_marks.json` re-emit on every relevant state change. Drives inotify-based external UIs (Quickshell, rofi) with zero polling.
- **[Live toplevel previews](/docs/visuals/live-previews)** — server-side `hyprland-toplevel-export-v1` so external shells can render per-window thumbnails (workspace overview, dock previews, marks overlay).
- **`xdg-toplevel-icon-v1`** — protocol exposed; client-provided icon names propagate via `dwl-ipc-v3` and the JSON dumps so external icons can adopt app-supplied hints.
- **`dim_inactive`** — darken unfocused clients via overlay rect (separate from `unfocused_opacity`).
- **Silent tag navigation** — `tagtoleftsilent`, `tagtorightsilent`, `viewtoleftsilent`, `viewtorightsilent`, `tagsilent` — move/view without forcing focus.
- **`switch_layout_prev`** — reverse-cycle layouts.
- **`mmsg` session detection** — accepts `XDG_SESSION_DESKTOP=noir` (so `XDG_CURRENT_DESKTOP` can stay `wlroots` for portal routing without breaking the IPC).
- **scenefx 0.4.1 with PR #154 backport** vendored — fixes layer blur stencil on rotated outputs.

## Acknowledgements

- **[MangoWC](https://github.com/mangowm/mango)** — upstream by DreamMaoMao. NoirWM tracks compatible features.
- **[dwl](https://codeberg.org/dwl/dwl)** — the dwl Wayland compositor; NoirWM's distant ancestor.
- **[wlroots](https://gitlab.freedesktop.org/wlroots/wlroots)** — Wayland protocol implementation.
- **[scenefx](https://github.com/wlrfx/scenefx)** — visual effects library.
- **[mwc](https://github.com/nikoloc/mwc)** — basal window animation reference.
- **[sway](https://github.com/swaywm/sway)** — sample Wayland compositor.

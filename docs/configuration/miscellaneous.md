---
title: Miscellaneous
description: Advanced settings — XWayland, focus, dim_inactive, auto-dump.
---

## System & hardware

| Setting | Default | Description |
| :--- | :--- | :--- |
| `xwayland_persistence` | `1` | Keep XWayland running even when no X11 apps are open (reduces startup lag). |
| `syncobj_enable` | `0` | Enable `drm_syncobj` timeline support (helps with gaming stutter/lag). **Requires restart.** |
| `allow_lock_transparent` | `0` | Allow the lock screen to be transparent. |
| `allow_shortcuts_inhibit` | `1` | Allow shortcuts to be inhibited by clients. |
| `vrr` | - | Set via [monitor rule](/docs/configuration/monitors#monitor-rules). |

## Focus & input

| Setting | Default | Description |
| :--- | :--- | :--- |
| `focus_on_activate` | `1` | Automatically focus windows when they request activation. |
| `sloppyfocus` | `1` | Focus follows the mouse cursor. |
| `warpcursor` | `1` | Warp the cursor to the center of the window when focus changes via keyboard. |
| `cursor_hide_timeout` | `0` | Hide the cursor after `N` seconds of inactivity (`0` to disable). |
| `drag_tile_to_tile` | `0` | Allow dragging a tiled window onto another to swap their positions. |
| `drag_corner` | `3` | Corner for drag-to-tile detection (0: none, 1–3: corners, 4: auto-detect). |
| `drag_warp_cursor` | `1` | Warp cursor when dragging windows to tile. |
| `axis_bind_apply_timeout` | `100` | Timeout (ms) for detecting consecutive scroll events for axis bindings. |
| `axis_scroll_factor` | `1.0` | Scroll factor for axis scroll speed (0.1–10.0). |

## Multi-monitor & tags

| Setting | Default | Description |
| :--- | :--- | :--- |
| `focus_cross_monitor` | `0` | Allow directional focus to cross monitor boundaries. |
| `exchange_cross_monitor` | `0` | Allow exchanging clients across monitor boundaries. |
| `focus_cross_tag` | `0` | Allow directional focus to cross into other tags. |
| `view_current_to_back` | `0` | Toggling the current tag switches back to the previously viewed tag. |
| `scratchpad_cross_monitor` | `0` | Share the scratchpad pool across all monitors. |
| `single_scratchpad` | `1` | Only allow one scratchpad (named or standard) to be visible at a time. |
| `circle_layout` | - | Comma-separated list of layouts `switch_layout` cycles through (e.g. `tile,scroller`). |

## Window behavior

| Setting | Default | Description |
| :--- | :--- | :--- |
| `enable_floating_snap` | `0` | Snap floating windows to edges or other windows. |
| `snap_distance` | `30` | Max distance (pixels) to trigger floating snap. |
| `no_border_when_single` | `0` | Remove window borders when only one window is visible on the tag. |
| `idleinhibit_ignore_visible` | `0` | Allow invisible clients (e.g., background audio players) to inhibit idle. |
| `drag_tile_refresh_interval` | `8.0` | Interval (1.0–16.0) to refresh tiled window resize during drag. Too small may cause application lag. |
| `drag_floating_refresh_interval` | `8.0` | Interval (1.0–16.0) to refresh floating window resize during drag. Too small may cause application lag. |

## Inactive dim (NoirWM)

Darken unfocused clients via overlay rect. Independent of `unfocused_opacity` — they stack.

| Setting | Default | Description |
| :--- | :--- | :--- |
| `dim_inactive` | `0` | Enable inactive-window dim overlay. |
| `dim_strength` | `0.3` | Dim opacity (0.0 – 1.0). |

```ini
dim_inactive=1
dim_strength=0.3

# Optional: per-window opt-out
windowrule=nodim:1,appid:mpv
```

## Auto-dump JSON state (NoirWM)

Push compositor state to `/tmp` on every relevant change, so external UIs read via inotify (zero polling). See [Auto-dump JSON](/docs/configuration/auto-dump) for the full schema.

| Setting | Default | Description |
| :--- | :--- | :--- |
| `auto_dump_clients` | `0` | Re-emit `/tmp/noir_clients.json` on every relevant client state change. |
| `auto_dump_marks` | `0` | Re-emit `/tmp/noir_marks.json` on every mark mutation. |

```ini
auto_dump_clients=1
auto_dump_marks=1
```

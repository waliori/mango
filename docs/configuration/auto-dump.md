---
title: Auto-dump JSON
description: Push compositor state to /tmp on every change so external UIs read via inotify.
---

NoirWM auto-emits JSON state files when configured to. External tools read them via inotify (`FileView { watchChanges: true }` in Quickshell, `inotifywait` in shell scripts, etc.) — no polling, no `mmsg` round-trips.

## Enabling auto-dump

```ini
auto_dump_clients=1
auto_dump_marks=1
```

When set, NoirWM writes:

- `/tmp/noir_clients.json` — re-emitted on every relevant client state change (open, close, focus, mapnotify, unmapnotify, title/appid update, icon set, monitor move, etc.).
- `/tmp/noir_marks.json` — re-emitted on every mark mutation.

The compositor writes the file atomically: open → write → close → rename, so consumers reading on inotify don't see partial state.

## `noir_clients.json` shape

```json
[
  {
    "appid": "kitty",
    "icon": "kitty",
    "title": "fish ~/projects",
    "tags": 1,
    "x": 0, "y": 0, "w": 1920, "h": 1080,
    "floating": 0,
    "minimized": 0,
    "activated": 1,
    "monitor": "DP-3"
  },
  ...
]
```

| Field | Description |
| :--- | :--- |
| `appid` | XDG appid (Wayland) or class hint (XWayland). |
| `icon` | Client-provided icon name from `xdg-toplevel-icon-v1` (empty string if the client didn't set one — fall back to `appid` for icon lookup). |
| `title` | Window title. |
| `tags` | Bitmask of tags this client is on. |
| `x`, `y`, `w`, `h` | Window geometry in screen coordinates. |
| `floating` | 1 if floating, 0 if tiled. |
| `minimized` | 1 if in the scratchpad pile. |
| `activated` | 1 if this is the focused client of the focused monitor. |
| `monitor` | Output name (e.g. `DP-3`, `HDMI-A-1`). |

## `noir_marks.json` shape

```json
[
  {
    "name": "last",
    "appid": "firefox",
    "icon": "firefox",
    "title": "Hacker News",
    "tags": 4,
    "monitor": "DP-3"
  },
  ...
]
```

| Field | Description |
| :--- | :--- |
| `name` | The mark's name (e.g. `last`, `one`, custom strings). |
| `appid` / `icon` / `title` / `tags` / `monitor` | Properties of the marked client. |

The `name` field is what you pass to `mmsg -d focus_mark,<name>` (or `swap_with_mark`).

## Manual dumps

You can also dump on demand:

```bash
mmsg -d dumpclients              # writes /tmp/noir_clients.json
mmsg -d dumpclients,/tmp/x.json  # custom path
mmsg -d dumpmarks                # writes /tmp/noir_marks.json
mmsg -d dumpmarks,/tmp/m.json    # custom path
```

## Consuming from Quickshell

```qml
import Quickshell.Io

FileView {
    id: clients
    path: "/tmp/noir_clients.json"
    preload: true
    watchChanges: true
    onFileChanged: reload()
    onTextChanged: {
        try {
            var data = JSON.parse(text())
            // ... use data
        } catch (e) {}
    }
}
```

## Consuming from a shell script

```bash
inotifywait -m -e modify /tmp/noir_clients.json |
while read; do
  jq '[.[] | select(.activated == 1)] | .[0]' /tmp/noir_clients.json
done
```

## Performance

The dumps are O(n) in client count, called from event handlers that already touch the client list. On a typical session (≤30 clients) the cost is microseconds — negligible. The compiler-inlined `auto_dump_*_maybe()` helpers no-op when the flag is off, so leaving them disabled has zero cost.

## Notes

- The shape is forward-compatible: new fields may be added but existing ones won't disappear without a major version bump. Consumers should ignore unknown keys.
- If you need a snapshot rather than a stream, just call `mmsg -d dumpclients` and read the file.
- The JSON is UTF-8 with strings escaped per RFC 8259.

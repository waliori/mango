---
title: Live Toplevel Previews
description: Render per-window thumbnails from external shells via hyprland-toplevel-export-v1.
---

NoirWM implements `hyprland-toplevel-export-v1` server-side. Wayland clients (e.g. [Quickshell](https://quickshell.outfoxxed.me/)) can use it to capture an individual toplevel window — including windows on inactive tags — to produce live thumbnails.

This is what powers workspace overviews, dock window previews, and the marks-overlay quick-switcher in companion shells.

## Why this protocol

The "modern" `ext_foreign_toplevel_image_capture_source_v1` is the upstream-blessed approach but isn't supported by Quickshell's `ScreencopyView` for per-toplevel captures yet. NoirWM implements the Hyprland protocol because that's what shell clients actually consume today.

NoirWM stays on **wlroots 0.19 + scenefx 0.4.1**; the protocol is implemented in a single header file (`src/ext-protocol/toplevel-export.h`) so noir doesn't need a wlroots bump.

## How clients use it

Quickshell:

```qml
import Quickshell.Wayland
import Quickshell.Wayland._ToplevelManagement

Item {
    property var toplevel    // a ToplevelHandle from ToplevelManager.toplevels.values

    ScreencopyView {
        anchors.fill: parent
        captureSource: parent.toplevel
        live: true            // continuously capture
    }
}
```

The capture works for every toplevel — visible, on a different tag, or partially obscured.

## Limitations (from the protocol spec)

- **No server-side decorations** — the capture contains the client's surface tree, not noir's borders, shadows, or scenefx effects. This matches Hyprland's behaviour and is by design.
- **Unmapped surfaces show as transparent** — a toplevel that hasn't committed a buffer (e.g. just created, not yet drawn) captures as a fully transparent frame. This is harmless but worth knowing.
- **`y_invert` is always 0** — top-down rendering matches what Quickshell expects; no extra transform needed on the client side.
- **Damage region is always full** — every frame is a full re-capture. Cheap because the renderer reuses textures, but worth knowing if you're profiling.

## Companion features

Live previews pair naturally with the [auto-dumped client JSON](/docs/configuration/auto-dump): use the JSON to enumerate windows (titles, tags, icons) and the protocol to render their thumbnails.

## References

- Protocol XML: `protocols/hyprland-toplevel-export-v1.xml`
- Implementation: `src/ext-protocol/toplevel-export.h`
- Reference consumer: `home-manager/dotfiles/quickshell/WorkspaceOverview.qml` in [waliori/WalioriOS](https://github.com/waliori/WalioriOS).

---
title: Status Bar
description: Configure Waybar (or any other Wayland status bar) for NoirWM.
---

NoirWM speaks the standard `ext/workspaces` Wayland protocol and the legacy `dwl/tags` protocol. Most status bars (waybar, eww) work out of the box.

For a richer external shell that also consumes NoirWM's [auto-dumped JSON state](/docs/configuration/auto-dump) and [live toplevel previews](/docs/visuals/live-previews), use [Quickshell](https://quickshell.outfoxxed.me/).

## Waybar

NoirWM is compatible with Waybar's `ext/workspaces` module (Wayland standard) and the `dwl/tags` module. We recommend `ext/workspaces` for the best experience.

> **Tip:** `ext/workspaces` requires **Waybar > 0.14.0**.

### `config.jsonc`

```jsonc
{
  "modules-left": [
    "ext/workspaces",
    "dwl/window"
  ],
  "ext/workspaces": {
    "format": "{icon}",
    "ignore-hidden": true,
    "on-click": "activate",
    "on-click-right": "deactivate",
    "sort-by-id": true
  },
  "dwl/window": {
    "format": "[{layout}] {title}"
  }
}
```

### `style.css` (sample)

```css
#workspaces {
  border-radius: 4px;
  border: 2px solid #c9b890;
  margin-left: 4px;
  padding: 0 6px 0 10px;
  background: rgba(40, 40, 40, 0.76);
}

#workspaces button {
  border: none;
  background: none;
  color: #ddca9e;
  padding: 1px;
  margin: 0 2px;
}

#workspaces button.hidden     { color: #9e906f; background: transparent; }
#workspaces button.visible    { color: #ddca9e; }
#workspaces button:hover      { color: #d79921; }
#workspaces button.active     { background-color: #ddca9e; color: #282828; border-radius: 3px; }
#workspaces button.urgent     { background-color: #ef5e5e; color: #282828; border-radius: 3px; }

#tags                         { background-color: transparent; }
#tags button                  { background-color: #fff; color: #a585cd; }
#tags button:not(.occupied):not(.focused) { font-size: 0; min-width: 0; min-height: 0; margin: -17px; padding: 0; color: transparent; background-color: transparent; }
#tags button.occupied         { background-color: #fff; color: #cdc885; }
#tags button.focused          { background-color: rgb(186, 142, 213); color: #fff; }
#tags button.urgent           { background: rgb(171, 101, 101); color: #fff; }

#window                       { background-color: rgb(237, 196, 147); color: rgb(63, 37, 5); }
```

## Reference

For a full Waybar + Quickshell setup with NoirWM, see [waliori/WalioriOS](https://github.com/waliori/WalioriOS) under `home-manager/dotfiles/waybar/` and `home-manager/dotfiles/quickshell/`.

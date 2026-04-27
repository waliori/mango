---
title: Quick Start
description: Basic configuration and first steps with NoirWM.
---

Now that you have NoirWM installed, get the environment set up.

## Initial setup

1. **Create configuration directory**

   NoirWM looks in `~/.config/noir/`.

   ```bash
   mkdir -p ~/.config/noir
   ```

2. **Copy the default config**

   The starter config is installed at `/etc/noir/config.conf`. Copy it to start customizing.

   ```bash
   cp /etc/noir/config.conf ~/.config/noir/config.conf
   ```

3. **Launch noir**

   From a TTY:

   ```bash
   noir
   ```

   Or with a custom config path:

   ```bash
   noir -c /path/to/your/config.conf
   ```

## Default keybindings

The starter config uses these defaults:

| Key | Action |
| :--- | :--- |
| `Alt` + `Return` | Open terminal (defaults to `foot`) |
| `Alt` + `Space` | Open launcher (defaults to `rofi`) |
| `Alt` + `Q` | Close (kill) the active window |
| `Super` + `M` | Quit noir |
| `Super` + `F` | Toggle fullscreen |
| `Alt` + arrows | Move focus (left/right/up/down) |
| `Ctrl` + `1-9` | Switch to tag 1-9 |
| `Alt` + `1-9` | Move window to tag 1-9 |

> **Warning:** Some bindings rely on `foot` (terminal) and `rofi` (launcher). Install them or update `binds.conf` to use your preferred alternatives.

## Recommended companion tools

| Category | Options |
| :--- | :--- |
| Application launcher | rofi, bemenu, wmenu, fuzzel |
| Terminal | foot, kitty, ghostty, wezterm, alacritty |
| Status bar / shell | **Quickshell** (preferred — pairs with noir's auto-dumped JSON), waybar, eww, ags |
| Wallpaper | swww, swaybg |
| Notifications | swaync, dunst, mako |
| Portals | xdg-desktop-portal, xdg-desktop-portal-wlr, xdg-desktop-portal-gtk |
| Clipboard | wl-clipboard, wl-clip-persist, cliphist |
| Gamma / night light | wlsunset, gammastep |
| Misc | xfce-polkit, wlogout |

## Reference configuration

For a full working setup using NoirWM + Quickshell + waybar, see the dotfiles in [waliori/WalioriOS](https://github.com/waliori/WalioriOS) under `home-manager/dotfiles/noir/` and `home-manager/dotfiles/quickshell/`.

## Next steps

- [Configure Monitors](/docs/configuration/monitors) — resolution, scaling, multi-monitor.
- [Window Rules](/docs/window-management/rules#window-rules) — per-application behavior.
- [Marks](/docs/window-management/marks) — Vim-style window marks.
- [Auto-dump JSON](/docs/configuration/auto-dump) — drive external UIs from compositor state.
- [Theming](/docs/visuals/theming) — colors, borders, gaps.

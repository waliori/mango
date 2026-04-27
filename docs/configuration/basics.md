---
title: Basic Configuration
description: Configure NoirWM via config files, environment variables, and autostart scripts.
---

## Configuration file

NoirWM uses a simple plain-text config format. By default it looks for `~/.config/noir/config.conf`.

1. **Locate the default config**

   A fallback configuration is installed at `/etc/noir/config.conf`. Use it as a reference.

2. **Create your user config**

   ```bash
   mkdir -p ~/.config/noir
   cp /etc/noir/config.conf ~/.config/noir/config.conf
   ```

3. **Or launch with a custom config (optional)**

   ```bash
   noir -c /path/to/your_config.conf
   ```

### Sub-configuration

Split the config into multiple files using `source`:

```ini
# Import keybindings from a separate file
source=~/.config/noir/binds.conf

# Relative paths work
source=./theme.conf

# Optional: ignore if file doesn't exist (useful for shared configs)
source-optional=~/.config/noir/optional.conf
```

### Validate configuration

Check for parse errors without starting noir:

```bash
noir -c /path/to/config.conf -p
```

Use with `source-optional` for shared configs across different setups.

## Environment variables

You can define environment variables directly in the config file. They're set before the window manager fully initializes.

> **Warning:** Environment variables defined here are **reset** every time you reload the configuration.

```ini
env=QT_IM_MODULES,wayland;fcitx
env=XMODIFIERS,@im=fcitx
env=XDG_SESSION_DESKTOP,noir
```

## Autostart

NoirWM can run commands or scripts on startup. Two execution modes:

| Command | Behavior | Use case |
| :--- | :--- | :--- |
| `exec-once` | Runs **only once** when noir starts. | Status bars, wallpapers, notification daemons. |
| `exec` | Runs **every time** the config is reloaded. | Scripts that need to refresh settings. |
| `exec-shutdown` | Runs once on noir exit. | Cleanup notifications, cache flushes. |

### Example

```ini
# Status bar (once)
exec-once=waybar

# Wallpaper
exec-once=swww-daemon
exec-once=swww img ~/.config/noir/wallpaper/room.png

# Reload a custom script on config change
exec=bash ~/.config/noir/reload-settings.sh

# Notify on shutdown
exec-shutdown=notify-send "noir" "session ended"
```

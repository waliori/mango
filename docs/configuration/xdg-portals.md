---
title: XDG Portals
description: Set up screen sharing, clipboard, keyring, and file pickers using XDG portals.
---

## Portal configuration

Customize portal settings via:

- **User config (priority):** `~/.config/xdg-desktop-portal/noir-portals.conf`
- **System fallback:** `/usr/share/xdg-desktop-portal/noir-portals.conf`

> **Warning:** If you previously added `dbus-update-activation-environment --systemd WAYLAND_DISPLAY XDG_CURRENT_DESKTOP=wlroots` to your config, remove it. NoirWM handles this automatically.

NoirWM keeps `XDG_CURRENT_DESKTOP=wlroots` so portal config keys off `wlroots` cleanly. `XDG_SESSION_DESKTOP=noir` is set in the starter config — that's what `mmsg` keys off, so the IPC keeps working.

## Screen sharing

Required for OBS / Discord / WebRTC: `xdg-desktop-portal-wlr`.

1. **Install dependencies**

   `pipewire`, `pipewire-pulse`, `xdg-desktop-portal-wlr`

2. **Optional: explicit portal start**

   Some setups need the portal launched explicitly:

   ```bash
   /usr/lib/xdg-desktop-portal-wlr &
   ```

3. **Restart** to apply changes.

### Known issues

- **Window screen sharing:** Some apps have issues sharing individual windows. Upstream tracking issue: [mangowm/mango#184](https://github.com/mangowm/mango/pull/184).
- **Screen recording lag:** See [xdg-desktop-portal-wlr#351](https://github.com/emersion/xdg-desktop-portal-wlr/issues/351).

## Clipboard manager

`cliphist` for clipboard history.

**Dependencies:** `wl-clipboard`, `cliphist`, `wl-clip-persist`.

**Autostart:**

```bash
# Persist clipboard content after the source app closes
wl-clip-persist --clipboard regular --reconnect-tries 0 &

# Watch clipboard and store history
wl-paste --type text --watch cliphist store &
```

## GNOME keyring

For password storage (VS Code, Minecraft launchers, etc.), install `gnome-keyring`.

```ini
# ~/.config/xdg-desktop-portal/noir-portals.conf
[preferred]
default=gtk
org.freedesktop.impl.portal.ScreenCast=wlr
org.freedesktop.impl.portal.Screenshot=wlr
org.freedesktop.impl.portal.Secret=gnome-keyring
org.freedesktop.impl.portal.Inhibit=none
```

## File picker

**Dependencies:** `xdg-desktop-portal`, `xdg-desktop-portal-gtk`. Reboot once to apply.

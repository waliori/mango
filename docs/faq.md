---
title: FAQ
description: Frequently asked questions and troubleshooting.
---

### How do I arrange tiled windows with my mouse?

Enable `drag_tile_to_tile` in the config. This allows dragging a tiled window onto another to swap them.

```ini
drag_tile_to_tile=1
```

---

### Why is my background blurry or why does blur look wrong?

Blur applies to the transparent areas of windows. To disable it entirely, set `blur=0`.

If you're seeing **performance issues with blur**, make sure `blur_optimized=1` (the default). This caches the wallpaper as the blur background, which is much cheaper on the GPU:

```ini
blur_optimized=1
```

---

### Blur shows my wallpaper instead of the real background content

This is expected when `blur_optimized=1` (the default). The optimizer caches the wallpaper to reduce GPU load — windows blur against the wallpaper rather than the actual content stacked beneath them.

If you want blur to composite against the true background, set:

```ini
blur_optimized=0
```

> **Warning:** Disabling `blur_optimized` significantly increases GPU consumption and may cause rendering lag, especially on lower-end hardware.

---

### Blur looks wrong (uniform / banded) on a rotated output

NoirWM vendors `scenefx 0.4.1` with a 2-part PR #154 backport that fixes layer blur stencil on rotated outputs. If you're seeing this regression, you've probably linked against a system `scenefx` that lacks the backport. Build with the vendored subproject (the default).

---

### My games are lagging or stuttering

Try enabling **SyncObj** timeline support and **Adaptive Sync** (VRR) if your monitor supports it.

```ini
syncobj_enable=1
adaptive_sync=1
```

---

### My games have high input latency

Enable **tearing** (similar to VSync off). Globally:

```ini
allow_tearing=1
```

Then force it for the specific game:

```ini
windowrule=force_tearing:1,title:Counter-Strike 2
```

> **Warning:** Some graphics cards require setting `WLR_DRM_NO_ATOMIC=1` before noir starts for tearing to work. Add it to `/etc/environment` and reboot, or launch noir with `WLR_DRM_NO_ATOMIC=1 noir`. See [Monitors — Tearing](/docs/configuration/monitors#tearing-game-mode).

---

### How do I use pipes `|` in spawn commands?

`spawn` does not support shell pipes directly — use `spawn_shell` instead.

```ini
bind=SUPER,P,spawn_shell,echo "hello" | rofi -dmenu
```

---

### Certain key combinations do not work on my keyboard layout.

`bind` automatically converts keysym to keycode. Compatible with most layouts but sometimes imprecise. If a combination isn't triggering, use the **keycode** directly.

Run `wev` and press the key to find its keycode, then use it in your bind:

```ini
# Instead of:
bind=ALT,q,killclient

# Use the keycode (e.g., code:24 = q on most layouts):
bind=ALT,code:24,killclient
```

You can also use `binds` (the `s` flag) to match by keysym instead of keycode:

```ini
binds=ALT,q,killclient
```

---

### `mmsg` says "neither XDG_CURRENT_DESKTOP nor XDG_SESSION_DESKTOP names 'noir'"

`mmsg` looks for the substring `noir` in either env var. The compositor sets `XDG_CURRENT_DESKTOP=noir` itself at start, but if your session was launched in a way that overrides that env (some display managers, some systemd unit overrides), the dispatcher will refuse to run.

Fix: in your noir config, ensure
```ini
env=XDG_SESSION_DESKTOP,noir
```
is set. The starter config has this. Then re-login.

If you keep `XDG_CURRENT_DESKTOP=wlroots` (because your portal config keys off `wlroots`), that's fine — `XDG_SESSION_DESKTOP=noir` is enough for `mmsg` detection.

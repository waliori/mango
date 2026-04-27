---
title: Monitors
description: Manage display outputs, resolution, scaling, and tearing.
---

## Monitor rules

Configure each display output via the `monitorrule` keyword.

**Syntax:**

```ini
monitorrule=name:Values,Parameter:Values,Parameter:Values
```

> **Info:** If any of the matching fields (`name`, `make`, `model`, `serial`) are set, **all** of the set ones must match. Use `wlr-randr` to get your monitor's name, make, model, and serial.

### Parameters

| Parameter | Type | Values | Description |
| :--- | :--- | :--- | :--- |
| `name` | string | Any | Match by monitor name (supports regex) |
| `make` | string | Any | Match by manufacturer |
| `model` | string | Any | Match by model |
| `serial` | string | Any | Match by serial number |
| `width` | integer | 0–9999 | Monitor width |
| `height` | integer | 0–9999 | Monitor height |
| `refresh` | float | 0.001–9999.0 | Refresh rate |
| `x` | integer | 0–99999 | X position |
| `y` | integer | 0–99999 | Y position |
| `scale` | float | 0.01–100.0 | Scale factor |
| `vrr` | integer | 0, 1 | Variable refresh rate |
| `rr` | integer | 0–7 | Transform |
| `custom` | integer | 0, 1 | Enable custom mode (not supported on all displays — may cause black screen) |

### Transform values

| Value | Rotation |
| :--- | :--- |
| `0` | No transform |
| `1` | 90° counter-clockwise |
| `2` | 180° counter-clockwise |
| `3` | 270° counter-clockwise |
| `4` | 180° vertical flip |
| `5` | Flip + 90° counter-clockwise |
| `6` | Flip + 180° counter-clockwise |
| `7` | Flip + 270° counter-clockwise |

> **Critical:** With XWayland applications, **never use negative coordinates** for monitor positions. This is a known XWayland bug that causes click events to malfunction. Always arrange monitors starting from `0,0` and extend into positive coordinates.

> **Note:** "name" is a regular expression. For an exact match, anchor with `^` and `$` (e.g. `^eDP-1$`).

### Examples

```ini
# Laptop display: 1080p, 60Hz, positioned at origin
monitorrule=name:^eDP-1$,width:1920,height:1080,refresh:60,x:0,y:10

# Match by make and model
monitorrule=make:Chimei Innolux Corporation,model:0x15F5,width:1920,height:1080,refresh:60,x:0,y:0

# Virtual monitor with pattern matching
monitorrule=name:HEADLESS-.*,width:1920,height:1080,refresh:60,x:1926,y:0,scale:1,rr:0,vrr:0
```

---

## Monitor spec format

`focusmon`, `tagmon`, `disable_monitor`, `enable_monitor`, `toggle_monitor`, `viewcrossmon`, `tagcrossmon` accept a **monitor_spec** string.

```text
name:xxx&&make:xxx&&model:xxx&&serial:xxx
```

- Any field can be omitted, no order requirement.
- If all fields are omitted, the string is treated as the monitor name (e.g., `eDP-1`).

```bash
mmsg -d toggle_monitor,eDP-1
mmsg -d toggle_monitor,make:Chimei Innolux Corporation&&model:0x15F5
mmsg -d toggle_monitor,serial:12345678
```

---

## Tearing (game mode)

Tearing lets games bypass compositor VSync for lower latency.

| Setting | Default | Description |
| :--- | :--- | :--- |
| `allow_tearing` | `0` | `0` (Disable), `1` (Enable), `2` (Fullscreen only). |

**Globally:**

```ini
allow_tearing=1
```

**Per window:**

```ini
windowrule=force_tearing:1,title:vkcube
```

### Tearing behavior matrix

| `force_tearing` \ `allow_tearing` | DISABLED (0) | ENABLED (1) | FULLSCREEN_ONLY (2) |
| :--- | :--- | :--- | :--- |
| **UNSPECIFIED** (0) | Not allowed | Follows tearing_hint | Fullscreen follows tearing_hint |
| **ENABLED** (1) | Not allowed | Allowed | Fullscreen only |
| **DISABLED** (2) | Not allowed | Not allowed | Not allowed |

### GPU compatibility

> **Warning:** Some graphics cards require setting `WLR_DRM_NO_ATOMIC=1` before noir starts to enable tearing.

```bash
# /etc/environment
WLR_DRM_NO_ATOMIC=1

# or one-shot
WLR_DRM_NO_ATOMIC=1 noir
```

---

## GPU selection

If noir shows a black screen on multi-GPU systems, force a specific GPU:

```bash
WLR_DRM_DEVICES=/dev/dri/card1 noir
WLR_DRM_DEVICES=/dev/dri/card0:/dev/dri/card1 noir
```

Some GPUs have compatibility issues with `syncobj_enable=1` — it may crash apps like `kitty`. Set `WLR_DRM_NO_ATOMIC=1` and reboot.

---

## Power management

```bash
mmsg -d disable_monitor,eDP-1
mmsg -d enable_monitor,eDP-1
mmsg -d toggle_monitor,eDP-1
```

Or `wlr-randr`:

```bash
wlr-randr --output eDP-1 --off
wlr-randr --output eDP-1 --on
wlr-randr
```

---

## Screen scale

### Without global scale (recommended)

If you don't use XWayland apps, use monitor rules or `wlr-randr` for a global monitor scale. With XWayland, global scale is not recommended.

For a 1.4 factor:

**Dependencies:** `xorg-xrdb`, `xwayland-satellite`.

```ini
env=QT_AUTO_SCREEN_SCALE_FACTOR,1
env=QT_WAYLAND_FORCE_DPI,140
```

**Autostart:**

```bash
echo "Xft.dpi: 140" | xrdb -merge
gsettings set org.gnome.desktop.interface text-scaling-factor 1.4
```

**XWayland scale:**

```bash
/usr/sbin/xwayland-satellite :11 &
sleep 0.5s && echo "Xft.dpi: 140" | xrdb -merge
```

### Using xwayland-satellite to prevent blurry XWayland apps

For fractional scaling, use `xwayland-satellite` to scale XWayland apps.

```ini
env=DISPLAY,:2
exec-once=xwayland-satellite :2
monitorrule=name:eDP-1,width:1920,height:1080,refresh:60,x:0,y:0,scale:1.4,vrr:0,rr:0
```

> **Warning:** Use a `DISPLAY` value other than `:1` to avoid conflicts with noir.

---

## Virtual monitors

Create headless displays for screen sharing/remote desktop:

```bash
mmsg -d create_virtual_output
mmsg -d destroy_all_virtual_output
```

```bash
wlr-randr
wlr-randr --output HEADLESS-1 --pos 1921,0 --scale 1 --custom-mode 1920x1080@60Hz
```

Pair with [Sunshine](https://github.com/LizardByte/Sunshine) and [Moonlight](https://github.com/moonlight-stream/moonlight-android) for using other devices as extended monitors.

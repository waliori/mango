---
title: Input Devices
description: Configure keyboard layouts, mouse sensitivity, and touchpad gestures.
---

## Device configuration

NoirWM provides granular control over different input devices.

### Keyboard settings

| Setting | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `repeat_rate` | `int` | `25` | Key repeats per second. |
| `repeat_delay` | `int` | `600` | Delay (ms) before a held key starts repeating. |
| `numlockon` | `0` or `1` | `0` | Enable NumLock on startup. |
| `xkb_rules_rules` | `string` | - | XKB rules file (`evdev`, `base`). Usually auto-detected. |
| `xkb_rules_model` | `string` | - | Keyboard model (`pc104`, `macbook`). |
| `xkb_rules_layout` | `string` | - | Layout code (`us`, `de`, `us,de`). |
| `xkb_rules_variant` | `string` | - | Variant (`dvorak`, `colemak`, `intl`). |
| `xkb_rules_options` | `string` | - | Options (`caps:escape`, `ctrl:nocaps`). |

```ini
repeat_rate=40
repeat_delay=300
numlockon=1
xkb_rules_layout=us,de
xkb_rules_variant=dvorak
xkb_rules_options=caps:escape,ctrl:nocaps
```

---

### Trackpad settings

For laptop touchpads. Some require a relogin.

| Setting | Default | Description |
| :--- | :--- | :--- |
| `disable_trackpad` | `0` | Set to `1` to disable the trackpad entirely. |
| `tap_to_click` | `1` | Tap to trigger a left click. |
| `tap_and_drag` | `1` | Tap and hold to drag items. |
| `trackpad_natural_scrolling` | `0` | Invert scrolling direction (natural scrolling). |
| `scroll_button` | `274` | The mouse button used for scrolling (272–279). |
| `scroll_method` | `1` | `1` (Two-finger), `2` (Edge), `4` (Button). |
| `click_method` | `1` | `1` (Button areas), `2` (Clickfinger). |
| `drag_lock` | `1` | Lock dragging after tapping. |
| `disable_while_typing` | `1` | Disable trackpad while typing. |
| `left_handed` | `0` | Swap left/right buttons. |
| `middle_button_emulation` | `0` | Emulate middle button. |
| `swipe_min_threshold` | `1` | Minimum swipe threshold. |

#### Detailed values

- **`scroll_button`**: `272` (Left), `273` (Right), `274` (Middle), `275` (Side), `276` (Extra), `277` (Forward), `278` (Back), `279` (Task).
- **`scroll_method`**: `0` (none), `1` (two-finger), `2` (edge), `4` (button).
- **`click_method`**: `0` (none), `1` (button areas), `2` (clickfinger).
- **`accel_profile`**: `0` (none), `1` (flat), `2` (adaptive).
- **`button_map`**: `0` (1/2/3 finger → left/right/middle), `1` (left/middle/right).
- **`send_events_mode`**: `0` (normal), `1` (disabled), `2` (disabled when external pointer plugged in).

---

### Mouse settings

| Setting | Default | Description |
| :--- | :--- | :--- |
| `mouse_natural_scrolling` | `0` | Invert scrolling direction. |
| `accel_profile` | `2` | `0` (None), `1` (Flat), `2` (Adaptive). |
| `accel_speed` | `0.0` | Speed adjustment (-1.0 to 1.0). |
| `left_handed` | `0` | Swap left and right buttons. |
| `middle_button_emulation` | `0` | Emulate middle button. |
| `swipe_min_threshold` | `1` | Minimum swipe threshold. |
| `send_events_mode` | `0` | `0` (Enabled), `1` (Disabled), `2` (Disabled on external mouse). |
| `button_map` | `0` | `0` (Left/right/middle), `1` (Left/middle/right). |

---

## Keyboard layout switching

Define multiple layouts and toggle between them:

```ini
xkb_rules_layout=us,us
xkb_rules_variant=,dvorak
xkb_rules_options=grp:lalt_lshift_toggle
```

Or bind manually:

```ini
bind=alt,shift_l,switch_keyboard_layout
```

Use `mmsg -g -k` to query the current layout.

---

## Input Method Editor (IME)

To use Fcitx5 or IBus, set these in your config.

> **Info:** Requires a restart of the window manager.

**Fcitx5:**

```ini
env=GTK_IM_MODULE,fcitx
env=QT_IM_MODULE,fcitx
env=QT_IM_MODULES,wayland;fcitx
env=SDL_IM_MODULE,fcitx
env=XMODIFIERS,@im=fcitx
env=GLFW_IM_MODULE,ibus
```

**IBus:**

```ini
env=GTK_IM_MODULE,ibus
env=QT_IM_MODULE,ibus
env=XMODIFIERS,@im=ibus
```

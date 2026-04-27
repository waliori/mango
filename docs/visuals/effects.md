---
title: Window Effects
description: Add visual polish with blur, shadows, opacity, and inactive-dim.
---

## Blur

Blur creates a frosted glass effect for transparent windows.

| Setting | Default | Description |
| :--- | :--- | :--- |
| `blur` | `0` | Enable blur for windows. |
| `blur_layer` | `0` | Enable blur for layer surfaces (bars/docks). |
| `blur_optimized` | `1` | Cache the wallpaper as the blur background. **Highly recommended.** |
| `blur_params_radius` | `5` | Strength (radius) of the blur. |
| `blur_params_num_passes` | `1` | Number of passes. Higher = smoother but more expensive. |
| `blur_params_noise` | `0.02` | Noise level. |
| `blur_params_brightness` | `0.9` | Brightness adjustment. |
| `blur_params_contrast` | `0.9` | Contrast adjustment. |
| `blur_params_saturation` | `1.2` | Saturation adjustment. |

> **Warning:** Blur has a relatively high impact on performance. If your hardware is limited, leave it off. With `blur_optimized=0` GPU consumption rises significantly. To disable blur entirely, set `blur=0`.

> **Note:** NoirWM vendors `scenefx 0.4.1` with a 2-part PR #154 backport that fixes layer blur stencil on rotated outputs. If you're seeing rotated-output artifacts (uniform blur, banding), confirm you're building with the vendored subproject (the default).

---

## Shadows

| Setting | Default | Description |
| :--- | :--- | :--- |
| `shadows` | `0` | Enable shadows. |
| `layer_shadows` | `0` | Enable shadows for layer surfaces. |
| `shadow_only_floating` | `1` | Only draw shadows for floating windows (saves performance). |
| `shadows_size` | `10` | Shadow size. |
| `shadows_blur` | `15` | Shadow blur amount. |
| `shadows_position_x` | `0` | X offset. |
| `shadows_position_y` | `0` | Y offset. |
| `shadowscolor` | `0x000000ff` | Shadow color. |

```ini
shadows=1
layer_shadows=1
shadow_only_floating=1
shadows_size=12
shadows_blur=15
shadowscolor=0x000000ff
```

---

## Opacity & corner radius

| Setting | Default | Description |
| :--- | :--- | :--- |
| `border_radius` | `0` | Window corner radius (px). |
| `border_radius_location_default` | `0` | Corner: `0` (all), `1` (top-left), `2` (top-right), `3` (bottom-left), `4` (bottom-right), `5` (closest corner). |
| `no_radius_when_single` | `0` | Disable radius when only one window is visible. |
| `focused_opacity` | `1.0` | Active-window opacity (0.0 – 1.0). |
| `unfocused_opacity` | `1.0` | Inactive-window opacity (0.0 – 1.0). |

```ini
border_radius=0
border_radius_location_default=0
no_radius_when_single=0
focused_opacity=1.0
unfocused_opacity=1.0
```

---

## Inactive dim (NoirWM)

Darken unfocused clients via overlay rect. Stacks with `unfocused_opacity` — they're independent.

| Setting | Default | Description |
| :--- | :--- | :--- |
| `dim_inactive` | `0` | Enable inactive-window dim overlay. |
| `dim_strength` | `0.3` | Dim opacity (0.0 – 1.0). |

```ini
dim_inactive=1
dim_strength=0.3

# Per-window opt-out
windowrule=nodim:1,appid:mpv
```

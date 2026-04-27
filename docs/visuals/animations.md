---
title: Animations
description: Configure smooth transitions for windows and layers.
---

## Enabling animations

NoirWM supports animations for both standard windows and layer-shell surfaces (bars, notifications).

```ini
animations=1
layer_animations=1
```

## Animation types

Available types: `slide`, `zoom`, `fade`, `none`.

```ini
animation_type_open=zoom
animation_type_close=slide
layer_animation_type_open=slide
layer_animation_type_close=slide
```

## Fade settings

```ini
animation_fade_in=1
animation_fade_out=1
fadein_begin_opacity=0.5
fadeout_begin_opacity=0.5
```

- `animation_fade_in` — Enable fade-in (0/1).
- `animation_fade_out` — Enable fade-out (0/1).
- `fadein_begin_opacity` — Starting opacity for fade-in (0.0–1.0).
- `fadeout_begin_opacity` — Starting opacity for fade-out (0.0–1.0).

## Zoom settings

```ini
zoom_initial_ratio=0.4
zoom_end_ratio=0.8
```

## Durations

| Setting | Default | Description |
| :--- | :--- | :--- |
| `animation_duration_move` | `500` | Move animation (ms). |
| `animation_duration_open` | `400` | Open animation (ms). |
| `animation_duration_tag` | `300` | Tag-switch animation (ms). |
| `animation_duration_close` | `300` | Close animation (ms). |
| `animation_duration_focus` | `0` | Focus opacity transition (ms). |

## Bezier curves

Curves shape the "feel" of animations (linear vs bouncy). Format: `x1,y1,x2,y2`.

Generators: [cssportal.com](https://www.cssportal.com/css-cubic-bezier-generator/), [easings.net](https://easings.net).

| Setting | Default | Description |
| :--- | :--- | :--- |
| `animation_curve_open` | `0.46,1.0,0.29,0.99` | Open animation. |
| `animation_curve_move` | `0.46,1.0,0.29,0.99` | Move animation. |
| `animation_curve_tag` | `0.46,1.0,0.29,0.99` | Tag animation. |
| `animation_curve_close` | `0.46,1.0,0.29,0.99` | Close animation. |
| `animation_curve_focus` | `0.46,1.0,0.29,0.99` | Focus transition. |
| `animation_curve_opafadein` | `0.46,1.0,0.29,0.99` | Open opacity animation. |
| `animation_curve_opafadeout` | `0.5,0.5,0.5,0.5` | Close opacity animation. |

## Tag animation direction

| Setting | Default | Description |
| :--- | :--- | :--- |
| `tag_animation_direction` | `1` | `1`: horizontal, `0`: vertical. |

---
title: Theming
description: Customize the visual appearance of borders, colors, and the cursor.
---

## Dimensions

| Setting | Default | Description |
| :--- | :--- | :--- |
| `borderpx` | `4` | Border width (px). |
| `gappih` | `5` | Horizontal inner gap (between windows). |
| `gappiv` | `5` | Vertical inner gap. |
| `gappoh` | `10` | Horizontal outer gap (between windows and screen edges). |
| `gappov` | `10` | Vertical outer gap. |

## Colors

Format: `0xRRGGBBAA` hex.

```ini
rootcolor=0x323232ff      # Root window background
bordercolor=0x444444ff    # Inactive window border
focuscolor=0xc66b25ff     # Active window border
urgentcolor=0xad401fff    # Urgent window border
```

### State-specific colors

| State | Key | Default |
| :--- | :--- | :--- |
| Maximized | `maximizescreencolor` | `0x89aa61ff` |
| Scratchpad | `scratchpadcolor` | `0x516c93ff` |
| Global | `globalcolor` | `0xb153a7ff` |
| Overlay | `overlaycolor` | `0x14a57cff` |

> **Tip:** For scratchpad sizing, see [Scratchpad](/docs/window-management/scratchpad).

## Cursor

```ini
cursor_size=24
cursor_theme=Adwaita
```

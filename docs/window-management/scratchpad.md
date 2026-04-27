---
title: Scratchpad
description: Manage hidden "scratchpad" windows for quick access.
---

NoirWM supports two types of scratchpads: the standard pool (Sway-like) and named scratchpads.

## Standard scratchpad

Send any window to the "scratchpad" pile to hide it. Cycle through hidden windows with the toggle.

```ini
# Send current window to scratchpad
bind=SUPER,i,minimized

# Toggle (show/hide) the scratchpad
bind=ALT,z,toggle_scratchpad

# Restore window from scratchpad
bind=SUPER+SHIFT,i,restore_minimized
```

---

## Named scratchpad

Named scratchpads bind specific keys to specific applications. When triggered, NoirWM either launches the app (if not running) or toggles its visibility.

**1. Define the window rule**

Identify the app with a unique `appid` or `title` and mark it as a named scratchpad. The application must support setting a custom appid or title at launch:

- `st -c my-appid` — sets the appid
- `kitty -T my-title` — sets the window title
- `foot --app-id my-appid` — sets the appid

Use `none` as a placeholder when matching by only one field.

```ini
# Match by appid
windowrule=isnamedscratchpad:1,width:1280,height:800,appid:st-yazi

# Match by title
windowrule=isnamedscratchpad:1,width:1000,height:700,title:kitty-scratch
```

**2. Bind the toggle key**

Format: `bind=MOD,KEY,toggle_named_scratchpad,appid,title,command`

Use `none` for the field you're not matching on.

```ini
# Match by appid: launch 'st' with class 'st-yazi' running 'yazi'
bind=alt,h,toggle_named_scratchpad,st-yazi,none,st -c st-yazi -e yazi

# Match by title: launch 'kitty' with window title 'kitty-scratch'
bind=alt,k,toggle_named_scratchpad,none,kitty-scratch,kitty -T kitty-scratch
```

---

## Appearance

Scratchpad window size is relative to the screen.

```ini
scratchpad_width_ratio=0.8
scratchpad_height_ratio=0.9
scratchpadcolor=0x516c93ff
```

---

## See also

For Vim-style window-to-name binding (faster than scratchpads for "where did I just have that window" workflows), see [Marks](/docs/window-management/marks).

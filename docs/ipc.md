---
title: IPC
description: Control NoirWM programmatically using mmsg.
---

## Introduction

NoirWM ships an IPC tool called `mmsg` (kept from upstream MangoWC). It speaks the `zdwl_ipc_v2` protocol with a few NoirWM-specific extensions, lets you query state, watch events, and dispatch commands from external scripts.

For inotify-driven UIs that just want the current client/marks state, see also the [auto-dumped JSON files](/docs/configuration/auto-dump) — they're often a better fit than polling `mmsg`.

## Basic usage

```bash
mmsg [-OTLq]
mmsg [-o <output>] -s [-t <tags>] [-l <layout>] [-c <tags>] [-d <cmd>,<arg1>,<arg2>,<arg3>,<arg4>,<arg5>]
mmsg [-o <output>] (-g | -w) [-OotlcvmfxekbA]
```

### Options

| Flag | Description |
| :--- | :--- |
| `-q` | Quit noir. |
| `-g` | **Get** values (tags, layout, focused client). |
| `-s` | **Set** values (switch tags, layouts). |
| `-w` | **Watch** mode (streams events). |
| `-O` | Get all output (monitor) information. |
| `-T` | Get number of tags. |
| `-L` | Get all available layouts. |
| `-o` | Select output (monitor). |
| `-t` | Get/set selected tags (set with `[+-^.]`). |
| `-l` | Get/set current layout. |
| `-c` | Get title and appid of focused client. |
| `-v` | Get visibility of statusbar. |
| `-m` | Get fullscreen status. |
| `-f` | Get floating status. |
| `-d` | **Dispatch** an internal command. |
| `-x` | Get focused client geometry. |
| `-e` | Get the name of the last focused layer. |
| `-k` | Get current keyboard layout. |
| `-b` | Get current keybind mode. |
| `-A` | Get scale factor of monitor. |

## Examples

### Tag management

```bash
# Switch to Tag 1
mmsg -t 1

# Add Tag 2 to current view (Multiview)
mmsg -s -t 2+

# Remove Tag 2 from current view
mmsg -s -t 2-

# Toggle Tag 2
mmsg -s -t 2^
```

### Layouts

Layout codes: `S` (Scroller), `T` (Tile), `G` (Grid), `M` (Monocle), `K` (Deck), `CT` (Center Tile), `RT` (Right Tile), `VS` (Vertical Scroller), `VT` (Vertical Tile), `VG` (Vertical Grid), `VK` (Vertical Deck), `TG` (TGMix).

```bash
mmsg -l "S"   # Scroller
mmsg -l "T"   # Tile
```

### Dispatching commands

Any command available in `config.conf` keybindings can be run via IPC.

```bash
mmsg -d killclient
mmsg -d resizewin,+10,0
mmsg -d togglefullscreen
mmsg -d disable_monitor,eDP-1
```

### Monitoring & status

```bash
mmsg -w           # Watch all changes
mmsg -g           # Get everything
mmsg -w -c        # Watch focused client appid + title
mmsg -O           # All outputs
mmsg -g -t        # Tag info
mmsg -g -k        # Keyboard layout
mmsg -g -b        # Current keymode
```

#### Tag message format

State: 0 → none, 1 → active, 2 → urgent.

| Monitor | Tag Number | Tag State | Clients | Focused |
|---------|------------|-----------|---------|---------|
| eDP-1   | tag 2      | 0         | 1       | 0       |

| Monitor | Occupied mask | Active mask | Urgent mask |
|---------|---------------|-------------|-------------|
| eDP-1   | 14            | 6           | 0           |

## NoirWM-specific dispatchers

These exist in NoirWM but not in upstream MangoWC.

### Marks (Vim-style)

| Dispatch | Args | Description |
| :--- | :--- | :--- |
| `mark` | `name` | Bind the focused client to a named mark. |
| `focus_mark` | `name` | Focus the client bound to the mark. Cursor warps onto the window. |
| `focus_mark_or_set` | `name` | If the mark exists, focus it; otherwise, save the focused window to it. One bind = save+jump. |
| `swap_with_mark` | `name` | Swap the focused window's tile slot with the marked window's slot. |

See [Window Marks](/docs/window-management/marks) for full usage.

### State dump

| Dispatch | Args | Description |
| :--- | :--- | :--- |
| `dumpclients` | `[path]` | Write a JSON array of all clients to `path` (default `/tmp/noir_clients.json`). |
| `dumpmarks` | `[path]` | Write a JSON array of all marks to `path` (default `/tmp/noir_marks.json`). |

When `auto_dump_clients=1` / `auto_dump_marks=1` is set in the config, NoirWM auto-emits these files on every relevant state change. See [Auto-dump JSON](/docs/configuration/auto-dump).

### Silent tag/view variants

| Dispatch | Args | Description |
| :--- | :--- | :--- |
| `tagsilent` | `1-9` | Move the focused window to a tag without focusing it. |
| `tagtoleftsilent` | `[synctag]` | Move window to left tag silently. |
| `tagtorightsilent` | `[synctag]` | Move window to right tag silently. |
| `viewtoleftsilent` | `[synctag]` | View previous tag without re-focusing. |
| `viewtorightsilent` | `[synctag]` | View next tag without re-focusing. |

### Layout cycling

| Dispatch | Args | Description |
| :--- | :--- | :--- |
| `switch_layout_prev` | - | Cycle to the previous layout (companion to `switch_layout`). |

### Inactive dim

`dim_inactive=1` and `dim_strength=0.3` (config keys) darken unfocused clients via overlay rect — independent of `unfocused_opacity`. See [Effects](/docs/visuals/effects).

## Virtual monitors

Headless outputs for screen mirroring or remote desktop access (Sunshine/Moonlight):

```bash
mmsg -d create_virtual_output                                            # create
wlr-randr --output HEADLESS-1 --pos 1920,0 --mode 1920x1080@60Hz         # configure
mmsg -d destroy_all_virtual_output                                       # destroy all
```

# NoirWM — Wayland compositor

NoirWM is a Wayland compositor tuned for external shells. It's a fork of [MangoWC](https://github.com/mangowm/mango) (which itself derives from [dwl](https://codeberg.org/dwl/dwl/)) with marks, IPC state dumps, live toplevel previews, and a handful of new protocols. Pairs with [Quickshell](https://quickshell.outfoxxed.me/).

The binary is `noir`; the dispatcher CLI is `mmsg` (kept from upstream).

## Heritage

This project carries forward upstream MangoWC's design and credits its author, DreamMaoMao. Where this fork diverges, it diverges deliberately — see the commit log for the trail.

## Highlights

- Lightweight & fast build — builds in seconds without compromising on functionality.
- Smooth and customizable animations (window/tag/layer open/move/close).
- Flexible window layouts: scroller, master-stack, monocle, center-master, grid, etc.
- Rich window states: swallow, minimize, maximize, unglobal, global, fakefullscreen, overlay.
- External configuration with hot reload.
- Sway-like scratchpad and named scratchpads.
- Window effects via [scenefx](https://github.com/wlrfx/scenefx) (blur, shadow, corner radius, opacity).
- IPC via `mmsg` (`zdwl_ipc_v2` protocol) for external scripts and shells.
- **Marks** — Vim-like window marks for jump-back and quick-switch.
- **Auto-dumped JSON state** at `/tmp/noir_clients.json` and `/tmp/noir_marks.json` for inotify-driven external UIs.
- **Live toplevel previews** via `hyprland-toplevel-export-v1` server-side implementation.
- Excellent XWayland support; tag-per-monitor; tearing/`WLR_DRM_NO_ATOMIC` support; X11/Wayland input methods.

## Supported layouts

`tile`, `scroller`, `monocle`, `grid`, `deck`, `center_tile`, `vertical_tile`, `vertical_grid`, `vertical_scroller`, `tgmix`, `tabbed`.

## Build from source

Dependencies: wayland, wayland-protocols, libinput, libdrm, libxkbcommon, pixman, libdisplay-info, libliftoff, hwdata, seatd, pcre2, xorg-xwayland, libxcb, scenefx 0.4.x, wlroots 0.19.x.

```bash
meson setup build --buildtype=release
ninja -C build
sudo ninja -C build install
```

`build/noir` is the compositor binary; `build/mmsg` is the dispatcher.

## NixOS + home-manager

This repo provides `nixosModules.noir` and `hmModules.noir` plus an overlay attribute `noir`.

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    home-manager.url = "github:nix-community/home-manager";
    home-manager.inputs.nixpkgs.follows = "nixpkgs";
    flake-parts.url = "github:hercules-ci/flake-parts";
    noirwm.url = "github:waliori/noirwm";
  };

  outputs = inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" ];
      flake.nixosConfigurations.hostname = inputs.nixpkgs.lib.nixosSystem {
        system = "x86_64-linux";
        modules = [
          inputs.home-manager.nixosModules.home-manager
          inputs.noirwm.nixosModules.noir
          { programs.noir.enable = true; }
          {
            home-manager = {
              useGlobalPkgs = true;
              useUserPackages = true;
              users."username".imports = [
                ({ ... }: {
                  wayland.windowManager.noir = {
                    enable = true;
                    settings = ''
                      # see assets/config.conf
                    '';
                    autostart_sh = ''
                      # commands to run on session start
                    '';
                  };
                })
                inputs.noirwm.hmModules.noir
              ];
            };
          }
        ];
      };
    };
}
```

## Suggested companion tools

- Application launcher: rofi, bemenu, wmenu, fuzzel
- Terminal: foot, kitty, ghostty, wezterm, alacritty
- Status bar / shell: **Quickshell** (preferred), waybar, eww, ags
- Wallpaper: swww, swaybg
- Notifications: swaync, dunst, mako
- Portals: xdg-desktop-portal, xdg-desktop-portal-wlr, xdg-desktop-portal-gtk
- Clipboard: wl-clipboard, wl-clip-persist, cliphist
- Gamma / night light: wlsunset, gammastep
- Misc: xfce-polkit, wlogout

## Default keybindings (sample)

- `alt+Return` — terminal
- `alt+space` — launcher
- `alt+q` — kill client
- `alt+arrows` — focus direction
- `super+m` — quit noir

See `assets/config.conf` for the complete starter config.

## Config documentation

Configuration syntax follows MangoWC's; the upstream wiki at <https://github.com/mangowm/mango/wiki/> remains a useful reference. Where noir differs from upstream, this repo's commits are the source of truth.

## Thanks

- [DreamMaoMao](https://github.com/mangowm/mango) — upstream MangoWC.
- [dwl](https://codeberg.org/dwl/dwl/) — the dwl Wayland compositor.
- [wlroots](https://gitlab.freedesktop.org/wlroots/wlroots) — Wayland protocol implementation.
- [scenefx](https://github.com/wlrfx/scenefx) — visual effects.

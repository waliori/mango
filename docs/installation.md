---
title: Installation
description: Build NoirWM from source or consume the Nix flake.
---

NoirWM is a personal fork without distro packaging. Two paths: build from source, or import the Nix flake.

## Build from source

> **Info:** Required dependencies:
>
> - `wayland`, `wayland-protocols`, `wayland-scanner`
> - `wlroots` (0.19.x)
> - `scenefx` (0.4.x — vendored as a meson subproject; no system install needed)
> - `libinput`, `libdrm`, `libxkbcommon`, `pixman`
> - `libdisplay-info`, `libliftoff`, `hwdata`, `seatd`
> - `pcre2`
> - `xorg-xwayland`, `libxcb` (for XWayland support)

```bash
git clone https://github.com/waliori/noirwm.git
cd noirwm
meson setup build --buildtype=release
ninja -C build
sudo ninja -C build install
```

After install:

- Compositor binary: `/usr/local/bin/noir`
- Dispatcher CLI: `/usr/local/bin/mmsg`
- Default config: `/etc/noir/config.conf`
- Wayland session: `/usr/local/share/wayland-sessions/noir.desktop`

> **Note:** `wlroots` and `scenefx` are sensitive to version. NoirWM is pinned to `wlroots 0.19` + `scenefx 0.4.1` with a PR #154 backport (vendored in `subprojects/scenefx/`). Don't use system `scenefx` unless you've confirmed the backport is present.

---

## NixOS via flake

The repo provides `nixosModules.noir` and `hmModules.noir`.

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
            home-manager.useGlobalPkgs = true;
            home-manager.users."username".imports = [
              ({ ... }: {
                wayland.windowManager.noir = {
                  enable = true;
                  settings = ''
                    # see assets/config.conf for the starter
                  '';
                  autostart_sh = ''
                    # commands to run at session start
                  '';
                };
              })
              inputs.noirwm.hmModules.noir
            ];
          }
        ];
      };
    };
}
```

### Module options

- `programs.noir.enable` — flips on the system module (Wayland session, portals, polkit).
- `programs.noir.package` — override the noirwm package (e.g. for a custom build).
- `wayland.windowManager.noir.enable` — flips on the home-manager half (Nix-side config generation, autostart helper).
- `wayland.windowManager.noir.settings` — structured Nix config (converted to `~/.config/noir/config.conf`). See `nix/lib.nix` for `toNoir` semantics.
- `wayland.windowManager.noir.extraConfig` — raw config-file lines appended after the structured output.
- `wayland.windowManager.noir.autostart_sh` — content of `~/.config/noir/autostart.sh`; an `exec-once` line for it is auto-added to the config.

---

## Verify

After install + session pick at the display manager, log in. You should land in noir. From a terminal inside the session:

```bash
echo "$XDG_CURRENT_DESKTOP $XDG_SESSION_DESKTOP"   # both 'noir'
noir -v                                            # prints noirwm <version>
mmsg -d dumpclients                                # writes /tmp/noir_clients.json
```

If `mmsg` complains about XDG variables, your session env didn't propagate — see [XDG Portals](/docs/configuration/xdg-portals).

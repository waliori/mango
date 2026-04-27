{
  description = "NoirWM — Wayland compositor tuned for external shells (fork of MangoWC)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    # scenefx is vendored at subprojects/scenefx/ and consumed via meson
    # subproject. No external flake input needed.
  };

  outputs = {
    self,
    flake-parts,
    ...
  } @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [
        inputs.flake-parts.flakeModules.easyOverlay
      ];

      flake = {
        hmModules.noir = import ./nix/hm-modules.nix self;
        nixosModules.noir = import ./nix/nixos-modules.nix self;
      };

      perSystem = {
        config,
        pkgs,
        ...
      }: let
        inherit (pkgs) callPackage ;
        noir = callPackage ./nix {};
        shellOverride = old: {
          nativeBuildInputs = old.nativeBuildInputs ++ [];
          buildInputs = old.buildInputs ++ [];
        };
      in {
        packages.default = noir;
        overlayAttrs = {
          inherit (config.packages) noir;
        };
        packages = {
          inherit noir;
        };
        devShells.default = noir.overrideAttrs shellOverride;
        formatter = pkgs.alejandra;
      };
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
    };
}

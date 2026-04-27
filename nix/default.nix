{
  lib,
  libX11,
  libdrm,
  libgbm,
  libinput,
  libxcb,
  libxkbcommon,
  pcre2,
  pixman,
  pkg-config,
  stdenv,
  wayland,
  wayland-protocols,
  wayland-scanner,
  libxcb-wm,
  xwayland,
  meson,
  ninja,
  wlroots_0_19,
  libGL,
  enableXWayland ? true,
  debug ? false,
}:
stdenv.mkDerivation {
  pname = "noirwm";
  version = "nightly";

  src = builtins.path {
    path = ../.;
    name = "source";
    # Keep local `meson setup build`/`result`/caches out of the sandbox —
    # a pre-existing build/ dir makes mesonConfigurePhase refuse flags.
    filter = path: type:
      let base = baseNameOf path;
      in !(type == "directory" && (base == "build" ||
                                   base == "result" ||
                                   base == ".cache" ||
                                   base == ".git"));
  };

  mesonFlags = [
    (lib.mesonEnable "xwayland" enableXWayland)
    (lib.mesonBool "asan" debug)
  ];

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
    wayland-scanner
  ];

  buildInputs =
    [
      libdrm
      libgbm
      libinput
      libxcb
      libxkbcommon
      pcre2
      pixman
      wayland
      wayland-protocols
      wlroots_0_19
      libGL
    ]
    ++ lib.optionals enableXWayland [
      libX11
      libxcb-wm
      xwayland
    ];

  passthru = {
    providedSessions = ["noir"];
  };

  meta = {
    mainProgram = "noir";
    description = "NoirWM — Wayland compositor tuned for external shells (fork of MangoWC)";
    homepage = "https://github.com/waliori/noirwm";
    license = lib.licenses.gpl3Plus;
    maintainers = [];
    platforms = lib.platforms.unix;
  };
}

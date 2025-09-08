{
  stdenv,
  lib,
  cmake,
  pkg-config,
  ninja,
  fetchNpmDeps,
  npmHooks,
  autoPatchelfHook,

  qt6,
  kdePackages,

  wayland,
  minizip,
  rapidfuzz-cpp,
  protobuf,
  grpc-tools,
  nodejs,
  cmark-gfm,
  libqalculate,

}:
let
  src = ./.;

  manifestRaw = builtins.readFile (src + /manifest.yaml);

  get =
    key:
    let
      m = builtins.match ".*${key}:[[:space:]]*\"([^\"]+)\".*" manifestRaw;
    in
    if m == null then throw "Key ${key} not found in manifest.yaml" else builtins.elemAt m 0;

  manifest = {
    tag = get "tag";
    rev = get "rev";
    short_rev = get "short_rev";
  };

  # Prepare node_modules for api folder
  apiDeps = fetchNpmDeps {
    src = src + /api;
    hash = "sha256-7rsaGjs1wMe0zx+/BD1Mx7DQj3IAEZQvdS768jVLl3E=";
  };

  # Prepare node_modules for extension-manager folder
  extensionManagerDeps = fetchNpmDeps {
    src = src + /extension-manager;
    hash = "sha256-zoTe/n7PmC7h3bEYFX8OtLKr6T8WA7ijNhAekIhsgLc=";
  };

in
stdenv.mkDerivation (finalAttrs: {
  pname = "vicinae";
  version = manifest.tag;

  inherit src;
  passthru = {
    inherit apiDeps extensionManagerDeps;
  };

  cmakeFlags = [
    "-DVICINAE_GIT_TAG=${manifest.tag}"
    "-DVICINAE_GIT_COMMIT_HASH=${manifest.short_rev}"
    "-DCMAKE_INSTALL_PREFIX=${placeholder "out"}"
    "-DCMAKE_INSTALL_DATAROOTDIR=share"
    "-DCMAKE_INSTALL_BINDIR=bin"
    "-DCMAKE_INSTALL_LIBDIR=lib"
  ];

  nativeBuildInputs = [
    cmake
    ninja
    nodejs
    pkg-config
    qt6.wrapQtAppsHook
    autoPatchelfHook
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtsvg
    qt6.qtwayland

    protobuf
    rapidfuzz-cpp
    wayland
    kdePackages.qtkeychain
    kdePackages.layer-shell-qt
    grpc-tools
    protobuf
    nodejs
    minizip
    cmark-gfm
    libqalculate
  ];

  postPatch = ''
    local postPatchHooks=()
    	source ${npmHooks.npmConfigHook}/nix-support/setup-hook
    	npmRoot=api npmDeps=${apiDeps} npmConfigHook
    	npmRoot=extension-manager npmDeps=${extensionManagerDeps} npmConfigHook
  '';

  postFixup = ''
    wrapProgram $out/bin/vicinae \
    --prefix PATH : ${
      lib.makeBinPath [
        nodejs
        (placeholder "out")
      ]
    }
  '';

  meta = {
    description = "A raycast-like, high-performance, native launcher for your desktop";
    longDescription = ''
      Vicinae (pronounced "vih-SIN-ay") is a high-performance, native launcher for your desktop — built with C++ and Qt.

      It includes a set of built-in modules, and extensions can be developed quickly using fully server-side React/TypeScript — with no browser or Electron involved.

      Inspired by the popular Raycast launcher, Vicinae provides a mostly compatible extension API, allowing reuse of many existing Raycast extensions with minimal modification.

      Vicinae is designed for developers and power users who want fast, keyboard-first access to common system actions — without unnecessary overhead.
    '';
    license = lib.licenses.gpl3;
    mainProgram = "vicinae";
    homepage = "https://github.com/vicinaehq/vicinae";
    # maintainers = with lib.maintainers [];
  };
})

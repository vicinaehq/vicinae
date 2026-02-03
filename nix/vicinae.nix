{
  abseil-cpp,
  cmake,
  cmark-gfm,
  fetchNpmDeps,
  kdePackages,
  lib,
  libqalculate,
  minizip,
  ninja,
  nodejs,
  npmHooks,
  pkg-config,
  protobuf,
  qt6,
  gcc15Stdenv,
  wayland,
  libxml2,
  glaze,
}:
let
  manifestRaw = builtins.readFile ../manifest.yaml;
  manifestGet =
    key:
    let
      m = builtins.match ".*${key}:[[:space:]]*\"([^\"]+)\".*" manifestRaw;
    in
    if m == null then throw "Key ${key} not found in manifest.yaml" else builtins.elemAt m 0;
in
gcc15Stdenv.mkDerivation (finalAttrs: {
  pname = "vicinae";
  version = lib.removePrefix "v" (manifestGet "tag");

  src = ../.;

  apiDeps = fetchNpmDeps {
    src = "${finalAttrs.src}/src/typescript/api";
    hash = "sha256-UsTpMR23UQBRseRo33nbT6z/UCjZByryWfn2AQSgm6U=";
  };

  extensionManagerDeps = fetchNpmDeps {
    src = "${finalAttrs.src}/src/typescript/extension-manager";
    hash = "sha256-wl8FDFB6Vl1zD0/s2EbU6l1KX4rwUW6dOZof4ebMMO8=";
  };

  cmakeFlags = lib.mapAttrsToList lib.cmakeFeature {
    "VICINAE_GIT_TAG" = "v${finalAttrs.version}";
    "VICINAE_GIT_COMMIT_HASH" = manifestGet "short_rev";
    "VICINAE_PROVENANCE" = "nix";
    "INSTALL_NODE_MODULES" = "OFF";
    "USE_SYSTEM_GLAZE" = "ON";
    "CMAKE_INSTALL_PREFIX" = placeholder "out";
    "CMAKE_INSTALL_DATAROOTDIR" = "share";
    "CMAKE_INSTALL_BINDIR" = "bin";
    "CMAKE_INSTALL_LIBDIR" = "lib";
    "INSTALL_BROWSER_NATIVE_HOST" = "OFF";
  };

  strictDeps = true;

  nativeBuildInputs = [
    cmake
    ninja
    nodejs
    pkg-config
    protobuf
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    abseil-cpp
    cmark-gfm
    kdePackages.layer-shell-qt
    kdePackages.qtkeychain
    libqalculate
    minizip
    nodejs
    protobuf
    qt6.qtbase
    qt6.qtsvg
    qt6.qtwayland
    wayland
    libxml2
    glaze
  ];

  postPatch = ''
    local postPatchHooks=()
    source ${npmHooks.npmConfigHook}/nix-support/setup-hook
    npmRoot=src/typescript/api npmDeps=${finalAttrs.apiDeps} npmConfigHook
    npmRoot=src/typescript/extension-manager npmDeps=${finalAttrs.extensionManagerDeps} npmConfigHook
  '';

  qtWrapperArgs = [
    "--prefix PATH : ${
      lib.makeBinPath [
        nodejs
        (placeholder "out")
      ]
    }"
  ];

  meta = {
    description = "A focused launcher for your desktop — native, fast, extensible";
    homepage = "https://github.com/vicinaehq/vicinae";
    license = lib.licenses.gpl3Plus;
    platforms = lib.platforms.linux;
    mainProgram = "vicinae";
  };
})

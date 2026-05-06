{
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
    hash = "sha256-lIXhMBJHujs6d9fXEK8Q+sfjkKyFJEMEtKrQorkfPeU=";
  };

  extensionManagerDeps = fetchNpmDeps {
    src = "${finalAttrs.src}/src/typescript/extension-manager";
    hash = "sha256-gpbS6MIHOSuHIfd4zDEB4EcMi9LHk9tPdnxwT0S0nbA=";
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
    qt6.wrapQtAppsHook
  ];

  buildInputs = [
    cmark-gfm
    kdePackages.layer-shell-qt
    kdePackages.qtkeychain
	kdePackages.qtshadertools
    kdePackages.syntax-highlighting
    libqalculate
    minizip
    nodejs
    qt6.qtbase
    qt6.qtdeclarative
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

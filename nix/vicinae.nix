{
  cmake,
  cmark-gfm,
  fetchNpmDeps,
  kdePackages,
  lib,
  libqalculate,
  llvmPackages_21,
  ninja,
  nodejs,
  npmHooks,
  pkg-config,
  qt6,
  stdenv,
  gcc15Stdenv,
  wayland,
  glaze,
  swift ? null,
  apple-sdk ? null,
}: let
  inherit (stdenv.hostPlatform) isLinux isDarwin;

  effectiveStdenv =
    if isLinux
    then gcc15Stdenv
    else llvmPackages_21.libcxxStdenv;

  # syntax-highlighting (and its extra-cmake-modules dep) is tagged Linux-only in
  # nixpkgs but builds fine on macOS; lift the platform tag so we can link the
  # system library instead of vendoring it. No-op on Linux.
  addDarwinPlatform = drv:
    drv.overrideAttrs (old: {
      meta = old.meta // {platforms = old.meta.platforms ++ lib.platforms.darwin;};
    });
  kdeScope = kdePackages.overrideScope (_: prev: {
    extra-cmake-modules = addDarwinPlatform prev.extra-cmake-modules;
  });
  syntax-highlighting = addDarwinPlatform kdeScope.syntax-highlighting;

  manifestRaw = builtins.readFile ../manifest.yaml;
  manifestGet = key: let
    m = builtins.match ".*${key}:[[:space:]]*\"([^\"]+)\".*" manifestRaw;
  in
    if m == null
    then throw "Key ${key} not found in manifest.yaml"
    else builtins.elemAt m 0;
in
  effectiveStdenv.mkDerivation (finalAttrs: {
    pname = "vicinae";
    version = lib.removePrefix "v" (manifestGet "tag");

    src = ../.;

    apiDeps = fetchNpmDeps {
      src = "${finalAttrs.src}/src/typescript/api";
      hash = "sha256-Ki/l3PiBY3R0Bzd6leqx2OxA7c+jckjr+YD4GHHaSqI=";
    };

    extensionManagerDeps = fetchNpmDeps {
      src = "${finalAttrs.src}/src/typescript/extension-manager";
      hash = "sha256-6Kz7I8cGm1lnGPOI/gju3t5/imnbBFlDEKzWar5O770=";
    };

    cmakeFlags = lib.mapAttrsToList lib.cmakeFeature {
      "VICINAE_GIT_TAG" = "v${finalAttrs.version}";
      "VICINAE_GIT_COMMIT_HASH" = manifestGet "short_rev";
      "VICINAE_PROVENANCE" = "nix";
      "INSTALL_NODE_MODULES" = "OFF";
      "USE_SYSTEM_CMARK_GFM" = "ON";
      "USE_SYSTEM_GLAZE" = "ON";
      "USE_SYSTEM_KF6" = "ON";
      "USE_SYSTEM_QT_KEYCHAIN" = "ON";
      "CMAKE_INSTALL_PREFIX" = placeholder "out";
      "CMAKE_INSTALL_DATAROOTDIR" = "share";
      "CMAKE_INSTALL_BINDIR" = "bin";
      "CMAKE_INSTALL_LIBDIR" = "lib";
      "INSTALL_BROWSER_NATIVE_HOST" = "OFF";
    };

    strictDeps = true;

    nativeBuildInputs =
      [
        cmake
        ninja
        nodejs
        pkg-config
        qt6.wrapQtAppsHook
      ]
      ++ lib.optionals isDarwin [
        swift
      ];

    buildInputs =
      [
        cmark-gfm
        kdePackages.qtkeychain
        kdePackages.qtshadertools
        syntax-highlighting
        libqalculate
        nodejs
        qt6.qtbase
        qt6.qtdeclarative
        qt6.qtimageformats
        qt6.qtsvg
        glaze
      ]
      ++ lib.optionals isLinux [
        kdePackages.layer-shell-qt
        qt6.qtwayland
        wayland
      ]
      ++ lib.optionals isDarwin [
        apple-sdk
      ];

    postPatch = ''
      local postPatchHooks=()
      source ${npmHooks.npmConfigHook}/nix-support/setup-hook
      npmRoot=src/typescript/api npmDeps=${finalAttrs.apiDeps} npmConfigHook
      npmRoot=src/typescript/extension-manager npmDeps=${finalAttrs.extensionManagerDeps} npmConfigHook
    '';

    # On macOS CMake installs only the CLI; assemble a thin .app bundle for the
    # GUI server. Both binaries go in the bundle so the CLI resolves the server
    # as a sibling (findServerBinary); $out/bin/vicinae is re-added in postFixup.
    postInstall = lib.optionalString isDarwin ''
      app=$out/Applications/Vicinae.app
      install -Dm755 bin/vicinae-server "$app/Contents/MacOS/Vicinae"
      install -Dm755 bin/vicinae "$app/Contents/MacOS/vicinae-cli"
      install -Dm644 Info.plist "$app/Contents/Info.plist"
      install -Dm644 ../extra/vicinae.icns "$app/Contents/Resources/vicinae.icns"
      cp -r ../extra/themes "$app/Contents/Resources/themes"
      rm -f "$out/bin/vicinae"
    '';

    # Symlink the CLI onto PATH after wrapQtAppsHook runs, so the hook doesn't
    # double-wrap it and the CLI still resolves from inside the bundle.
    postFixup = lib.optionalString isDarwin ''
      ln -s ../Applications/Vicinae.app/Contents/MacOS/vicinae-cli "$out/bin/vicinae"
    '';

    qtWrapperArgs =
      [
        "--prefix PATH : ${
          lib.makeBinPath [
            nodejs
            (placeholder "out")
          ]
        }"
      ]
      ++ lib.optionals isLinux [
        "--set VICINAE_INPUT_SERVER_BIN /run/wrappers/bin/vicinae-input-server"
      ];

    meta = {
      description = "A focused launcher for your desktop — native, fast, extensible";
      homepage = "https://github.com/vicinaehq/vicinae";
      license = lib.licenses.gpl3Plus;
      platforms = with lib.platforms; linux ++ darwin;
      mainProgram = "vicinae";
    };
  })

{
  stdenv,
  fetchFromGitHub,
  cmake,
  pkg-config,
  kdePackages,
  rapidfuzz-cpp,
  protobuf,
  grpc-tools,
  nodejs,
  minizip-ng,
  cmark-gfm,
  libqalculate,
  ninja,
  lib,
  fetchNpmDeps,
  protoc-gen-js,
  rsync,
  which,
  autoPatchelfHook,
  writeShellScriptBin,
  minizip,
  qt6,
  typescript,
  wayland,
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
    hash = "sha256-dSHEzw15lSRRbldl9PljuWFf2htdG+HgSeKPAB88RBg=";
  };
  ts-protoc-gen-wrapper = writeShellScriptBin "protoc-gen-ts_proto" ''
    exec node /build/source/vicinae-upstream/api/node_modules/.bin/protoc-gen-ts_proto
  '';

  # Prepare node_modules for extension-manager folder
  extensionManagerDeps = fetchNpmDeps {
    src = src + /extension-manager;
    hash = "sha256-JXyQJfPFHXOhYGSqRdDzGGEZVlFrX97T/WNQaeYdEAk=";
  };
in
stdenv.mkDerivation rec {
  name = "vicinae";

  inherit src;

  cmakeFlags = [
    "-DVICINAE_GIT_TAG=${manifest.tag}"
    "-DVICINAE_GIT_COMMIT_HASH=${manifest.short_rev}"
    "-DCMAKE_INSTALL_PREFIX=${placeholder "out"}"
    "-DCMAKE_INSTALL_DATAROOTDIR=share"
    "-DCMAKE_INSTALL_BINDIR=bin"
    "-DCMAKE_INSTALL_LIBDIR=lib"
  ];

  nativeBuildInputs = [
    ts-protoc-gen-wrapper
    extensionManagerDeps
    autoPatchelfHook
    cmake
    ninja
    nodejs
    pkg-config
    qt6.wrapQtAppsHook
    rapidfuzz-cpp
    protoc-gen-js
    protobuf
    grpc-tools
    which
    rsync
    typescript
  ];

  buildInputs = [
    qt6.qtbase
    qt6.qtsvg
    qt6.qttools
    qt6.qtwayland
    qt6.qtdeclarative
    qt6.qt5compat
    wayland
    kdePackages.qtkeychain
    kdePackages.layer-shell-qt
    minizip
    grpc-tools
    protobuf
    nodejs
    minizip-ng
    cmark-gfm
    libqalculate
  ];

  configurePhase = ''
    cmake -G Ninja -B build $cmakeFlags
  '';

  buildPhase = ''
    buildDir=$PWD
    echo $buildDir
    export npm_config_cache=${apiDeps}
    cd $buildDir/api
    npm i --ignore-scripts
    patchShebangs $buildDir/api
    npm rebuild --foreground-scripts
    export npm_config_cache=${extensionManagerDeps}
    cd $buildDir/extension-manager
    npm i --ignore-scripts
    patchShebangs $buildDir/extension-manager
    npm rebuild --foreground-scripts
    cd $buildDir
    substituteInPlace cmake/ExtensionApi.cmake cmake/ExtensionManager.cmake --replace "COMMAND npm install" ""
    cmake --build build
    cd $buildDir
  '';

  dontWrapQtApps = true;
  preFixup = ''
    wrapQtApp "$out/bin/vicinae" --prefix LD_LIBRARY_PATH : ${lib.makeLibraryPath buildInputs}
  '';
  postFixup = ''
    wrapProgram $out/bin/vicinae \
    --prefix PATH : ${
      lib.makeBinPath [
        nodejs
        qt6.qtwayland
        wayland
        (placeholder "out")
      ]
    }
  '';

  installPhase = ''
    cmake --install build
  '';

  meta = with lib; {
    description = "A focused launcher for your desktop — native, fast, extensible";
    homepage = "https://github.com/vicinaehq/vicinae";
    license = licenses.gpl3Plus;
    mainProgram = "vicinae";
  };
  passthru = {
    inherit apiDeps extensionManagerDeps;
  };
}

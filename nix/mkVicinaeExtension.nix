{
  pkgs,
  name,
  src,
}:
pkgs.buildNpmPackage {
  inherit name src;
  installPhase = ''
    runHook preInstall

    mkdir -p $out
    cp -r /build/.local/share/vicinae/extensions/${name}/* $out/

    runHook postInstall
  '';
  npmDeps = pkgs.importNpmLock { npmRoot = src; };
  npmConfigHook = pkgs.importNpmLock.npmConfigHook;
}

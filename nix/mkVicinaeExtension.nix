{
  lib,
  buildNpmPackage,
  importNpmLock,
}:
{
  name,
  src,
  # Keep to prevent breaking api and provide warning.
  # TODO: throw after 26.05, remove after 26.11 NixOS release
  pkgs ? null,
}:
lib.warnIf (pkgs != null) "passing pkgs to mkVicinaeExtension is deprecated and no longer necessary"
  (buildNpmPackage {
    inherit name src;
    npmDeps = importNpmLock { npmRoot = src; };
    npmConfigHook = importNpmLock.npmConfigHook;
    # NOTE: using buildPhase because $out doesn't work with npmBuildFlags
    buildPhase = "npm run build -- --out=$out";
    dontNpmInstall = true;
  })

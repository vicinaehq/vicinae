{
  pkgs,
  name,
  src,
}:
pkgs.buildNpmPackage {
  inherit name src;
  npmDeps = pkgs.importNpmLock { npmRoot = src; };
  npmConfigHook = pkgs.importNpmLock.npmConfigHook;
  # NOTE: using buildPhase because $out doesn't work with npmBuildFlags
  buildPhase = "npm run build -- --out=$out";
  dontNpmInstall = true;
}

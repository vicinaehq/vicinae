{
  lib,
  buildNpmPackage,
  importNpmLock,
}:
lib.extendMkDerivation {
  constructDrv = buildNpmPackage;

  extendDrvArgs =
    _:
    {
      version ? lib.warn "mkVicinaeExtension: not including version is deprecated" "0",
      src,
      ...
    }:
    {
      inherit version;
      npmDeps = importNpmLock { npmRoot = src; };
      npmConfigHook = importNpmLock.npmConfigHook;
      # NOTE: using buildPhase because $out doesn't work with npmBuildFlags
      buildPhase = "npm run build -- --out=$out";
      dontNpmInstall = true;
    };
}

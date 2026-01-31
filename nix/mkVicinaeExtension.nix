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
    }@attrs:
    {
      inherit version;
      npmDeps = attrs.npmDeps or (importNpmLock { npmRoot = src; });
      npmConfigHook = attrs.npmConfigHook or importNpmLock.npmConfigHook;
      # NOTE: using buildPhase because $out doesn't work with npmBuildFlags
      buildPhase = attrs.buildPhase or "npm run build -- --out=$out";
      dontNpmInstall = attrs.dontNpmInstall or true;
    };
}

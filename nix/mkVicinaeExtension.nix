{
  lib,
  buildNpmPackage,
  importNpmLock,
}:
{
  pname ? lib.warn "mkVicinaeExtension: name has been replaced with pname" name,
  version ? "0",
  src,
  # Keep to prevent breaking api and provide warnings.
  # TODO: throw after 26.05, remove after 26.11 NixOS release
  name ? lib.throwIf (pname == null) "mkVicinaeExtension: must be called with pname" null,
  pkgs ? null,
  ...
}@args:
lib.warnIf (pkgs != null)
  "mkVicinaeExtension: calling with pkgs is deprecated and no longer necessary"
  (
    buildNpmPackage (
      {
        inherit pname version;
        npmDeps = importNpmLock { npmRoot = src; };
        npmConfigHook = importNpmLock.npmConfigHook;
        # NOTE: using buildPhase because $out doesn't work with npmBuildFlags
        buildPhase = "npm run build -- --out=$out";
        dontNpmInstall = true;
      }
      // builtins.removeAttrs args [
        "pname"
        "version"
        "name"
        "pkgs"
      ]
    )
  )

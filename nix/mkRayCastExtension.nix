{
  lib,
  buildNpmPackage,
  importNpmLock,
  fetchgit,
}:
lib.extendMkDerivation {
  constructDrv = buildNpmPackage;

  extendDrvArgs =
    finalAttrs:
    {
      name,
      rev,
      sha256,
      ...
    }:
    {
      inherit name;
      src =
        fetchgit {
          inherit rev sha256;
          url = "https://github.com/raycast/extensions";
          sparseCheckout = [
            "/extensions/${name}"
          ];
        }
        + "/extensions/${name}";

      npmDeps = importNpmLock { npmRoot = finalAttrs.src; };
      npmConfigHook = importNpmLock.npmConfigHook;

      installPhase = ''
        runHook preInstall

        mkdir -p $out
        cp -r /build/.config/raycast/extensions/${name}/* $out/

        runHook postInstall
      '';
    };
}

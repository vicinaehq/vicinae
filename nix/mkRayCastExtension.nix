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
      hash ? null,
      sha256 ? null,
      ...
    }:
    {
      inherit name;
      src =
        fetchgit {
          inherit rev;
          hash =
            if hash != null then
              hash
            else if sha256 != null then
              sha256
            else
              throw "mkRayCastExtension: `hash` or `sha256` is required";
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

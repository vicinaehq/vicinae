{
  lib,
  buildNpmPackage,
  importNpmLock,
  fetchFromGitHub,
}:
lib.extendMkDerivation {
  constructDrv = buildNpmPackage;

  extendDrvArgs =
    finalAttrs:
    {
      name,
      hash ? null,
      sha256 ? null,
      ...
    }@attrs:
    {
      inherit name;
      src =
        attrs.src or (
          fetchFromGitHub {
            owner = "raycast";
            repo = "extensions";
            rev = attrs.rev or (throw "mkRayCastExtension: `rev` is required when src isn't suplied");
            hash =
              if hash != null then
                hash
              else if sha256 != null then
                sha256
              else
                throw "mkRayCastExtension: `hash` or `sha256` is required";
            sparseCheckout = [
              "/extensions/${name}"
            ];
          }
          + "/extensions/${name}"
        );

      npmDeps = attrs.npmDeps or (importNpmLock { npmRoot = finalAttrs.src; });
      npmConfigHook = attrs.npmConfigHook or importNpmLock.npmConfigHook;

      installPhase =
        attrs.installPhase or ''
          runHook preInstall

          mkdir -p $out
          cp -r /build/.config/raycast/extensions/${name}/* $out/

          runHook postInstall
        '';
    };
}

{
  lib,
  buildNpmPackage,
  importNpmLock,
  fetchgit,
}:
{
  name,
  sha256,
  rev,
}@args:
let
  src =
    fetchgit {
      inherit rev sha256;
      url = "https://github.com/raycast/extensions";
      sparseCheckout = [
        "/extensions/${name}"
      ];
    }
    + "/extensions/${name}";
in
buildNpmPackage (
  {
    inherit name src;
    installPhase = ''
      runHook preInstall

      mkdir -p $out
      cp -r /build/.config/raycast/extensions/${name}/* $out/

      runHook postInstall
    '';
    npmDeps = importNpmLock { npmRoot = src; };
    npmConfigHook = importNpmLock.npmConfigHook;
  }
  // builtins.removeAttrs args [
    "name"
    "sha256"
    "rev"
  ]
)

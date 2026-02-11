{
  description = "A flake for Vicinae, a high-performance native launcher for Linux.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    systems.url = "github:nix-systems/default";
  };

  nixConfig = {
    extra-substituters = [ "https://vicinae.cachix.org" ];
    extra-trusted-public-keys = [ "vicinae.cachix.org-1:1kDrfienkGHPYbkpNj1mWTr7Fm1+zcenzgTizIcI3oc=" ];
  };

  outputs =
    {
      self,
      nixpkgs,
      systems,
    }:
    let
      inherit (nixpkgs) lib;
      forEachPkgs = f: lib.genAttrs (import systems) (system: f nixpkgs.legacyPackages.${system});
    in
    {
      packages = forEachPkgs (pkgs: {
        default = pkgs.callPackage ./nix/vicinae.nix { gcc15Stdenv = pkgs.gcc15Stdenv; };
        nix-update-script = pkgs.writeShellScriptBin "nix-update-script" ''
          OLD_API_DEPS_HASH=$(${pkgs.lib.getExe pkgs.nix} eval --raw .#packages.x86_64-linux.default.apiDeps.hash)
          OLD_EXT_MAN_DEPS_HASH=$(${pkgs.lib.getExe pkgs.nix} eval --raw .#packages.x86_64-linux.default.extensionManagerDeps.hash)

          cd src/typescript/api
          NEW_API_DEPS_HASH=$(${pkgs.lib.getExe pkgs.prefetch-npm-deps} package-lock.json)
          cd ../extension-manager
          NEW_EXT_MAN_DEPS_HASH=$(${pkgs.lib.getExe pkgs.prefetch-npm-deps} package-lock.json)
          cd ..

          [[ "$OLD_API_DEPS_HASH" == "$NEW_API_DEPS_HASH" ]] || { echo -e "\e[31mHash mismatch for API npm deps, please replace the value in vicinae.nix with '$NEW_API_DEPS_HASH'.\e[0m" >&2; exit 1;}

          [[ "$OLD_EXT_MAN_DEPS_HASH" == "$NEW_EXT_MAN_DEPS_HASH" ]] || { echo -e "\e[31mHash mismatch for extension-manager npm deps, please replace the value in vicinae.nix with '$NEW_EXT_MAN_DEPS_HASH'.\e[0m" >&2; exit 1;}
        '';
        mkVicinaeExtension = pkgs.callPackage ./nix/mkVicinaeExtension.nix { };
        mkRayCastExtension = pkgs.callPackage ./nix/mkRayCastExtension.nix { };
      });
      devShells = forEachPkgs (pkgs: {
        default = pkgs.mkShell {
          # automatically pulls nativeBuildInputs + buildInputs
          inputsFrom = [ (pkgs.callPackage ./nix/vicinae.nix { gcc15Stdenv = pkgs.gcc15Stdenv; }) ];
          buildInputs = with pkgs; [
            ccache
            catch2_3
          ];

          packages = with pkgs; [
            nixd
            nixfmt-rfc-style
          ];
        };
      });
      overlays.default = final: prev: {
        vicinae = final.callPackage ./nix/vicinae.nix { };
        mkVicinaeExtension = prev.callPackage ./nix/mkVicinaeExtension.nix { };
        mkRayCastExtension = prev.callPackage ./nix/mkRayCastExtension.nix { };
      };
      homeManagerModules.default = import ./nix/module.nix self;
    };
}

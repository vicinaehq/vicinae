{
  description = "A flake for Vicinae, a high-performance native launcher for Linux.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  nixConfig = {
    extra-substituters = [ "https://vicinae.cachix.org" ];
    extra-trusted-public-keys = [ "vicinae.cachix.org-1:1kDrfienkGHPYbkpNj1mWTr7Fm1+zcenzgTizIcI3oc=" ];
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        vicinaePkg = pkgs.callPackage ./nix/vicinae.nix { };
        nix-update-script = pkgs.writeShellScriptBin "nix-update-script" ''
          OLD_API_DEPS_HASH=$(${pkgs.lib.getExe pkgs.nix} eval --raw .#packages.x86_64-linux.default.passthru.apiDeps.hash)
          OLD_EXT_MAN_DEPS_HASH=$(${pkgs.lib.getExe pkgs.nix} eval --raw .#packages.x86_64-linux.default.passthru.extensionManagerDeps.hash)

          cd typescript/api
          NEW_API_DEPS_HASH=$(${pkgs.lib.getExe pkgs.prefetch-npm-deps} package-lock.json)
          cd ../extension-manager
          NEW_EXT_MAN_DEPS_HASH=$(${pkgs.lib.getExe pkgs.prefetch-npm-deps} package-lock.json)
          cd ..

          [[ "$OLD_API_DEPS_HASH" == "$NEW_API_DEPS_HASH" ]] || { echo -e "\e[31mHash mismatch for API npm deps, please replace the value in vicinae.nix with '$NEW_API_DEPS_HASH'.\e[0m" >&2; exit 1;}

          [[ "$OLD_EXT_MAN_DEPS_HASH" == "$NEW_EXT_MAN_DEPS_HASH" ]] || { echo -e "\e[31mHash mismatch for extension-manager npm deps, please replace the value in vicinae.nix with '$NEW_EXT_MAN_DEPS_HASH'.\e[0m" >&2; exit 1;}
        '';
      in
      {
        packages.default = vicinaePkg;
        packages.nix-update-script = nix-update-script;
        mkVicinaeExtension = import ./nix/mkVicinaeExtension.nix;
        devShells.default = pkgs.mkShell {
          inputsFrom = [ vicinaePkg ]; # automatically pulls nativeBuildInputs + buildInputs
          buildInputs = [
            pkgs.ccache
          ];

          packages = with pkgs; [
            nixd
            nixfmt-rfc-style
          ];
        };
      }
    )
    // {
      overlays.default = final: prev: {
        vicinae = self.packages.${final.system}.default;
        mkVicinaeExtension = import ./nix/mkVicinaeExtension.nix;
      };
      homeManagerModules.default = import ./nix/module.nix self;
    };
}

{
  description = "A flake for Vicinae, a high-performance native launcher for Linux.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
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
        vicinaePkg = pkgs.callPackage ./package.nix {
          nix-support = (builtins.fromJSON (builtins.readFile ./nix-support.json));
        };
        nix-update-script = pkgs.writeShellScriptBin "nix-update" ''
          		set -x
          		

          		VERSION=$(${pkgs.lib.getExe pkgs.git} describe --tags --abbrev=0)

          		${pkgs.lib.getExe pkgs.nix} flake update --accept-flake-config

          		cd api
          		API_DEPS_HASH=$(${pkgs.lib.getExe pkgs.prefetch-npm-deps} package-lock.json)
          		cd ../extension-manager
          		EXTMAN_DEPS_HASH=$(${pkgs.lib.getExe pkgs.prefetch-npm-deps} package-lock.json)
          		cd ..

          		#now we output the file
          		${pkgs.lib.getExe pkgs.jq} --null-input --arg version "$VERSION" --arg adh "$API_DEPS_HASH" --arg emdh "$EXTMAN_DEPS_HASH" '{"version": $version, "apiDeps-hash": $adh, "extensionManagerDeps-hash": $emdh}' > nix-support.json
          	'';
      in
      {
        packages.default = vicinaePkg;
        packages.nix-update-script = nix-update-script;
      }
    )
    // {
      overlays.default = final: prev: {
        vicinae = self.packages.${final.system}.default;
      };
      homeManagerModules.default =
        {
          config,
          pkgs,
          lib,
          ...
        }:
        import ./module.nix {
          inherit
            config
            pkgs
            lib
            self
            ;
        };
    };
}

{
  description = "A flake for Vicinae, a high-performance native launcher for Linux.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  nixConfig = {
    extra-substituters = ["https://tomromeo.cachix.org"];
    extra-trusted-public-keys = ["tomromeo.cachix.org-1:RNtotxybU2sStsoulE4np6THs0bTw2SBtKFfKogBah0="];
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        vicinaePkg = pkgs.callPackage ./vicinae.nix { hash = self.rev ? self.dirtyRev; };
      in
      {
        packages.default = vicinaePkg;
      }
    ) // {
      overlays.default = final: prev: {
        vicinae = self.packages.${final.system}.default;
      };
      homeManagerModules.default = {config,pkgs,lib,...}: import ./module.nix {inherit config pkgs lib self;};
    };
}

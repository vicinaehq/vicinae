{ pkgs ? import <nixpkgs> {} }: {
  vicinae = pkgs.callPackage ./vicinae.nix {};
}

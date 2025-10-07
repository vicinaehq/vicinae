{
  pkgs ? import <nixpkgs> { },
}:
{
  vicinae = pkgs.callPackage ./nix/vicinae.nix { };
}

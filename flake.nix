{
  description = "A flake for Vicinae, a high-performance native launcher for Linux.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    systems.url = "github:nix-systems/default";
    soulver-cpp.url = "github:vicinaehq/soulver-cpp";
  };

  nixConfig = {
    extra-substituters = ["https://vicinae.cachix.org"];
    extra-trusted-public-keys = ["vicinae.cachix.org-1:1kDrfienkGHPYbkpNj1mWTr7Fm1+zcenzgTizIcI3oc="];
  };

  outputs = {
    self,
    nixpkgs,
    systems,
    soulver-cpp,
  }: let
    inherit (nixpkgs) lib;
    forEachPkgs = f: lib.genAttrs (import systems) (system: f nixpkgs.legacyPackages.${system});
  in {
    packages = forEachPkgs (pkgs: let
      vicinae = pkgs.callPackage ./nix/vicinae.nix {gcc15Stdenv = pkgs.gcc15Stdenv;};
      soulver = soulver-cpp.packages.${pkgs.stdenv.hostPlatform.system}.default or null;
    in
      lib.optionalAttrs (soulver != null) {
        with-soulver = pkgs.symlinkJoin {
          name = "${vicinae.name}-with-soulver";
          paths = [vicinae];
          nativeBuildInputs = [pkgs.makeWrapper];
          postBuild = ''
            for bin in $out/bin/*; do
              wrapProgram "$bin" \
                --prefix LD_LIBRARY_PATH : ${soulver}/lib \
                --prefix XDG_DATA_DIRS : ${soulver}/share
            done
          '';
          inherit (vicinae) meta;
        };
      }
      // {
        default = vicinae;
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
      });
    lib = forEachPkgs (pkgs: {
      mkVicinaeExtension = pkgs.callPackage ./nix/mkVicinaeExtension.nix {};
      mkRayCastExtension = pkgs.callPackage ./nix/mkRayCastExtension.nix {};
    });
    devShells = forEachPkgs (
      pkgs: let
        inherit (pkgs.stdenv.hostPlatform) isLinux;
        qtEnv = pkgs.qt6.env "qt-custom-${pkgs.qt6.qtbase.version}" ([
            pkgs.qt6.qtdeclarative
            pkgs.qt6.qtsvg
            pkgs.qt6.qtimageformats
            pkgs.qt6.qttools
          ]
          ++ pkgs.lib.optionals isLinux [
            pkgs.qt6.qtwayland
            pkgs.kdePackages.layer-shell-qt
          ]);
        package = self.packages.${pkgs.stdenv.hostPlatform.system}.default;
      in {
        default = pkgs.mkShell.override {stdenv = package.stdenv;} {
          # automatically pulls nativeBuildInputs + buildInputs
          inputsFrom = [package];

          packages = with pkgs; [
            ccache
            catch2_3
            qtEnv
            clang-tools
          ];

          shellHook = pkgs.lib.optionalString isLinux ''
            export CC=${pkgs.gcc15}/bin/gcc
            export CXX=${pkgs.gcc15}/bin/g++
            export CMAKE_C_COMPILER=$CC
            export CMAKE_CXX_COMPILER=$CXX

            export QML2_IMPORT_PATH=${pkgs.qt6.qtdeclarative}/lib/qt-6/qml
            export QML_IMPORT_PATH=${pkgs.qt6.qtdeclarative}/lib/qt-6/qml
          '';
        };
      }
    );
    overlays.default = final: prev: {
      vicinae = final.callPackage ./nix/vicinae.nix {};
      mkVicinaeExtension = prev.callPackage ./nix/mkVicinaeExtension.nix {};
      mkRayCastExtension = prev.callPackage ./nix/mkRayCastExtension.nix {};
    };

    homeManagerModules.default = import ./nix/home-manager-module.nix self;
    nixosModules.default = import ./nix/nixos-module.nix self;
  };
}

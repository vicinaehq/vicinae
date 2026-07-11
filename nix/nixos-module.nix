self: {
  pkgs,
  lib,
  config,
  ...
}: let
  cfg = config.programs.vicinae.input-server;
in {
  options.programs.vicinae.input-server = {
    enable = lib.mkOption {
      type = lib.types.bool;
      default = true;
      description = "Enable Vicinae's input server. Required for some functionality such as clipboard / emoji pasting and snippets.";
    };
    package = lib.mkPackageOption self.packages.${pkgs.stdenv.hostPlatform.system} "default" {
      extraDescription = "The Vicinae package to wrap.";
    };
  };

  config = lib.mkIf cfg.enable {
    security.wrappers.vicinae-input-server = {
      source = "${cfg.package}/libexec/vicinae/vicinae-input-server";
      capabilities = "cap_dac_override+ep";
      owner = "root";
      group = "root";
    };
  };
}

{
  config,
  pkgs,
  lib,
  self,
  ...
}:
let
  cfg = config.services.vicinae;
  vicinaePkg = self.outputs.packages.${pkgs.system}.default;
in {

  options.services.vicinae = {
    enable = lib.mkEnableOption "vicinae launcher daemon" // {default = false;};

    package = lib.mkOption {
      type = lib.types.package;
      default = vicinaePkg;
      defaultText = lib.literalExpression "vicinae";
      description = "The vicinae package to use";
    };

    autoStart = lib.mkOption {
      type = lib.types.bool;
      default = true;
      description = "If the vicinae daemon should be started automatically";
    };

    useLayerShell = lib.mkOption {
      type = lib.types.bool;
      default = true;
      description = "If vicinae should use the layer shell";
    };
  };
  config = lib.mkIf cfg.enable {
    home.packages = [cfg.package];

    systemd.user.services.vicinae = {
      Unit = {
        Description = "Vicinae server daemon";
        Documentation = ["https://docs.vicinae.com"];
        After = ["graphical-session.target"];
        PartOf = ["graphical-session.target"];
        BindsTo = ["graphical-session.target"];
      };
      Service = {
        EnvironmentFile = pkgs.writeText "vicinae-env" ''
          USE_LAYER_SHELL=${if cfg.useLayerShell then builtins.toString 1 else builtins.toString 0}
        '';
        Type = "simple";
        ExecStart = "${lib.getExe' cfg.package "vicinae"} server";
        Restart = "always";
        RestartSec = 5;
        KillMode = "process";
      };
      Install = lib.mkIf cfg.autoStart {
        WantedBy = ["graphical-session.target"];
      };
    };
  };
}

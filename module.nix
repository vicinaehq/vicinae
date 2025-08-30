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
      environment = {
        USE_LAYER_SHELL = if cfg.useLayerShell then 1 else 0;
      };
      Unit = {
        Description = "Vicinae server daemon";
        After = ["graphical-session.target"];
        PartOf = ["graphical-session.target"];
        BindsTo = ["graphical-session.target"];
      };
      Service = {
        Type = "simple";
        ExecStart = "${cfg.package}/bin/vicinae server";
        Restart = "on-failure";
        RestartSec = 3;
      };
      Install = lib.mkIf cfg.autoStart {
        WantedBy = ["graphical-session.target"];
      };
    };
  };
}

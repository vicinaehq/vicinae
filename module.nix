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
in
{

  options.services.vicinae = {
    enable = lib.mkEnableOption "vicinae launcher daemon" // {
      default = false;
    };

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

    themes = lib.mkOption {
      default = { };
      description = ''
        Theme settings to add to the themes folder in `~/.config/vicinae/themes`. 
        The attribute name of the theme will be the name of theme json file, 
        e.g. `base16-default-dark` will be `base16-default-dark.json`.
      '';
      example =
        lib.literalExpression # nix
          ''
            {
              base16-default-dark = {
                version = "1.0.0";
                appearance = "dark";
                icon = /path/to/icon.png;
                name = "base16 default dark";
                description = "base16 default dark by Chris Kempson";
                palette = {
                  background = "#181818";
                  foreground = "#d8d8d8";
                  blue = "#7cafc2";
                  green = "#a3be8c";
                  magenta = "#ba8baf";
                  orange = "#dc9656";
                  purple = "#a16946";
                  red = "#ab4642";
                  yellow = "#f7ca88";
                  cyan = "#86c1b9";
                };
              };
            }
          '';
      type = lib.types.attrsOf lib.types.attrs;
    };

    settings = lib.mkOption {
      type = lib.types.nullOr lib.types.attrs;
      default = null;
      description = "Settings written as JSON to `~/.config/vicinae/vicinae.json.";
      example = lib.literalExpression ''
        {
          faviconService = "twenty";
          font = {
            size = 10;
          };
          popToRootOnClose = false;
          rootSearch = {
            searchFiles = false;
          };
          theme = {
            name = "vicinae-dark";
          };
          window = {
           csd = true;
           opacity = 0.95;
           rounding = 10;
          };
        }
      '';
    };
  };

  config = lib.mkIf cfg.enable {
    home.packages = [ cfg.package ];

    xdg.configFile =
      let
        settingsFile =
          if cfg.settings == null then
            { }
          else
            {
              "vicinae/vicinae.json".text = builtins.toJSON cfg.settings;
            };
      in
      settingsFile
      // lib.mapAttrs' (
        name: theme:
        lib.nameValuePair "vicinae/themes/${name}.json" {
          text = builtins.toJSON theme;
        }
      ) cfg.themes;

    systemd.user.services.vicinae = {
      Unit = {
        Description = "Vicinae server daemon";
        Documentation = [ "https://docs.vicinae.com" ];
        After = [ "graphical-session.target" ];
        PartOf = [ "graphical-session.target" ];
        BindsTo = [ "graphical-session.target" ];
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
        WantedBy = [ "graphical-session.target" ];
      };
    };
  };
}

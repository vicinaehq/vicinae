self:
{
  config,
  pkgs,
  lib,
  ...
}:
let
  cfg = config.services.vicinae;

  inherit (pkgs.stdenv.hostPlatform) system;
  vicinaePkg = self.packages.${system}.default;

  jsonFormat = pkgs.formats.json { };
  tomlFormat = pkgs.formats.toml { };
in
{

  options.services.vicinae = {
    enable = lib.mkEnableOption "vicinae launcher daemon";

    package = lib.mkOption {
      type = lib.types.package;
      default = vicinaePkg;
      defaultText = lib.literalExpression "vicinae";
      description = "The vicinae package to use";
    };

    systemd = {
      enable = lib.mkEnableOption "vicinae systemd integration";

      autoStart = lib.mkOption {
        type = lib.types.bool;
        default = true;
        description = "If the vicinae daemon should be started automatically";
      };

      environment = lib.mkOption {
        type =
          with lib.types;
          let
            valueType = attrsOf (oneOf [
              str
              int
              float
              bool
            ]);
          in
          valueType;
        default = { };
        description = "Environment variables for the vicinae daemon. See <https://docs.vicinae.com/launcher-window#wayland-layer-shell>";
        example = lib.literalExpression ''
          {
            USE_LAYER_SHELL=1;
            QT_SCALE_FACTOR=1.5;
          }
        '';
      };

      target = lib.mkOption {
        type = lib.types.str;
        default = "graphical-session.target";
        example = "sway-session.target";
        description = ''
          The systemd target that will automatically start the vicinae service.
        '';
      };
    };

    extensions = lib.mkOption {
      type = lib.types.listOf lib.types.package;
      default = [ ];
      description = ''
        List of Vicinae extensions to install.
        You can use the `mkVicinaeExtension` function from the overlay to create extensions.
      '';
    };

    themes = lib.mkOption {
      inherit (tomlFormat) type;
      default = { };
      description = ''
        Theme settings to add to the themes folder in `~/.config/vicinae/themes`. See <https://docs.vicinae.com/theming/getting-started> for supported values.

        The attribute name of the theme will be the name of theme file,
      '';
      example =
        lib.literalExpression # nix
          ''
            {
              catppuccin-mocha = {
                meta = {
                  version = 1;
                  name = "Catppuccin Mocha";
                  description = "Cozy feeling with color-rich accents";
                  variant = "dark";
                  icon = "icons/catppuccin-mocha.png";
                  inherits = "vicinae-dark";
                };

                colors = {
                  core = {
                    background = "#1E1E2E";
                    foreground = "#CDD6F4";
                    secondary_background = "#181825";
                    border = "#313244";
                    accent = "#89B4FA";
                  };
                  accents = {
                    blue = "#89B4FA";
                    green = "#A6E3A1";
                    magenta = "#F5C2E7";
                    orange = "#FAB387";
                    purple = "#CBA6F7";
                    red = "#F38BA8";
                    yellow = "#F9E2AF";
                    cyan = "#94E2D5";
                  };
                };
              };
            }
          '';
    };

    settings = lib.mkOption {
      inherit (jsonFormat) type;
      default = { };
      description = "Settings written as JSON to `~/.config/vicinae/settings.json.";
      example = lib.literalExpression ''
        {
          close_on_focus_loss = true;
          consider_preedit = true;
          pop_to_root_on_close = true;
          favicon_service = "twenty";
          search_files_in_root = true;
          font = {
            normal = {
              size = 12;
              family = "Maple Nerd Font";
            };
          };
          theme = {
            light = {
              name = "vicinae-light";
              icon_theme = "default";
            };
            dark = {
              name = "vicinae-dark";
              icon_theme = "default";
            };
          };
          launcher_window = {
            opacity = 0.98;
          };
        }
      '';
    };
  };

  config = lib.mkIf cfg.enable {
    home.packages = [ cfg.package ];

    xdg =
      let
        themeFiles = lib.mapAttrs' (
          name: theme:
          lib.nameValuePair "vicinae/themes/${name}.toml" {
            source = tomlFormat.generate "vicinae-${name}-theme" theme;
          }
        ) cfg.themes;
      in
      {
        configFile = {
          "vicinae/settings.json" = lib.mkIf (cfg.settings != { }) {
            source = jsonFormat.generate "vicinae-settings" cfg.settings;
          };
        };

        dataFile =
          builtins.listToAttrs (
            builtins.map (item: {
              name = "vicinae/extensions/${item.name}";
              value.source = item;
            }) cfg.extensions
          )
          // themeFiles;
      };

    systemd.user.services.vicinae = lib.mkIf (cfg.systemd.enable) {
      Unit = {
        Description = "Vicinae server daemon";
        Documentation = [ "https://docs.vicinae.com" ];
        After = [ cfg.systemd.target ];
        PartOf = [ cfg.systemd.target ];
      };
      Service = {
        Environment = lib.mapAttrsToList (
          key: val:
          let
            valueStr = if lib.isBool val then (if val then "1" else "0") else toString val;
          in
          "${key}=${valueStr}"
        ) cfg.systemd.environment;
        Type = "simple";
        ExecStart = "${lib.getExe' cfg.package "vicinae"} server";
        Restart = "always";
        RestartSec = 5;
        KillMode = "process";
      };
      Install = lib.mkIf cfg.systemd.autoStart {
        WantedBy = [ cfg.systemd.target ];
      };
    };

  };
}

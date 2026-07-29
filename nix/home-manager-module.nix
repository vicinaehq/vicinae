self: {
  config,
  pkgs,
  lib,
  ...
}: let
  cfg = config.programs.vicinae;

  inherit (pkgs.stdenv.hostPlatform) system;
  soulverVicinaePkg = self.packages.${system}.with-soulver or null;
  vicinaePkg =
    if cfg.package != null
    then cfg.package
    else if cfg.enableSoulver && soulverVicinaePkg != null
    then soulverVicinaePkg
    else self.packages.${system}.default;

  jsonFormat = pkgs.formats.json {};
  tomlFormat = pkgs.formats.toml {};

  envVarType = with lib.types; attrsOf (oneOf [str int float bool]);

  envValueToString = val:
    if lib.isBool val
    then
      (
        if val
        then "1"
        else "0"
      )
    else toString val;
in {
  disabledModules = ["programs/vicinae"];

  # backwards compatibility: services.vicinae -> programs.vicinae
  imports = lib.flatten [
    (
      map (x: lib.mkRenamedOptionModule ["services" "vicinae" x] ["programs" "vicinae" x]) [
        "enable"
        "package"
        "enableFirefoxIntegration"
        "extensions"
        "themes"
        "settingOverrides"
        "settings"
      ]
    )
    (
      map (x: lib.mkRenamedOptionModule ["services" "vicinae" "systemd" x] ["programs" "vicinae" "systemd" x]) [
        "enable"
        "autoStart"
        "environment"
        "target"
      ]
    )
  ];

  options.programs.vicinae = {
    enable = lib.mkEnableOption "vicinae launcher daemon";

    package = lib.mkOption {
      type = lib.types.nullOr lib.types.package;
      default = null;
      defaultText = lib.literalExpression "vicinae.packages.\${system}.default, or vicinae.packages.\${system}.with-soulver when enableSoulver is true";
      description = ''
        The vicinae package to use. When null, this will default to the flake's
        `default` package, or `with-soulver` when `enableSoulver` is true.
      '';
    };

    enableSoulver = lib.mkOption {
      type = lib.types.bool;
      default = false;
      description = ''
        Whether to enable the SoulverCore calculator backend.
        SoulverCore is considered unfree software, therefore disabled by default.
        Uses the flake's `with-soulver` package.
        Ignored when `package` is set, wrap your own package if absolutely needed.
      '';
    };

    enableFirefoxIntegration = lib.mkOption {
      default = true;
      description = ''
        Whether to install the messaging host so that the firefox extension <https://addons.mozilla.org/en-US/firefox/addon/vicinae/> works.
      '';
    };

    enableChromeIntegration = lib.mkOption {
      default = true;
      description = ''
        Whether to install the messaging host so that the chrome extension <https://chromewebstore.google.com/detail/vicinae-integration/kcmipingpfbohfjckomimmahknoddnke> works.
      '';
    };

    systemd = {
      enable = lib.mkEnableOption "vicinae systemd integration";

      autoStart = lib.mkOption {
        type = lib.types.bool;
        default = true;
        description = "If the vicinae daemon should be started automatically";
      };

      environment = lib.mkOption {
        type = envVarType;
        default = {};
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

    launchd = {
      enable = lib.mkEnableOption "vicinae launchd integration (macOS)";

      autoStart = lib.mkOption {
        type = lib.types.bool;
        default = true;
        description = "Whether to start the vicinae daemon automatically at login on macOS.";
      };

      environment = lib.mkOption {
        type = envVarType;
        default = {};
        description = "Environment variables to pass to the vicinae daemon on macOS.";
        example = lib.literalExpression ''
          {
            QT_SCALE_FACTOR = 1.5;
            VICINAE_NODE_BIN = "/opt/homebrew/bin/node";
          }
        '';
      };
    };

    extensions = lib.mkOption {
      type = lib.types.listOf lib.types.package;
      default = [];
      description = ''
        List of Vicinae extensions to install.
        You can use the `mkVicinaeExtension` function from the overlay to create extensions.
      '';
    };

    themes = lib.mkOption {
      inherit (tomlFormat) type;
      default = {};
      description = ''
        Theme settings to add to the themes folder in `~/.local/share/vicinae/themes`. See <https://docs.vicinae.com/theming/getting-started> for supported values.

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

    settingOverrides = lib.mkOption {
      type = lib.types.listOf lib.types.path;
      default = [];
      example =
        lib.literalExpression # nix
        
        ''
          [
            ${config.xdg.configHome}/vicinae/override.json
            /run/secrets/vicinae-secrets.json
          ]
        '';
      description = ''
        Allows you to specify additional JSON files that will be merged with the imperative settings and take precedence.
      '';
    };

    settings = lib.mkOption {
      inherit (jsonFormat) type;
      default = {};
      description = ''
        Settings written as JSON to `~/.config/vicinae/nix.json`.
        This is will override any settings from the default settings.json.
        The easiest way to configure this is first configuring your settings in the app,
        then copying the generated `~/.config/vicinae/settings.json` to `~/.config/vicinae/nix.json` and then modifying it as needed.
        If you want to set secrets you should import these files using the settingOverrides option.
      '';
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

  config = let
    settingsFile = jsonFormat.generate "vicinae-settings.json" cfg.settings;

    wrappedVicinae = pkgs.symlinkJoin {
      name = "${vicinaePkg.name}-configured";
      paths = [vicinaePkg];
      nativeBuildInputs = [pkgs.makeWrapper];
      postBuild = let
        allOverrides = (lib.optional (cfg.settings != {}) settingsFile) ++ cfg.settingOverrides;
        overrideString = lib.concatStringsSep ":" allOverrides;
      in ''
        wrapProgram $out/bin/vicinae \
          ${lib.optionalString (allOverrides != []) ''--set VICINAE_OVERRIDES "${overrideString}"''}
      '';
    };
  in
    lib.mkIf cfg.enable {
      assertions = [
        {
          assertion = cfg.enableSoulver -> (cfg.package != null || soulverVicinaePkg != null);
          message = "programs.vicinae.enableSoulver: the soulver backend is not available on ${system}";
        }
      ];

      warnings = lib.optional (cfg.enableSoulver && cfg.package != null) "programs.vicinae.enableSoulver is ignored because programs.vicinae.package is set; wrap your package with soulver-cpp yourself";

      home.packages = [wrappedVicinae];

      programs.vicinae.settings = lib.mkIf cfg.enableSoulver {
        providers.calculator.preferences.backend = lib.mkDefault "soulver-core";
      };

      xdg = let
        themeFiles =
          lib.mapAttrs' (
            name: theme:
              lib.nameValuePair "vicinae/themes/${name}.toml" {
                source = tomlFormat.generate "vicinae-${name}-theme" theme;
              }
          )
          cfg.themes;
      in {
        dataFile =
          builtins.listToAttrs (
            map (item: {
              name = "vicinae/extensions/${item.name}";
              value.source = item;
            })
            cfg.extensions
          )
          // themeFiles;
      };

      programs.firefox.nativeMessagingHosts = lib.mkIf (cfg.enableFirefoxIntegration) [vicinaePkg];

      programs.google-chrome.nativeMessagingHosts = lib.mkIf (cfg.enableChromeIntegration) [vicinaePkg];

      systemd.user.services.vicinae = lib.mkIf (cfg.systemd.enable) {
        Unit = {
          Description = "Vicinae server daemon";
          Documentation = ["https://docs.vicinae.com"];
          After = [cfg.systemd.target];
          PartOf = [cfg.systemd.target];
        };
        Service = {
          Environment =
            lib.mapAttrsToList (key: val: "${key}=${envValueToString val}")
            cfg.systemd.environment;
          Type = "simple";
          ExecStart = "${lib.getExe' wrappedVicinae "vicinae"} server";
          Restart = "always";
          RestartSec = 5;
          KillMode = "process";
        };
        Install = lib.mkIf cfg.systemd.autoStart {
          WantedBy = [cfg.systemd.target];
        };
      };

      launchd.agents.vicinae = lib.mkIf cfg.launchd.enable {
        enable = true;
        config = {
          ProgramArguments = [
            "${lib.getExe' wrappedVicinae "vicinae"}"
            "server"
          ];
          EnvironmentVariables = lib.mapAttrs (_: envValueToString) cfg.launchd.environment;
          RunAtLoad = cfg.launchd.autoStart;
          KeepAlive = {
            Crashed = true;
            SuccessfulExit = false;
          };
          ProcessType = "Interactive";
        };
      };
    };
}

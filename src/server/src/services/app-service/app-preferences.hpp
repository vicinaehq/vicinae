#pragma once
#include <string>
#include <vector>
#include <glaze/core/meta.hpp>
#include <QCoreApplication>
#include "command/preference-schema.hpp"

#ifndef Q_OS_MACOS
enum class AppDefaultAction { Focus, Launch };

template <> struct glz::meta<AppDefaultAction> {
  using enum AppDefaultAction;
  static constexpr auto value = glz::enumerate("focus", Focus, "launch", Launch);
};
#endif

struct AppPreferences {
#if !defined(Q_OS_MACOS) && !defined(Q_OS_WIN)
  AppDefaultAction defaultAction = AppDefaultAction::Focus;
  std::string launchPrefix;
  std::vector<std::string> paths;
#elif defined(Q_OS_WIN)
  AppDefaultAction defaultAction = AppDefaultAction::Focus;
  std::vector<std::string> paths;
#else
  std::vector<std::string> paths;
#endif
};

template <> struct PreferenceSchema<AppPreferences> {
#ifndef Q_OS_MACOS
  PreferenceMeta defaultAction{
      .title = tr("Default action"),
      .description = tr("Action to perform when the return key is pressed. Always default to 'launch' if the "
                        "app has no open window."),
      .options =
          [] {
            return std::vector<Preference::DropdownData::Option>{
                option(AppDefaultAction::Focus, tr("Focus window")),
                option(AppDefaultAction::Launch, tr("Launch app")),
            };
          },
  };
#endif
#if !defined(Q_OS_MACOS) && !defined(Q_OS_WIN)
  PreferenceMeta launchPrefix{
      .title = tr("Launch Prefix"),
      .description = tr("Custom app launcher to use. Affects applications as well as their sub-actions."),
      .placeholder = "uwsm app --",
  };
  PreferenceMeta paths{
      .title = tr("Application directories"),
      .description = tr(
          "Directories applications are sourced from. The list cannot be modified directly. In order to do "
          "so, you need to append additonal paths to the <b>XDG_DATA_DIRS</b> environment variables."),
      .kind = PreferenceMeta::Kind::Directories,
      .readOnly = true,
  };
#elif defined(Q_OS_WIN)
  PreferenceMeta paths{
      .title = tr("Application directories"),
      .description = tr("Directories applications are sourced from."),
      .kind = PreferenceMeta::Kind::Directories,
  };
#else
  PreferenceMeta paths{
      .title = tr("Application directories"),
      .description =
          tr("Directories applications are sourced from. System directories are always scanned and "
             "cannot be removed."),
      .kind = PreferenceMeta::Kind::Directories,
  };
#endif
  Q_DECLARE_TR_FUNCTIONS(AppPreferences)
};

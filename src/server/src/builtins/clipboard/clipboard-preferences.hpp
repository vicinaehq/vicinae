#pragma once
#include <chrono>
#include <optional>
#include <vector>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <QCoreApplication>
#include "builtins/clipboard/history/clipboard-history-model.hpp"
#include "command/preference-schema.hpp"
#include "service-registry.hpp"
#include "services/paste/paste-service.hpp"

enum class ClipboardEviction { Never, FifteenMinutes, OneHour, OneDay, OneWeek, OneMonth, OneYear };

template <> struct glz::meta<ClipboardEviction> {
  using enum ClipboardEviction;
  static constexpr auto value =
      glz::enumerate("never", Never, "900", FifteenMinutes, "3600", OneHour, "86400", OneDay, "604800",
                     OneWeek, "2592000", OneMonth, "31536000", OneYear);
};

template <> struct glz::meta<ClipboardHistorySection::DefaultAction> {
  using enum ClipboardHistorySection::DefaultAction;
  static constexpr auto value = glz::enumerate("copy", Copy, "paste", Paste);
};

inline std::optional<std::chrono::seconds> evictionThreshold(ClipboardEviction eviction) {
  using namespace std::chrono_literals;
  switch (eviction) {
  case ClipboardEviction::Never:
    return std::nullopt;
  case ClipboardEviction::FifteenMinutes:
    return 15min;
  case ClipboardEviction::OneHour:
    return 1h;
  case ClipboardEviction::OneDay:
    return 24h;
  case ClipboardEviction::OneWeek:
    return 24h * 7;
  case ClipboardEviction::OneMonth:
    return 24h * 30;
  case ClipboardEviction::OneYear:
    return 24h * 365;
  }
  return std::nullopt;
}

struct ClipboardPreferences {
  bool monitoring = true;
#ifndef Q_OS_MACOS
  bool ignorePasswords = true;
#endif
  bool preserveTagged = true;
  ClipboardEviction evictionThreshold = ClipboardEviction::Never;
  bool eraseOnStartup = false;
};

template <> struct PreferenceSchema<ClipboardPreferences> {
  PreferenceMeta monitoring{
      .title = tr("Clipboard monitoring"),
      .description = tr("Whether new clipboard selections are appended to the history"),
  };
#ifndef Q_OS_MACOS
  PreferenceMeta ignorePasswords{
      .title = tr("Ignore Passwords"),
      .description =
          tr("Ignore selections that can be identified as a password. May not work with all apps."),
  };
#endif
  PreferenceMeta preserveTagged{
      .title = tr("Preserve tagged"),
      .description = tr("Never evict or mass delete selections that have been explicitly tagged (pinned, "
                        "custom keyword)"),
  };
  PreferenceMeta evictionThreshold{
      .title = tr("Eviction threshold"),
      .description = tr("Automatically delete selections older than this threshold"),
      .options =
          [] {
            return std::vector<Preference::DropdownData::Option>{
                option(ClipboardEviction::Never, tr("Never")),
                option(ClipboardEviction::FifteenMinutes, tr("15 minutes")),
                option(ClipboardEviction::OneHour, tr("1 hour")),
                option(ClipboardEviction::OneDay, tr("1 day")),
                option(ClipboardEviction::OneWeek, tr("1 week")),
                option(ClipboardEviction::OneMonth, tr("1 month")),
                option(ClipboardEviction::OneYear, tr("1 year")),
            };
          },
  };
  PreferenceMeta eraseOnStartup{
      .title = tr("Erase on startup"),
      .description = tr("Erase clipboard history every time the vicinae server is started"),
  };
  Q_DECLARE_TR_FUNCTIONS(ClipboardPreferences)
};

struct ClipboardHistoryPreferences {
  ClipboardHistorySection::DefaultAction defaultAction = ClipboardHistorySection::DefaultAction::Paste;
};

template <> struct PreferenceSchema<ClipboardHistoryPreferences> {
  PreferenceMeta defaultAction{
      .title = tr("Default Action"),
      .description = tr("The default action to perform on pressing return. Paste is only available if your "
                        "environment supports it."),
      .options =
          [] {
            using Action = ClipboardHistorySection::DefaultAction;
            std::vector<Preference::DropdownData::Option> options;
            if (ServiceRegistry::instance()->pasteService()->supportsPaste()) {
              options.emplace_back(option(Action::Paste, tr("Paste")));
            }
            options.emplace_back(option(Action::Copy, tr("Copy")));
            return options;
          },
  };
  Q_DECLARE_TR_FUNCTIONS(ClipboardHistoryPreferences)
};

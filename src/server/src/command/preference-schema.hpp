#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/generic.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <QDebug>
#include <QString>
#include "command/preference.hpp"

struct PreferenceMeta {
  enum class Kind : std::uint8_t { Auto, Text, Password, Shortcut, Files, Directories, App, Apps };
  using Options = std::function<std::vector<Preference::DropdownData::Option>()>;
  using Sections = std::function<std::vector<Preference::DropdownData::Section>()>;

  QString key;
  QString title;
  QString description;
  QString placeholder;
  QString label;
  Kind kind = Kind::Auto;
  bool required = true;
  bool readOnly = false;
  Options options;
  Sections sections;
  std::function<bool()> available;
  std::vector<QString> lockedPaths;
};

/**
 * One `PreferenceMeta` per struct member, same names, same order. `Q_DECLARE_TR_FUNCTIONS` goes last:
 * it ends with `private:`, which would otherwise break aggregate reflection.
 */
template <typename T> struct PreferenceSchema;

template <typename T>
concept TypedPreferences = std::is_aggregate_v<PreferenceSchema<T>>;

struct NoPreferences {};

template <> struct PreferenceSchema<NoPreferences> {};

template <typename E>
  requires std::is_enum_v<E>
QString enumName(E value) {
  const std::string_view name = glz::get_enum_name(value);
  return QString::fromUtf8(name.data(), static_cast<qsizetype>(name.size()));
}

template <typename E>
  requires std::is_enum_v<E>
Preference::DropdownData::Option option(E value, QString title) {
  return {.title = std::move(title), .value = enumName(value)};
}

namespace preference_schema {

template <typename T, std::size_t I> QString keyOf(const PreferenceMeta &meta) {
  if (!meta.key.isEmpty()) return meta.key;
  constexpr std::string_view name = glz::reflect<T>::keys[I];
  return QString::fromUtf8(name.data(), static_cast<qsizetype>(name.size()));
}

template <typename M> Preference make(const QString &key, const M &value, const PreferenceMeta &meta) {
  using Kind = PreferenceMeta::Kind;
  Preference pref;

  if constexpr (std::is_same_v<M, bool>) {
    pref = Preference::makeCheckbox(key, meta.label.isEmpty() ? std::nullopt : std::optional{meta.label});
    pref.setDefaultValue(value);
  } else if constexpr (std::is_enum_v<M>) {
    static_assert(glz::glaze_enum_t<M>, "enum preferences need a glz::meta enumerating their names");
    pref = meta.sections
               ? Preference::makeDropdown(key, meta.sections())
               : Preference::makeDropdown(
                     key, meta.options ? meta.options() : std::vector<Preference::DropdownData::Option>{});
    pref.setDefaultValue(enumName(value));
  } else if constexpr (std::is_same_v<M, std::string> || std::is_same_v<M, std::optional<std::string>>) {
    switch (meta.kind) {
    case Kind::Password:
      pref = Preference::makePassword(key);
      break;
    case Kind::Shortcut:
      pref = Preference::makeShortcut(key);
      break;
    case Kind::App:
      pref = Preference::app(key);
      break;
    default:
      if (meta.sections) {
        pref = Preference::makeDropdown(key, meta.sections());
      } else if (meta.options) {
        pref = Preference::makeDropdown(key, meta.options());
      } else {
        pref = Preference::makeText(key);
      }
      break;
    }
    if constexpr (std::is_same_v<M, std::string>) {
      pref.setDefaultValue(glz::generic(value));
    } else {
      pref.setDefaultValue(value ? glz::generic(*value) : glz::generic{});
    }
  } else if constexpr (std::is_arithmetic_v<M>) {
    pref = Preference::makeText(key);
    pref.setDefaultValue(glz::generic(static_cast<double>(value)));
  } else if constexpr (std::is_same_v<M, std::vector<std::string>>) {
    switch (meta.kind) {
    case Kind::Directories:
      pref = Preference::directories(key, meta.lockedPaths);
      break;
    case Kind::Apps:
      pref = Preference::apps(key);
      break;
    default:
      pref = Preference::files(key);
      break;
    }
    glz::generic::array_t defaults;
    defaults.reserve(value.size());
    for (const auto &entry : value) {
      defaults.emplace_back(entry);
    }
    pref.setDefaultValue(std::move(defaults));
  } else {
    static_assert(!std::is_same_v<M, M>, "unsupported preference member type");
  }

  if (!meta.title.isEmpty()) pref.setTitle(meta.title);
  if (!meta.description.isEmpty()) pref.setDescription(meta.description);
  if (!meta.placeholder.isEmpty()) pref.setPlaceholder(meta.placeholder);
  pref.setRequired(meta.required);
  if (meta.readOnly) pref.setReadOnly();

  return pref;
}

template <TypedPreferences T> constexpr void checkMeta() {
  using Meta = PreferenceSchema<T>;
  static_assert(glz::reflect<T>::size == glz::reflect<Meta>::size,
                "preference meta must declare one entry per preference member");
  if constexpr (glz::reflect<T>::size > 0) {
    static_assert(glz::reflect<T>::keys == glz::reflect<Meta>::keys,
                  "preference meta members must mirror the preference members, in the same order");
  }
}

} // namespace preference_schema

template <TypedPreferences T> std::vector<Preference> describePreferences(T defaults = {}) {
  preference_schema::checkMeta<T>();
  constexpr auto N = glz::reflect<T>::size;

  if constexpr (N == 0) {
    return {};
  } else {
    PreferenceSchema<T> metas{};
    auto values = glz::to_tie(defaults);
    auto entries = glz::to_tie(metas);
    std::vector<Preference> preferences;
    preferences.reserve(N);

    [&]<std::size_t... I>(std::index_sequence<I...>) {
      (
          [&] {
            const auto &meta = glz::get<I>(entries);
            if (meta.available && !meta.available()) return;
            preferences.emplace_back(
                preference_schema::make(preference_schema::keyOf<T, I>(meta), glz::get<I>(values), meta));
          }(),
          ...);
    }(std::make_index_sequence<N>{});

    return preferences;
  }
}

template <TypedPreferences T> T readPreferences(const PreferenceValues &values, T defaults = {}) {
  preference_schema::checkMeta<T>();
  constexpr auto N = glz::reflect<T>::size;

  if constexpr (N > 0) {
    PreferenceSchema<T> metas{};
    auto entries = glz::to_tie(metas);
    auto members = glz::to_tie(defaults);

    [&]<std::size_t... I>(std::index_sequence<I...>) {
      (
          [&] {
            auto &member = glz::get<I>(members);
            using M = std::remove_cvref_t<decltype(member)>;
            const auto key = preference_schema::keyOf<T, I>(glz::get<I>(entries));
            const auto *value = preferences::find(values, key.toStdString());
            if (!value || value->is_null()) return;

            if constexpr (std::is_arithmetic_v<M>) {
              if (value->is_string()) {
                bool ok = false;
                const double number = QString::fromStdString(value->get_string()).toDouble(&ok);
                if (ok) member = static_cast<M>(number);
                return;
              }
            }

            std::string json;
            if (glz::write_json(*value, json)) return;
            const M fallback = member;
            if (auto const error = glz::read<glz::opts{.error_on_unknown_keys = false}>(member, json)) {
              qWarning() << "Ignoring preference" << key << ":" << glz::format_error(error, json);
              member = fallback;
            }
          }(),
          ...);
    }(std::make_index_sequence<N>{});
  }

  return defaults;
}

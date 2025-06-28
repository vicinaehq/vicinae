#pragma once
#include "command-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include <optional>
#include "single-view-command-context.hpp"
#include <qcontainerfwd.h>

class CommandBuilder {
  QString _id;
  QString _name;
  QString m_description;
  std::optional<OmniIconUrl> _url;
  std::vector<Preference> _preferences;
  bool m_fallback = false;

public:
  CommandBuilder(const QString &id) : _id(id) {}

  CommandBuilder &withName(const QString &name) {
    _name = name;
    return *this;
  }
  CommandBuilder &withIcon(const OmniIconUrl &url) {
    _url = url;
    return *this;
  }

  CommandBuilder &asFallback() {
    m_fallback = true;
    return *this;
  }

  CommandBuilder &withTintedIcon(const QString &name, ColorTint tint) {
    _url = BuiltinOmniIconUrl(name).setBackgroundTint(tint);
    return *this;
  }

  CommandBuilder &withPreference(const Preference &preference) {
    _preferences.push_back(preference);
    return *this;
  }

  CommandBuilder &withDescription(const QString &description) {
    m_description = description;
    return *this;
  }

  template <typename T> std::shared_ptr<BuiltinCommand> toSingleView() {
    auto cmd = std::make_shared<BuiltinViewCommand<T>>(_id, _name, _url, _preferences);

    cmd->setIsFallback(m_fallback);
    cmd->setDescription(m_description);
    return cmd;
  }

  template <typename T> std::shared_ptr<BuiltinCommand> toNoViewContext() {
    auto cmd = std::make_shared<BuiltinNoViewCommandContext<T>>(_id, _name, _url, _preferences);

    cmd->setIsFallback(m_fallback);
    cmd->setDescription(m_description);
    return cmd;
  }

  template <typename T> std::shared_ptr<BuiltinCommand> toContext() {
    auto cmd = std::make_shared<BuiltinCommandContext<T>>(_id, _name, _url, _preferences);

    cmd->setIsFallback(m_fallback);
    cmd->setDescription(m_description);

    return cmd;
  }
};

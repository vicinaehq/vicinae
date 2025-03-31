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
  std::optional<OmniIconUrl> _url;

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

  CommandBuilder &withTintedIcon(const QString &name, ColorTint tint) {
    _url = BuiltinOmniIconUrl(name).setBackgroundTint(tint);
    return *this;
  }

  CommandBuilder &withPreference(const Preference &preference) {}

  template <typename T> std::shared_ptr<BuiltinCommand> toView() {
    return std::make_shared<BuiltinViewCommand<T>>(_id, _name, _url);
  }
};

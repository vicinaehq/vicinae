#pragma once
#include "ui/image/url.hpp"
#include <QString>

namespace qml {

inline QString imageSourceFor(const ImageURL &url) {
  switch (url.type()) {
  case ImageURLType::Builtin: {
    QString base = QString("image://vicinae/builtin:%1").arg(url.name());
    if (auto bg = url.backgroundTint())
      base += QString("?bg=%1").arg(static_cast<int>(*bg));
    if (auto fg = url.foregroundTint())
      base += QString(base.contains('?') ? "&fg=%1" : "?fg=%1").arg(static_cast<int>(*fg));
    return base;
  }
  case ImageURLType::System:
    return QString("image://vicinae/system:%1").arg(url.name());
  case ImageURLType::Local:
    return QString("image://vicinae/local:%1").arg(url.name());
  case ImageURLType::Emoji:
    return QString("image://vicinae/emoji:%1").arg(url.name());
  case ImageURLType::Http:
  case ImageURLType::Https:
    return QStringLiteral("image://vicinae/builtin:globe-01");
  case ImageURLType::Favicon:
    return QStringLiteral("image://vicinae/builtin:globe-01");
  default:
    return QStringLiteral("image://vicinae/builtin:question-mark-circle");
  }
}

} // namespace qml

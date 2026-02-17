#pragma once
#include "extend/metadata-model.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <QVariantList>

namespace qml {

QVariantList metadataToVariantList(const MetadataModel &metadata);

inline QString imageSourceFor(const ImageURL &url) {
  const auto &theme = ThemeService::instance().theme();

  switch (url.type()) {
  case ImageURLType::Builtin: {
    // Use the ImageURL's fill color if set, otherwise default to Foreground.
    // Extension icons may specify a tintColor (e.g. Blue, Red) via fillColor().
    QColor fg;
    if (auto fill = url.fillColor()) {
      if (auto *sc = std::get_if<SemanticColor>(&*fill)) {
        fg = theme.resolve(*sc);
      } else if (auto *qc = std::get_if<QColor>(&*fill)) {
        fg = *qc;
      } else if (auto *str = std::get_if<QString>(&*fill)) {
        fg = QColor(*str);
      } else if (auto *dc = std::get_if<DynamicColor>(&*fill)) {
        fg = theme.isLight() ? QColor(dc->light) : QColor(dc->dark);
      }
    }
    if (!fg.isValid()) { fg = theme.resolve(SemanticColor::Foreground); }
    QString base = QString("image://vicinae/builtin:%1?fg=%2")
                       .arg(url.name(), fg.name(QColor::HexRgb));
    if (auto bg = url.backgroundTint()) {
      QColor bgColor = theme.resolve(*bg);
      base += QString("&bg=%1").arg(bgColor.name(QColor::HexRgb));
    }
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
    return QString("image://vicinae/http:%1").arg(url.name());
  case ImageURLType::Favicon:
    return QString("image://vicinae/favicon:%1").arg(url.name());
  case ImageURLType::DataURI:
    return QString("image://vicinae/datauri:%1").arg(url.name());
  default:
    return QStringLiteral("image://vicinae/builtin:question-mark-circle");
  }
}

} // namespace qml

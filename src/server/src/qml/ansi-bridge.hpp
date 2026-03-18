#pragma once

#include "ansi/ansi.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <QObject>

inline ansi::Palette buildAnsiPalette() {
  auto &theme = ThemeService::instance().theme();
  auto hex = [&](SemanticColor c) { return theme.resolve(c).name().toStdString(); };

  ansi::Palette pal;
  pal.standard[0] = "#000000";
  pal.standard[1] = hex(SemanticColor::Red);
  pal.standard[2] = hex(SemanticColor::Green);
  pal.standard[3] = hex(SemanticColor::Yellow);
  pal.standard[4] = hex(SemanticColor::Blue);
  pal.standard[5] = hex(SemanticColor::Magenta);
  pal.standard[6] = hex(SemanticColor::Cyan);
  pal.standard[7] = hex(SemanticColor::Foreground);

  pal.bright[0] = hex(SemanticColor::TextMuted);
  pal.bright[1] = hex(SemanticColor::Red);
  pal.bright[2] = hex(SemanticColor::Green);
  pal.bright[3] = hex(SemanticColor::Yellow);
  pal.bright[4] = hex(SemanticColor::Blue);
  pal.bright[5] = hex(SemanticColor::Magenta);
  pal.bright[6] = hex(SemanticColor::Cyan);
  pal.bright[7] = "#ffffff";
  return pal;
}

class AnsiBridge : public QObject {
  Q_OBJECT

public:
  explicit AnsiBridge(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE QString toHtml(const QString &text) const {
    return QString::fromStdString(ansi::to_html(text.toStdString(), buildAnsiPalette()));
  }

  Q_INVOKABLE QString strip(const QString &text) const {
    return QString::fromStdString(ansi::strip(text.toStdString()));
  }
};

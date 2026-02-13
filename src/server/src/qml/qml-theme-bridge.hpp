#pragma once
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <QColor>
#include <QObject>

class QmlThemeBridge : public QObject {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background NOTIFY changed)
  Q_PROPERTY(QColor foreground READ foreground NOTIFY changed)
  Q_PROPERTY(QColor textMuted READ textMuted NOTIFY changed)
  Q_PROPERTY(QColor textPlaceholder READ textPlaceholder NOTIFY changed)
  Q_PROPERTY(QColor listItemSelectionBg READ listItemSelectionBg NOTIFY changed)
  Q_PROPERTY(QColor listItemHoverBg READ listItemHoverBg NOTIFY changed)
  Q_PROPERTY(QColor accent READ accent NOTIFY changed)
  Q_PROPERTY(QColor statusBarBackground READ statusBarBackground NOTIFY changed)
  Q_PROPERTY(QColor mainWindowBorder READ mainWindowBorder NOTIFY changed)
  Q_PROPERTY(QColor inputBorder READ inputBorder NOTIFY changed)
  Q_PROPERTY(QColor inputBorderFocus READ inputBorderFocus NOTIFY changed)
  Q_PROPERTY(QColor divider READ divider NOTIFY changed)
  Q_PROPERTY(QColor secondaryBackground READ secondaryBackground NOTIFY changed)
  Q_PROPERTY(QColor listItemSelectionFg READ listItemSelectionFg NOTIFY changed)
  Q_PROPERTY(QColor scrollBarBackground READ scrollBarBackground NOTIFY changed)
  Q_PROPERTY(qreal regularFontSize READ regularFontSize NOTIFY changed)
  Q_PROPERTY(qreal smallerFontSize READ smallerFontSize NOTIFY changed)

signals:
  void changed();

public:
  explicit QmlThemeBridge(QObject *parent = nullptr) : QObject(parent) {
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, &QmlThemeBridge::changed);
  }

  QColor background() const { return resolve(SemanticColor::Background); }
  QColor foreground() const { return resolve(SemanticColor::Foreground); }
  QColor textMuted() const { return resolve(SemanticColor::TextMuted); }
  QColor textPlaceholder() const { return resolve(SemanticColor::TextPlaceholder); }
  QColor listItemSelectionBg() const { return resolve(SemanticColor::ListItemSelectionBackground); }
  QColor listItemSelectionFg() const { return resolve(SemanticColor::ListItemSelectionForeground); }
  QColor listItemHoverBg() const { return resolve(SemanticColor::ListItemHoverBackground); }
  QColor accent() const { return resolve(SemanticColor::Accent); }
  QColor statusBarBackground() const { return resolve(SemanticColor::StatusBarBackground); }
  QColor mainWindowBorder() const { return resolve(SemanticColor::MainWindowBorder); }
  QColor inputBorder() const { return resolve(SemanticColor::InputBorder); }
  QColor inputBorderFocus() const { return resolve(SemanticColor::InputBorderFocus); }
  QColor divider() const { return resolve(SemanticColor::BackgroundBorder); }
  QColor secondaryBackground() const { return resolve(SemanticColor::SecondaryBackground); }
  QColor scrollBarBackground() const { return resolve(SemanticColor::ScrollBarBackground); }

  qreal regularFontSize() const { return ThemeService::instance().pointSize(TextRegular); }
  qreal smallerFontSize() const { return ThemeService::instance().pointSize(TextSmaller); }

private:
  QColor resolve(SemanticColor color) const { return ThemeService::instance().theme().resolve(color); }
};

#pragma once
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <QColor>
#include <QGuiApplication>
#include <QObject>

class ThemeBridge : public QObject {
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
  Q_PROPERTY(QColor inputBorderError READ inputBorderError NOTIFY changed)
  Q_PROPERTY(QColor divider READ divider NOTIFY changed)
  Q_PROPERTY(QColor secondaryBackground READ secondaryBackground NOTIFY changed)
  Q_PROPERTY(QColor listItemSelectionFg READ listItemSelectionFg NOTIFY changed)
  Q_PROPERTY(QColor scrollBarBackground READ scrollBarBackground NOTIFY changed)
  Q_PROPERTY(QColor gridItemBackground READ gridItemBackground NOTIFY changed)
  Q_PROPERTY(QColor gridItemSelectionOutline READ gridItemSelectionOutline NOTIFY changed)
  Q_PROPERTY(QColor gridItemHoverOutline READ gridItemHoverOutline NOTIFY changed)
  Q_PROPERTY(QColor danger READ danger NOTIFY changed)
  Q_PROPERTY(QColor toastSuccess READ toastSuccess NOTIFY changed)
  Q_PROPERTY(QColor toastInfo READ toastInfo NOTIFY changed)
  Q_PROPERTY(QColor toastWarning READ toastWarning NOTIFY changed)
  Q_PROPERTY(QColor toastDanger READ toastDanger NOTIFY changed)
  Q_PROPERTY(QColor textSelectionBg READ textSelectionBg NOTIFY changed)
  Q_PROPERTY(QColor textSelectionFg READ textSelectionFg NOTIFY changed)
  Q_PROPERTY(QColor linkColor READ linkColor NOTIFY changed)
  Q_PROPERTY(QColor loadingBar READ loadingBar NOTIFY changed)
  Q_PROPERTY(QColor buttonPrimaryBg READ buttonPrimaryBg NOTIFY changed)
  Q_PROPERTY(QColor buttonPrimaryHoverBg READ buttonPrimaryHoverBg NOTIFY changed)
  Q_PROPERTY(qreal regularFontSize READ regularFontSize NOTIFY changed)
  Q_PROPERTY(qreal smallerFontSize READ smallerFontSize NOTIFY changed)
  Q_PROPERTY(QString fontFamily READ fontFamily NOTIFY changed)

signals:
  void changed();

public:
  explicit ThemeBridge(QObject *parent = nullptr) : QObject(parent) {
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, &ThemeBridge::changed);
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
  QColor inputBorderError() const { return resolve(SemanticColor::InputBorderError); }
  QColor divider() const { return resolve(SemanticColor::BackgroundBorder); }
  QColor secondaryBackground() const { return resolve(SemanticColor::SecondaryBackground); }
  QColor scrollBarBackground() const { return resolve(SemanticColor::ScrollBarBackground); }
  QColor gridItemBackground() const { return resolve(SemanticColor::GridItemBackground); }
  QColor gridItemSelectionOutline() const { return resolve(SemanticColor::GridItemSelectionOutline); }
  QColor gridItemHoverOutline() const { return resolve(SemanticColor::GridItemHoverOutline); }
  QColor danger() const { return resolve(SemanticColor::TextDanger); }
  QColor toastSuccess() const { return resolve(SemanticColor::Green); }
  QColor toastInfo() const { return resolve(SemanticColor::Blue); }
  QColor toastWarning() const { return resolve(SemanticColor::Orange); }
  QColor toastDanger() const { return resolve(SemanticColor::Red); }

  QColor linkColor() const { return resolve(SemanticColor::LinkDefault); }
  QColor textSelectionBg() const { return resolve(SemanticColor::TextSelectionBackground); }
  QColor textSelectionFg() const { return resolve(SemanticColor::TextSelectionForeground); }
  QColor loadingBar() const { return resolve(SemanticColor::LoadingBar); }
  QColor buttonPrimaryBg() const { return resolve(SemanticColor::ButtonPrimaryBackground); }
  QColor buttonPrimaryHoverBg() const { return resolve(SemanticColor::ButtonPrimaryHoverBackground); }

  qreal regularFontSize() const { return ThemeService::instance().pointSize(TextRegular); }
  qreal smallerFontSize() const { return ThemeService::instance().pointSize(TextSmaller); }
  QString fontFamily() const { return QGuiApplication::font().family(); }

private:
  QColor resolve(SemanticColor color) const { return ThemeService::instance().theme().resolve(color); }
};

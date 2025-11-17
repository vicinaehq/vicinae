#pragma once
#include "services/config/config-service.hpp"
#include "ui/favicon-service-selector/favicon-service-selector.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/qtheme-selector/qtheme-selector.hpp"
#include "ui/theme-selector/theme-selector.hpp"
#include "ui/keybinding-selector/keybinding-selector.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include <qnamespace.h>
#include <qwidget.h>
#include <QFont>

// We will implement something better than this, eventually
class GeneralSettings : public VerticalScrollArea {
  CheckboxInput *m_rootFileSearch;
  CheckboxInput *m_csd;
  CheckboxInput *m_closeOnFocusLoss;
  CheckboxInput *m_considerPreedit;
  BaseInput *m_opacity;
  ThemeSelector *m_themeSelector;
  FontSelector *m_fontSelector;
  QThemeSelector *m_qThemeSelector;
  FaviconServiceSelector *m_faviconSelector;
  KeyBindingSelector *m_keybindingSelector;
  CheckboxInput *m_popToRootOnClose;
  BaseInput *m_fontSize;

  void setupUI();

  void handleCloseOnFocusLossChange(bool value);
  void handleConsiderPreeditChange(bool value);
  void handleRootSearchFilesChange(bool value);
  void handleThemeChange(const QString &id);
  void handleClientSideDecorationChange(bool value);
  void handleFontChange(const QString &fontFamily);
  void handleOpacityChange(double opacity);
  void handleIconThemeChange(const QString &iconTheme);
  void handleFaviconServiceChange(const QString &service);
  void handleKeybindingChange(const QString &keybinding);
  void handlePopToRootOnCloseChange(bool popToRootOnClose);
  void handleFontSizeChange(double size);

  void setConfig(const ConfigService::Value &value);

public:
  GeneralSettings();
};

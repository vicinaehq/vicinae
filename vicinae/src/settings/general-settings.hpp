#include "services/config/config-service.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/theme-selector/theme-selector.hpp"
#include <qnamespace.h>
#include <qwidget.h>
#include <QFont>

class GeneralSettings : public QWidget {
  CheckboxInput *m_rootFileSearch;
  CheckboxInput *m_csd;
  BaseInput *m_opacity;
  ThemeSelector *m_themeSelector;
  FontSelector *m_fontSelector;

  void setupUI();

  void handleRootSearchFilesChange(bool value);
  void handleThemeChange(const QString &id);
  void handleClientSideDecorationChange(bool value);
  void handleFontChange(const QString &fontFamily);
  void handleOpacityChange(double opacity);

  void setConfig(const ConfigService::Value &value);

public:
  GeneralSettings();
};

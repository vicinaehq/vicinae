#include "../ui/image/url.hpp"
#include "settings/extension-settings.hpp"
#include "general-settings.hpp"
#include "settings-about.hpp"
#include <qwidget.h>

class SettingsCategory {
public:
  /**
   * The category content.
   * The content of each category is created when creating the settings window, not on the fly.
   */
  virtual QWidget *createContent() = 0;

  /**
   * Category's title as shown on the top bar
   */
  virtual QString title() const = 0;

  /**
   * Category's icon as shown on the top bar
   */
  virtual ImageURL icon() const = 0;

  virtual ~SettingsCategory() = default;
};

class ExtensionSettingsCategory : public SettingsCategory {
public:
  QString title() const override { return "Extensions"; }
  ImageURL icon() const override { return ImageURL::builtin("computer-chip"); }
  QWidget *createContent() override { return new ExtensionSettingsContent(); }
};

class GeneralSettingsCategory : public SettingsCategory {
  QString title() const override { return "General"; }
  ImageURL icon() const override { return ImageURL::builtin("cog"); }
  QWidget *createContent() override { return new GeneralSettings(); }
};

class AdvancedSettingsCategory : public SettingsCategory {
  QString title() const override { return "Advanced"; }
  ImageURL icon() const override { return ImageURL::builtin("wrench-screwdriver"); }
  QWidget *createContent() override { return new QWidget; }
};

class AboutSettingsCategory : public SettingsCategory {
  QString title() const override { return "About"; }
  ImageURL icon() const override { return ImageURL::builtin("vicinae"); }
  QWidget *createContent() override { return new SettingsAbout; }
};

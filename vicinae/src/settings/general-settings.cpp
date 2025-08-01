#include "general-settings.hpp"
#include "service-registry.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/theme-selector/theme-selector.hpp"
#include "utils/layout.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "services/config/config-service.hpp"

void GeneralSettings::setConfig(const ConfigService::Value &value) {
  auto appFont = QApplication::font().family();

  m_opacity->setText(QString::number(value.window.opacity));
  m_csd->setValueAsJson(value.window.csd);
  m_themeSelector->setValue(value.theme.name.value_or("vicinae-dark"));
  m_fontSelector->setValue(value.font.normal.value_or(appFont));
  m_rootFileSearch->setValueAsJson(value.rootSearch.searchFiles);
}

void GeneralSettings::handleThemeChange(const QString &id) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.theme.name = id; });
}

void GeneralSettings::handleClientSideDecorationChange(bool csd) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.window.csd = csd; });
}

void GeneralSettings::handleFontChange(const QString &font) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.font.normal = font; });
}

void GeneralSettings::handleRootSearchFilesChange(bool enabled) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.rootSearch.searchFiles = enabled; });
}

void GeneralSettings::handleOpacityChange(double opacity) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.window.opacity = opacity; });
}

void GeneralSettings::setupUI() {
  auto config = ServiceRegistry::instance()->config();
  auto appFont = QApplication::font().family();
  auto value = config->value();

  m_rootFileSearch = new CheckboxInput;
  m_csd = new CheckboxInput;
  m_opacity = new BaseInput;
  m_themeSelector = new ThemeSelector;
  m_fontSelector = new FontSelector;

  FormWidget *form = new FormWidget;

  auto fontField = new FormField(m_fontSelector, "Font");

  connect(m_fontSelector, &FontSelector::selectionChanged, this,
          [this](auto &&item) { handleFontChange(item.id()); });

  auto themeField = new FormField(m_themeSelector, "Theme");

  auto opacityField = new FormField;

  opacityField->setName("Window opacity");
  opacityField->setWidget(m_opacity, m_opacity->focusNotifier());

  m_opacity->setText(QString::number(value.window.opacity));

  connect(m_themeSelector, &ThemeSelector::selectionChanged, this,
          [this](auto &&item) { handleThemeChange(item.id()); });

  connect(m_themeSelector, &ThemeSelector::selectionChanged, this,
          [this](auto &&item) { handleThemeChange(item.id()); });

  connect(opacityField, &FormField::blurred, this,
          [this]() { handleOpacityChange(m_opacity->text().toDouble()); });

  m_rootFileSearch->setValueAsJson(value.rootSearch.searchFiles);

  m_csd->setLabel("Use client-side decorations");
  m_csd->setValueAsJson(value.window.csd);

  connect(m_csd, &CheckboxInput::valueChanged, this, &GeneralSettings::handleClientSideDecorationChange);

  auto csdField = new FormField(m_csd, "CSD");

  csdField->setInfo(
      R"(Let Vicinae draw its own rounded borders instead of relying on the windowing system to do so. You can usually get more refined results by properly configuring your window manager.)");

  m_themeSelector->setValue(value.theme.name.value_or("vicinae-dark"));
  m_fontSelector->setValue(value.font.normal.value_or(appFont));

  m_rootFileSearch->setLabel("Show files in root search");
  auto checkField = new FormField;

  connect(m_rootFileSearch, &CheckboxInput::valueChanged, this,
          &GeneralSettings::handleRootSearchFilesChange);

  checkField->setWidget(m_rootFileSearch);
  checkField->setName("Root file search");
  checkField->setInfo("Files are searched asynchronously, so if this is enabled you should expect a slight "
                      "delay for file search results to show up");

  form->addField(checkField);
  form->addField(themeField);
  form->addField(fontField);
  form->addField(csdField);
  form->addField(opacityField);
  form->setMaximumWidth(600);

  VStack().margins(0, 20, 0, 20).add(HStack().add(form, Qt::AlignCenter)).imbue(this);
}

GeneralSettings::GeneralSettings() {
  auto config = ServiceRegistry::instance()->config();

  setupUI();
  setConfig(config->value());
  connect(config, &ConfigService::configChanged, [this](auto next, auto prev) { setConfig(next); });
}

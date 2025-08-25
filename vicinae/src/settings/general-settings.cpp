#include "general-settings.hpp"
#include "service-registry.hpp"
#include "ui/favicon-service-selector/favicon-service-selector.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/qtheme-selector/qtheme-selector.hpp"
#include "ui/theme-selector/theme-selector.hpp"
#include "utils/layout.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "services/config/config-service.hpp"
#include <algorithm>
#include <qlogging.h>

void GeneralSettings::setConfig(const ConfigService::Value &value) {
  auto appFont = QApplication::font().family();
  auto currentIconTheme = QIcon::themeName();

  m_opacity->setText(QString::number(value.window.opacity));
  m_csd->setValueAsJson(value.window.csd);
  m_themeSelector->setValue(value.theme.name.value_or("vicinae-dark"));
  m_fontSelector->setValue(value.font.normal.value_or(appFont));
  m_rootFileSearch->setValueAsJson(value.rootSearch.searchFiles);
  m_qThemeSelector->setValue(value.theme.iconTheme.value_or(currentIconTheme));
  m_faviconSelector->setValue(value.faviconService);
  m_popToRootOnClose->setValueAsJson(value.popToRootOnClose);
  m_fontSize->setText(QString::number(value.font.baseSize));
}

void GeneralSettings::handleFaviconServiceChange(const QString &service) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.faviconService = service; });
}

void GeneralSettings::handleIconThemeChange(const QString &iconTheme) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.theme.iconTheme = iconTheme; });
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

void GeneralSettings::handlePopToRootOnCloseChange(bool popToRootOnClose) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig([&](ConfigService::Value &value) { value.popToRootOnClose = popToRootOnClose; });
}

void GeneralSettings::handleFontSizeChange(double size) {
  auto config = ServiceRegistry::instance()->config();

  config->updateConfig(
      [&](ConfigService::Value &value) { value.font.baseSize = std::clamp(size, 1.0, 99.0); });
}

void GeneralSettings::setupUI() {
  auto config = ServiceRegistry::instance()->config();
  auto appFont = QApplication::font().family();
  auto value = config->value();

  m_rootFileSearch = new CheckboxInput;
  m_csd = new CheckboxInput;
  m_opacity = new BaseInput;
  m_themeSelector = new ThemeSelector;
  m_qThemeSelector = new QThemeSelector;
  m_fontSelector = new FontSelector;
  m_faviconSelector = new FaviconServiceSelector;
  m_popToRootOnClose = new CheckboxInput;
  m_fontSize = new BaseInput;

  m_popToRootOnClose->setLabel("Pop to root on window close");

  FormWidget *form = new FormWidget;

  auto checkField = form->addField("Root file search", m_rootFileSearch);

  checkField->setInfo("Files are searched asynchronously, so if this is enabled you should expect a slight "
                      "delay for file search results to show up");

  auto popToRootOnCloseField = form->addField("Pop on close", m_popToRootOnClose);

  popToRootOnCloseField->setInfo("Whether to reset the navigation state when the launcher window is closed.");

  auto fontField = form->addField("Font", m_fontSelector);

  connect(m_fontSelector, &FontSelector::selectionChanged, this,
          [this](auto &&item) { handleFontChange(item.id()); });

  auto themeField = form->addField("Theme", m_themeSelector);
  auto opacityField = form->addField("Window opacity", m_opacity);

  m_opacity->setText(QString::number(value.window.opacity));

  connect(m_themeSelector, &ThemeSelector::selectionChanged, this,
          [this](auto &&item) { handleThemeChange(item.id()); });

  connect(m_themeSelector, &ThemeSelector::selectionChanged, this,
          [this](auto &&item) { handleThemeChange(item.id()); });

  connect(m_qThemeSelector, &QThemeSelector::selectionChanged, this,
          [this](auto &&item) { handleIconThemeChange(item.id()); });

  connect(m_faviconSelector, &FaviconServiceSelector::selectionChanged, this,
          [this](auto &&item) { handleFaviconServiceChange(item.id()); });

  connect(opacityField, &FormField::blurred, this,
          [this]() { handleOpacityChange(m_opacity->text().toDouble()); });

  m_rootFileSearch->setValueAsJson(value.rootSearch.searchFiles);

  m_csd->setLabel("Use client-side decorations");
  m_csd->setValueAsJson(value.window.csd);

  connect(m_csd, &CheckboxInput::valueChanged, this, &GeneralSettings::handleClientSideDecorationChange);

  auto csdField = form->addField("CSD", m_csd);

  csdField->setInfo(
      R"(Let Vicinae draw its own rounded borders instead of relying on the windowing system to do so. You can usually get more refined results by properly configuring your window manager.)");

  m_themeSelector->setValue(value.theme.name.value_or("vicinae-dark"));
  m_fontSelector->setValue(value.font.normal.value_or(appFont));

  m_rootFileSearch->setLabel("Show files in root search");

  auto qThemeField = form->addField("Icon Theme", m_qThemeSelector);

  qThemeField->setInfo("The icon theme used for system icons (applications, mime types, folder icons...). "
                       "This does not affect builtin Vicinae icons.");

  auto faviconField = form->addField("Favicon Fetching", m_faviconSelector);

  faviconField->setInfo("The favicon provider used to load favicons where needed. You can turn off favicon "
                        "loading by selecting 'None'.");

  connect(m_rootFileSearch, &CheckboxInput::valueChanged, this,
          &GeneralSettings::handleRootSearchFilesChange);

  connect(m_popToRootOnClose, &CheckboxInput::valueChanged, this,
          [this](bool value) { handlePopToRootOnCloseChange(value); });

  auto fontSizeField = form->addField("Font size ", m_fontSize);

  connect(fontSizeField, &FormField::blurred, this, [this]() {
    bool ok = false;

    if (double size = m_fontSize->text().toDouble(&ok); ok) {
      handleFontSizeChange(m_fontSize->text().toDouble());
    } else {
      qWarning() << m_fontSize->text() << "is not a valid font size";
    }
  });

  fontSizeField->setInfo(
      "The base point size used to compute font sizes. Fractional values are accepted and should render as "
      "expected on most platforms. The recommended range is [10.0;12.0].");

  form->setMaximumWidth(650);

  setWidget(VStack().margins(0, 20, 0, 20).add(HStack().add(form, Qt::AlignCenter)).buildWidget());
}

GeneralSettings::GeneralSettings() {
  auto config = ServiceRegistry::instance()->config();

  setupUI();
  setConfig(config->value());
  connect(config, &ConfigService::configChanged, [this](auto next, auto prev) { setConfig(next); });
}

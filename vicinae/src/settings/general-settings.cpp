#include "general-settings.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "ui/favicon-service-selector/favicon-service-selector.hpp"
#include "ui/font-selector/font-selector.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/checkbox-input.hpp"
#include "ui/shortcut-recorder-input/shortcut-recorder-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "ui/qtheme-selector/qtheme-selector.hpp"
#include "ui/theme-selector/theme-selector.hpp"
#include "ui/keybinding-selector/keybinding-selector.hpp"
#include "utils/layout.hpp"
#include "ui/font-selector/font-selector.hpp"
#include <qlogging.h>

void GeneralSettings::setConfig(const config::ConfigValue &value) {
  auto appFont = QApplication::font().family();
  auto currentIconTheme = QIcon::themeName();
  auto &normalFont = value.font.normal;

  m_opacity->setText(QString::number(value.launcherWindow.opacity));
  m_csd->stealthySetValueAsJson(value.launcherWindow.clientSideDecorations.enabled);
  m_themeSelector->setValue(value.systemTheme().name.c_str());
  m_fontSelector->setValue(normalFont.family == "auto" ? appFont : normalFont.family.c_str());
  m_rootFileSearch->stealthySetValueAsJson(value.searchFilesInRoot);

  if (auto ith = value.systemTheme().iconTheme; ith != "auto") { m_qThemeSelector->setValue(ith.c_str()); }

  m_faviconSelector->setValue(value.faviconService.c_str());
  m_keybindingSelector->setValue(value.keybinding.c_str());
  m_popToRootOnClose->stealthySetValueAsJson(value.popToRootOnClose);
  m_closeOnFocusLoss->stealthySetValueAsJson(value.closeOnFocusLoss);
  m_considerPreedit->stealthySetValueAsJson(value.considerPreedit);
  m_fontSize->setText(QString::number(value.font.normal.size));
}

void GeneralSettings::handleFaviconServiceChange(const QString &service) {
  m_cfg.mergeWithUser({.faviconService = service.toStdString()});
}

void GeneralSettings::handleKeybindingChange(const QString &keybinding) {
  m_cfg.mergeWithUser({.keybinding = keybinding.toStdString()});
}

void GeneralSettings::handleIconThemeChange(const QString &iconTheme) {
  m_cfg.mergeThemeConfig({.iconTheme = iconTheme.toStdString()});
}

void GeneralSettings::handleThemeChange(const QString &id) {
  m_cfg.mergeThemeConfig({.name = id.toStdString()});
}

void GeneralSettings::handleClientSideDecorationChange(bool csd) {
  m_cfg.mergeWithUser({.launcherWindow = config::Partial<config::WindowConfig>{
                           .clientSideDecorations = config::Partial<config::WindowCSD>{.enabled = csd}}});
}

void GeneralSettings::handleFontChange(const QString &font) {
  m_cfg.mergeWithUser(
      {.font = config::Partial<config::FontConfig>{.normal = {.family = font.toStdString()}}});
}

void GeneralSettings::handleRootSearchFilesChange(bool enabled) {
  m_cfg.mergeWithUser({.searchFilesInRoot = enabled});
}

void GeneralSettings::handleOpacityChange(float opacity) {
  m_cfg.mergeWithUser({.launcherWindow = config::Partial<config::WindowConfig>{.opacity = opacity}});
}

void GeneralSettings::handlePopToRootOnCloseChange(bool popToRootOnClose) {
  m_cfg.mergeWithUser({.popToRootOnClose = popToRootOnClose});
}

void GeneralSettings::handleFontSizeChange(double size) {
  m_cfg.mergeWithUser({.font = config::Partial<config::FontConfig>{.normal{.size = (float)size}}});
}

void GeneralSettings::handleCloseOnFocusLossChange(bool value) {
  m_cfg.mergeWithUser({.closeOnFocusLoss = value});
}

void GeneralSettings::handleConsiderPreeditChange(bool value) {
  m_cfg.mergeWithUser({.considerPreedit = value});
}

void GeneralSettings::setupUI() {
  auto config = ServiceRegistry::instance()->config();
  auto appFont = QApplication::font().family();
  auto &value = m_cfg.value();

  m_rootFileSearch = new CheckboxInput;
  m_csd = new CheckboxInput;
  m_opacity = new BaseInput;
  m_themeSelector = new ThemeSelector;
  m_qThemeSelector = new QThemeSelector;
  m_fontSelector = new FontSelector;
  m_faviconSelector = new FaviconServiceSelector;
  m_keybindingSelector = new KeyBindingSelector;
  m_popToRootOnClose = new CheckboxInput;
  m_closeOnFocusLoss = new CheckboxInput;
  m_considerPreedit = new CheckboxInput;
  m_fontSize = new BaseInput;

  m_popToRootOnClose->setLabel("Pop to root on window close");

  FormWidget *form = new FormWidget;

  auto checkField = form->addField("Root file search", m_rootFileSearch);

  checkField->setInfo("Files are searched asynchronously, so if this is enabled you should expect a slight "
                      "delay for file search results to show up");

  auto popToRootOnCloseField = form->addField("Pop on close", m_popToRootOnClose);

  popToRootOnCloseField->setInfo("Whether to reset the navigation state when the launcher window is closed.");

  form->addField("Focus handling", m_closeOnFocusLoss);
  m_closeOnFocusLoss->setLabel("Close on focus loss");

  connect(m_closeOnFocusLoss, &CheckboxInput::valueChanged, this,
          &GeneralSettings::handleCloseOnFocusLossChange);

  auto considerPreeditField = form->addField("IME handling", m_considerPreedit);
  m_considerPreedit->setLabel("Include Preedit strings in search");
  considerPreeditField->setInfo("Whether to include IME Preedit strings as part of search queries.");

  connect(m_considerPreedit, &CheckboxInput::valueChanged, this,
          &GeneralSettings::handleConsiderPreeditChange);

  auto fontField = form->addField("Font", m_fontSelector);

  connect(m_fontSelector, &FontSelector::selectionChanged, this,
          [this](auto &&item) { handleFontChange(item.id()); });

  auto themeField = form->addField("Theme", m_themeSelector);
  auto opacityField = form->addField("Window opacity", m_opacity);

  m_opacity->setText(QString::number(value.launcherWindow.opacity));

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

  m_rootFileSearch->setValueAsJson(value.searchFilesInRoot);

  m_csd->setLabel("Use client-side decorations");
  m_csd->setValueAsJson(value.launcherWindow.clientSideDecorations.enabled);

  connect(m_csd, &CheckboxInput::valueChanged, this, &GeneralSettings::handleClientSideDecorationChange);

  auto csdField = form->addField("CSD", m_csd);

  csdField->setInfo(
      R"(Let Vicinae draw its own rounded borders instead of relying on the windowing system to do so. You can usually get more refined results by properly configuring your window manager.)");

  m_rootFileSearch->setLabel("Show files in root search");

  auto qThemeField = form->addField("Icon Theme", m_qThemeSelector);

  qThemeField->setInfo("The icon theme used for system icons (applications, mime types, folder icons...). "
                       "This does not affect builtin Vicinae icons.");

  auto faviconField = form->addField("Favicon Fetching", m_faviconSelector);

  faviconField->setInfo("The favicon provider used to load favicons where needed. You can turn off favicon "
                        "loading by selecting 'None'.");

  auto keybindingField = form->addField("Keybinding Scheme", m_keybindingSelector);

  keybindingField->setInfo(
      "The keybinding scheme used for navigation. Default uses Vim-style Ctrl+J/K and Ctrl+H/L; "
      "Emacs uses Ctrl+N/P and Ctrl+Opt+B/F for navigation, plus Emacs editing in the search bar "
      "(Ctrl+A/B/E/F/K/U, Opt+B/F/Backspace/Delete).");

  connect(m_keybindingSelector, &KeyBindingSelector::selectionChanged, this,
          [this](auto &&item) { handleKeybindingChange(item.id()); });

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

  form->setFixedWidth(700);

  setWidget(VStack().margins(0, 20, 0, 20).add(HStack().add(form).center()).buildWidget());
}

GeneralSettings::GeneralSettings(config::Manager &manager) : m_cfg(manager) {
  setupUI();
  setConfig(m_cfg.value());
  connect(&m_cfg, &config::Manager::configChanged, this, [this](auto next, auto prev) { setConfig(next); });
}

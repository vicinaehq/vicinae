#include "app-settings-detail.hpp"
#include "service-registry.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "ui/file-picker/file-picker.hpp"
#include <qboxlayout.h>
#include <qlogging.h>

void AppSettingsDetail::setupUI() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto appDb = ServiceRegistry::instance()->appDb();
  QJsonObject preferences = manager->getProviderPreferenceValues("apps");
  FilePicker *picker = m_filePickerItem->filerPicker();
  auto defaultSearchPaths = appDb->defaultSearchPaths();

  picker->setDelegate<AppSearchPathPickerItemDelegate>();
  picker->button()->setText("Add application folder");

  for (const auto &path : defaultSearchPaths) {
    picker->addFile(path);
  }

  for (const auto &obj : preferences.value("paths").toArray()) {
    std::filesystem::path path = obj.toString().toStdString();
    bool isDefaultSearchPath = std::ranges::any_of(defaultSearchPaths, [&](auto &&p) { return p == path; });

    if (!isDefaultSearchPath) { picker->addFile(obj.toString().toStdString()); }
  }

  auto layout = new QVBoxLayout;

  m_filePickerItem->setLabel("Application folders");
  m_filePickerItem->setDescription(
      "Application folders searched by Omnicast to index applications shown in the root search. System "
      "search paths are computed at runtime and cannot be removed.");

  layout->addWidget(m_filePickerItem);
  layout->setSpacing(20);
  layout->addStretch();
  setLayout(layout);
}

void AppSettingsDetail::savePreferences() {
  auto appDb = ServiceRegistry::instance()->appDb();
  auto manager = ServiceRegistry::instance()->rootItemManager();
  FilePicker *picker = m_filePickerItem->filerPicker();
  auto defaultSearchPaths = appDb->defaultSearchPaths();
  QJsonObject preferences;
  QJsonArray jsonPaths;

  for (const auto &file : picker->files()) {
    bool isDefaultSearchPath =
        std::ranges::any_of(defaultSearchPaths, [&](auto &&p) { return p == file.path; });
    if (!isDefaultSearchPath) { jsonPaths.push_back(file.path.c_str()); }
  }

  preferences["paths"] = jsonPaths;
  manager->setProviderPreferenceValues("apps", preferences);
}

AppSettingsDetail::AppSettingsDetail() { setupUI(); }

AppSettingsDetail::~AppSettingsDetail() { savePreferences(); }

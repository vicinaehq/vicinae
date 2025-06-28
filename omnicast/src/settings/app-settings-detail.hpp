#pragma once
#include "service-registry.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "ui/file-picker/file-picker-default-item-delegate.hpp"
#include "ui/app-selector/app-selector.hpp"
#include "ui/file-picker/file-picker.hpp"
#include <qwidget.h>

class AppSearchPathPickerItemDelegate : public DefaultFilePickerItemDelegate {
  bool isSystemPath() const {
    auto app = ServiceRegistry::instance()->appDb();

    return std::ranges::any_of(app->defaultSearchPaths(), [&](auto &&path) { return file().path == path; });
  }

  OmniListItemWidget *createWidget() const override {
    auto widget = new SelectedFileWidget;

    widget->setFile(file());
    widget->setRemovable(!isSystemPath());

    return widget;
  }
};

class AppSettingsDetail : public QWidget {
  FilePickerPreferenceFormItem *m_filePickerItem = new FilePickerPreferenceFormItem;

public:
  void setupUI();
  void savePreferences();

  AppSettingsDetail();
  ~AppSettingsDetail();
};

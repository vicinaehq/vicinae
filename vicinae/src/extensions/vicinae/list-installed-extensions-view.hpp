#pragma once

#include "services/extension-registry/extension-registry.hpp"
#include "ui/views/list-view.hpp"
#include <memory>

class ListInstalledExtensionsView : public SearchableListView {
public:
  ListInstalledExtensionsView();
  Data initData() const override;
  QString sectionName() const override { return "Installed Extensions ({count})"; }
  QString initialSearchPlaceholderText() const override;
};

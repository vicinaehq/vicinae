#pragma once
#include "common/entrypoint.hpp"
#include "ui/form/base-input.hpp"
#include "ui/views/form-view.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

class RootItemAliasView : public ManagedFormView {
  void onSubmit() override {
    auto manager = context()->services->rootItemManager();
    auto toast = context()->services->toastService();

    if (manager->setAlias(m_id, m_input->text().toStdString())) {
      toast->setToast("Alias modified", ToastStyle::Success);
      popSelf();
    } else {
      toast->setToast("Failed to modifiy alias", ToastStyle::Danger);
    }
  }

  void initializeForm() override {
    auto manager = context()->services->rootItemManager();
    auto meta = manager->itemMetadata(m_id);

    setNavigationTitle(QString("Set alias - %1").arg(meta.item->displayName()));
    setNavigationIcon(meta.item->iconUrl());

    m_input->setText(meta.alias.value_or("").c_str());
    m_input->input()->selectAll();
  }

public:
  RootItemAliasView(EntrypointId entrypoint) : m_id(entrypoint) {
    auto inputField = new FormField();
    inputField->setWidget(m_input);
    inputField->setName("Alias");
    inputField->setInfo("Additional words to index this item against");
    form()->addField(inputField);
  }

private:
  EntrypointId m_id;
  BaseInput *m_input = new BaseInput(this);
};

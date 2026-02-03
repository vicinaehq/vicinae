#pragma once
#include "extend/form-model.hpp"
#include "extension/form/extension-form-input.hpp"
#include "ui/form/selector-input.hpp"

class DropdownSelectorItem : public SelectorInput::AbstractItem {
  DropdownModel::Item m_model;

  QString generateId() const override { return m_model.value; }

  std::optional<ImageURL> icon() const override { return m_model.icon; }

  QString displayName() const override { return m_model.title; }

  AbstractItem *clone() const override { return new DropdownSelectorItem(*this); }

  const DropdownModel::Item &item() const { return m_model; }

public:
  DropdownSelectorItem(const DropdownModel::Item &model) : m_model(model) {}
};

class ExtensionDropdown : public ExtensionFormInput {
  SelectorInput *m_input = new SelectorInput(this);
  std::shared_ptr<FormModel::DropdownField> m_model;
  bool m_hasPendingResetSelection = false;

  void handleSelectionChanged(const SelectorInput::AbstractItem &item) {
    if (auto onChange = m_model->onChange) {
      m_hasPendingResetSelection = true;
      m_extensionNotifier->notify(*onChange, item.id());
    }
    if (auto value = m_model->value) { m_input->setValue(value->toString()); }
  }

  void clear() override { m_input->clear(); }

  void handleTextChanged(const QString &text) {
    if (auto change = m_model->onSearchTextChange) { m_extensionNotifier->notify(*change, text); }
  }

public:
  QJsonValue asJsonValue() const override {
    if (auto value = m_input->value()) { return value->id(); }
    return QJsonValue();
  }

  void setValueAsJson(const QJsonValue &value) override { m_input->setValue(value.toString()); }

  void render(const std::shared_ptr<FormModel::IField> &field) override {
    OmniList::SelectionPolicy selectionPolicy = OmniList::PreserveSelection;

    m_model = std::static_pointer_cast<FormModel::DropdownField>(field);

    if (m_hasPendingResetSelection) {
      m_hasPendingResetSelection = false;
      selectionPolicy = OmniList::SelectFirst;
    }

    m_input->resetModel();
    std::vector<std::shared_ptr<SelectorInput::AbstractItem>> rootSectionItems;

    for (const auto &item : m_model->m_items) {
      if (auto listItem = std::get_if<DropdownModel::Item>(&item)) {
        // root item
        rootSectionItems.emplace_back(std::make_shared<DropdownSelectorItem>(*listItem));
      } else if (auto section = std::get_if<DropdownModel::Section>(&item)) {
        // section
        std::vector<std::shared_ptr<SelectorInput::AbstractItem>> sectionItems;
        for (const auto &sectionItem : section->items) {
          // section items
          sectionItems.emplace_back(std::make_shared<DropdownSelectorItem>(sectionItem));
        }
        if (!sectionItems.empty()) { m_input->addSection(section->title, sectionItems); }
      }
    }
    if (!rootSectionItems.empty()) { m_input->addSection("", rootSectionItems); }
    m_input->updateModel();

    m_input->setIsLoading(m_model->isLoading);
    m_input->setEnableDefaultFilter(m_model->filtering);

    if (auto value = m_model->value) m_input->setValue(value->toString());
  }

  ExtensionDropdown() {
    setWrapped(m_input, m_input->focusNotifier());
    connect(m_input, &SelectorInput::selectionChanged, this, &ExtensionDropdown::handleSelectionChanged);
    connect(m_input, &SelectorInput::textChanged, this, &ExtensionDropdown::handleTextChanged);
  }
};

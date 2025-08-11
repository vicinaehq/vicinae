#include "extension/extension-list-component.hpp"
#include "extend/list-model.hpp"
#include "extension/extension-list-detail.hpp"
#include <chrono>
#include <memory>
#include <qcoreevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <ranges>
#include "extension/form/extension-dropdown.hpp"
#include "ui/form/app-picker-input.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/omni-list/omni-list.hpp"

static const std::chrono::milliseconds THROTTLE_DEBOUNCE_DURATION(300);

static const KeyboardShortcutModel primaryShortcut{.key = "return"};
static const KeyboardShortcutModel secondaryShortcut{.key = "return", .modifiers = {"shift"}};

void ExtensionListComponent::renderDropdown(const DropdownModel &dropdown) {
  OmniList::SelectionPolicy selectionPolicy = OmniList::PreserveSelection;

  m_dropdownDebounce->setInterval(dropdown.throttle ? THROTTLE_DEBOUNCE_DURATION
                                                    : std::chrono::milliseconds(0));

  if (dropdown.dirty) {
    if (m_dropdownShouldResetSelection) {
      m_dropdownShouldResetSelection = false;
      selectionPolicy = OmniList::SelectFirst;
    }

    m_selector->resetModel();

    std::vector<std::shared_ptr<SelectorInput::AbstractItem>> freeSectionItems;

    for (const auto &item : dropdown.children) {
      if (auto listItem = std::get_if<DropdownModel::Item>(&item)) {
        freeSectionItems.emplace_back(std::make_shared<DropdownSelectorItem>(*listItem));
      } else if (auto section = std::get_if<DropdownModel::Section>(&item)) {
        if (!freeSectionItems.empty()) {
          // m_selector->addSection("", freeSectionItems);
          freeSectionItems.clear();
        }

        auto mapItem = [](auto &&item) -> std::shared_ptr<SelectorInput::AbstractItem> {
          return std::make_unique<DropdownSelectorItem>(item);
        };
        auto items = section->items | std::views::transform(mapItem) | std::ranges::to<std::vector>();

        m_selector->addSection(section->title, items);
      }
    }

    if (!freeSectionItems.empty()) {
      m_selector->addSection("", freeSectionItems);
      freeSectionItems.clear();
    }

    m_selector->updateModel();
  }

  m_selector->setEnableDefaultFilter(dropdown.filtering.enabled);

  if (auto controlledValue = dropdown.value) {
    m_selector->setValue(*controlledValue);
  } else if (!m_selector->value()) {
    if (dropdown.defaultValue) {
      m_selector->setValue(*dropdown.defaultValue);
    } else if (auto item = m_selector->list()->firstSelectableItem()) {
      m_selector->setValue(item->generateId());
    }
  }

  m_selector->setIsLoading(dropdown.isLoading);
}

void ExtensionListComponent::render(const RenderModel &baseModel) {
  ++m_renderCount;
  auto newModel = std::get<ListModel>(baseModel);

  if (auto accessory = newModel.searchBarAccessory) {
    auto dropdown = std::get<DropdownModel>(*accessory);

    renderDropdown(dropdown);
  }

  m_selector->setVisible(newModel.searchBarAccessory.has_value() && isVisible());

  if (!newModel.navigationTitle.isEmpty()) { setNavigationTitle(newModel.navigationTitle); }
  if (!newModel.searchPlaceholderText.isEmpty()) { setSearchPlaceholderText(newModel.searchPlaceholderText); }
  if (auto text = newModel.searchText) { setSearchText(*text); }

  if (newModel.throttle != _model.throttle) {
    _debounce->stop();

    if (newModel.throttle) {
      _debounce->setInterval(THROTTLE_DEBOUNCE_DURATION);
    } else {
      _debounce->setInterval(0);
    }
  }

  setLoading(newModel.isLoading);

  if (newModel.dirty) {
    OmniList::SelectionPolicy policy = OmniList::SelectFirst;

    if (_shouldResetSelection) {
      _shouldResetSelection = false;
      policy = OmniList::SelectFirst;
    } else {
      policy = OmniList::PreserveSelection;
    }

    m_list->setModel(newModel.items, policy);
  }

  /*
  if (!newModel.searchText) {
    if (_shouldResetSelection) {
      if (newModel.filtering) {
        m_list->setFilter(searchText());
      } else {
        m_list->setFilter("");
      }
    }
  }
  */

  _model = newModel;

  if (auto selected = m_list->selected(); selected && newModel.dirty) {
    m_split->setDetailVisibility(_model.isShowingDetail);

    if (auto detail = selected->detail) {
      m_split->detailWidget()->show();
      if (m_split->isDetailVisible()) {
        m_detail->updateDetail(*detail);
      } else {
        m_detail->setDetail(*detail);
      }
    } else {
      m_split->detailWidget()->hide();
    }

    if (auto panel = selected->actionPannel; panel && _model.dirty && panel->dirty) {
      setActionPanel(*panel);
    }
  }

  if (m_list->empty()) {
    if (auto panel = newModel.actions; panel && panel->dirty) { setActionPanel(*panel); }
  }
}

void ExtensionListComponent::onSelectionChanged(const ListItemViewModel *next) {
  if (!next) {
    m_split->setDetailVisibility(false);

    if (auto &pannel = _model.actions) {
      setActionPanel(*pannel);
    } else {
      setActionPanel({});
    }
    return;
  }

  if (auto handler = _model.onSelectionChanged) { notify(*handler, {next->id}); }

  m_split->setDetailVisibility(_model.isShowingDetail);

  if (auto detail = next->detail) { m_detail->setDetail(*detail); }

  if (auto pannel = next->actionPannel) { setActionPanel(*pannel); }
}

void ExtensionListComponent::handleDropdownSelectionChanged(const SelectorInput::AbstractItem &item) {
  if (auto accessory = _model.searchBarAccessory) {
    if (auto dropdown = std::get_if<DropdownModel>(&*accessory)) {
      if (auto onChange = dropdown->onChange) { notify(*onChange, {item.generateId()}); }
    }
  }
}

void ExtensionListComponent::handleDropdownSearchChanged(const QString &text) {

  if (auto accessory = _model.searchBarAccessory) {
    if (auto dropdown = std::get_if<DropdownModel>(&*accessory)) {
      m_dropdownShouldResetSelection = !dropdown->filtering.enabled;

      if (auto onChange = dropdown->onSearchTextChange) { emit notify(*onChange, {text}); }
    }
  }
}

void ExtensionListComponent::handleDebouncedSearchNotification() {
  auto text = searchText();
  auto itemMatches = [&](const ListItemViewModel &model) -> bool {
    return model.title.contains(searchText(), Qt::CaseInsensitive);
  };

  if (_model.filtering) {
    m_list->setFilter(searchText());
  } else {
    m_list->setFilter("");
  }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;

    notify(*handler, {text});
  }
}

void ExtensionListComponent::onItemActivated(const ListItemViewModel &item) { executePrimaryAction(); }

void ExtensionListComponent::textChanged(const QString &text) {
  if (text == _model.searchText) { /*m_topBar->input->setText(*_model.searchText);*/
    return;
  }

  if (_model.filtering) {
    m_list->setFilter(text);
  } else {
    m_list->setFilter("");
  }

  if (auto handler = _model.onSearchTextChange) {
    // flag next render to reset the search selection
    _shouldResetSelection = !_model.filtering;
    notify(*handler, {text});
  }

  //_debounce->start();
}

ExtensionListComponent::ExtensionListComponent() : _debounce(new QTimer(this)), _shouldResetSelection(true) {
  m_selector->setMinimumWidth(300);
  m_selector->setEnableDefaultFilter(false);
  m_selector->hide();
  setDefaultActionShortcuts({primaryShortcut, secondaryShortcut});
  m_split->setMainWidget(m_list);
  m_split->setDetailWidget(m_detail);
  m_split->detailWidget()->hide();
  setupUI(m_split);

  _debounce->setSingleShot(true);
  connect(_debounce, &QTimer::timeout, this, &ExtensionListComponent::handleDebouncedSearchNotification);
  connect(m_list, &ExtensionList::selectionChanged, this, &ExtensionListComponent::onSelectionChanged);
  connect(m_list, &ExtensionList::itemActivated, this, &ExtensionListComponent::onItemActivated);
  connect(m_selector, &SelectorInput::selectionChanged, this,
          &ExtensionListComponent::handleDropdownSelectionChanged);
  connect(m_selector, &SelectorInput::textChanged, this,
          &ExtensionListComponent::handleDropdownSearchChanged);
}

ExtensionListComponent::~ExtensionListComponent() {}

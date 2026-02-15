#pragma once
#include <algorithm>
#include <qwidget.h>
#include "common/scored.hpp"
#include "lib/fzf.hpp"
#include "keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "config/config.hpp"
#include "service-registry.hpp"
#include "services/keybinding/keybinding-service.hpp"
#include "simple-view.hpp"
#include "ui/empty-view/empty-view.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "ui/split-detail/split-detail.hpp"
#include "ui/vlist/common/vertical-list-model.hpp"
#include "ui/vlist/vlist.hpp"

template <typename ModelType> class TypedListView : public SimpleView {
public:
  using ItemType = typename ModelType::Item;

  TypedListView(QWidget *parent = nullptr) {}

  virtual ~TypedListView() = default;

  void setModel(ModelType *model) {
    m_model = model;
    m_model->setParent(this);
    m_list->setModel(model);
  }

  ModelType *model() const { return m_model; }

  void setHorizontalNavigation(bool value) { m_horizontalNavigation = value; }

protected:
  virtual QWidget *generateDetail(const ItemType &item) const { return nullptr; }
  virtual std::unique_ptr<CompleterData> createCompleter(const ItemType &item) const { return nullptr; }
  virtual std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const { return nullptr; }

  virtual std::optional<QString> navigationTitle(const ItemType &item) const { return std::nullopt; }

  virtual void itemSelected(const ItemType &item) {}

  virtual void emptied() {}

  virtual bool inputFilter(QKeyEvent *event) override {
    auto config = ServiceRegistry::instance()->config();
    const std::string &keybinding = config->value().keybinding;

    if (Keyboard::Shortcut(Keybind::OpenSearchAccessorySelector) == event) {
      // FIXME: improve this so that we don't need dynamic cast
      if (auto accessory = dynamic_cast<SelectorInput *>(currentSearchAccessory())) {
        accessory->openSelector();
      }
    }

    if (event->modifiers() == Qt::KeyboardModifier::ControlModifier) {
      if (KeyBindingService::isUpKey(event, keybinding)) {
        return m_list->selectUp();
      } else if (KeyBindingService::isDownKey(event, keybinding)) {
        return m_list->selectDown();
      } else if (m_horizontalNavigation && KeyBindingService::isLeftKey(event, keybinding)) {
        return m_list->selectLeft();
      } else if (m_horizontalNavigation && KeyBindingService::isRightKey(event, keybinding)) {
        return m_list->selectRight();
      }
    } else if (event->modifiers().toInt() == 0) {
      switch (event->key()) {
      case Qt::Key_Up:
        return m_list->selectUp();
      case Qt::Key_Down:
        return m_list->selectDown();
      case Qt::Key_Left:
        if (m_horizontalNavigation) return m_list->selectLeft();
        break;
      case Qt::Key_Right:
        if (m_horizontalNavigation) return m_list->selectRight();
        break;
      case Qt::Key_Tab: {
        if (!context()->navigation->hasCompleter()) {
          m_list->selectNext();
          return true;
        }
        break;
      }
      case Qt::Key_Return:
      case Qt::Key_Enter:
        m_list->activateCurrentSelection();
        return true;
      default:
        break;
      }
    }

    return SimpleView::inputFilter(event);
  }

  void forceReselection() {}

  virtual void initialize() override { setupUI(); }

  virtual void selectionChanged(std::optional<typename ModelType::Index> idx) {
    if (!idx || !m_model) {
      destroyCompleter();
      clearActions();
      setNavigationTitle(initialNavigationTitle());
      m_split->setDetailVisibility(false);

      if (m_model && m_model->isEmpty() && !(searchText().isEmpty() && isLoading())) {
        m_content->setCurrentWidget(m_emptyView);
        emptied();
      }

      return;
    }

    if (m_content->currentWidget() != m_split) { m_content->setCurrentWidget(m_split); }

    std::optional<ItemType> item = m_model->fromIndex(idx.value());

    if (!item) {
      qWarning() << "Invalid data at index" << idx.value();
      return;
    }

    setActions(createActionPanel(item.value()));

    auto &nav = context()->navigation;

    if (auto completer = createCompleter(item.value())) {
      nav->createCompletion(completer->arguments, completer->iconUrl);
    } else {
      nav->destroyCurrentCompletion();
    }

    if (auto title = navigationTitle(item.value())) {
      setNavigationTitle(QString("%1 - %2").arg(initialNavigationTitle()).arg(title.value()));
    } else {
      setNavigationTitle(initialNavigationTitle());
    }

    auto detail = generateDetail(item.value());

    if (detail) { setDetail(detail); }
    m_split->setDetailVisibility(detail);

    itemSelected(item.value());
  }

  std::optional<ItemType> currentItem() const {
    return m_list->currentSelection().and_then([&](auto idx) { return m_model->fromIndex(idx); });
  }

  void refreshCurrent() { selectionChanged(m_list->currentSelection()); }

  virtual void itemActivated(typename ModelType::Index idx) { executePrimaryAction(); }

  QWidget *detail() const { return m_split->detailWidget(); }

  void setDetail(QWidget *widget) { m_split->setDetailWidget(widget); }

  /**
   * Wrap the list view UI inside a layout if necessary
   */
  virtual QWidget *wrapUI(QWidget *content) { return content; }

  void setupUI() {
    m_split = new SplitDetailWidget(this);
    m_content = new QStackedWidget(this);
    m_emptyView = new EmptyViewWidget(this);
    m_list = new vicinae::ui::VListWidget;
    m_content->addWidget(m_split);
    m_content->addWidget(m_emptyView);
    m_content->setCurrentWidget(m_split);

    m_emptyView->setTitle("No results");
    m_emptyView->setDescription("No results matching your search. You can try to refine your search.");
    m_emptyView->setIcon(ImageURL::builtin("magnifying-glass"));

    m_split->setMainWidget(m_list);

    connect(m_list, &vicinae::ui::VListWidget::itemSelected, this, &TypedListView::selectionChanged);
    connect(m_list, &vicinae::ui::VListWidget::itemActivated, this, &TypedListView::itemActivated);

    SimpleView::setupUI(wrapUI(m_content));
  }

  vicinae::ui::VListWidget *m_list = nullptr;
  ModelType *m_model = nullptr;
  SplitDetailWidget *m_split = nullptr;
  QStackedWidget *m_content = nullptr;
  EmptyViewWidget *m_emptyView = nullptr;
  bool m_horizontalNavigation = false;
};

struct FilteredItemData {
  std::string id;
  std::string title;
  std::optional<std::string> subtitle;
  std::optional<ImageURL> icon;
  std::optional<std::string> alias;
  std::vector<std::string> keywords;
  AccessoryList accessories;
  bool isActive = false;
};

/**
 * Vertical list model with built-in fuzzy filtering capabilities.
 */
class FilteredVerticalListModel : public vicinae::ui::VerticalListModel<FilteredItemData> {
public:
  struct Section {
    std::string name;
    std::vector<FilteredItemData> items;
  };
  struct ScoredSection {
    std::string name;
    int bestScore = 0;
    std::vector<Scored<FilteredItemData>> items;
  };

  void setSections(const std::vector<Section> &sections) { m_sections = sections; }
  void setItems(const std::vector<FilteredItemData> &items) { m_sections = {Section{.items = items}}; }

  void setFilter(std::string_view query) {
    m_filteredSections.clear();
    for (const Section &section : m_sections) {
      ScoredSection scoredSection{.name = section.name};

      for (const FilteredItemData &item : section.items) {
        int score = query.empty() ? 1 : [&]() {
          using WS = fzf::WeightedString;
          std::vector<WS> fields;

          fields.emplace_back(item.title, 1.0f);
          
          for (const auto &kw : item.keywords) {
            if (!kw.empty()) fields.emplace_back(kw, 0.5f);
          }

          return fzf::defaultMatcher.fuzzy_match_v2_score_query(fields, query, false);
        }();
        if (!score) continue;
        scoredSection.bestScore = std::max(scoredSection.bestScore, score);
        scoredSection.items.emplace_back(Scored<FilteredItemData>{.data = item, .score = score});
      }

      std::ranges::stable_sort(scoredSection.items, [](auto &&a, auto &&b) { return a.score > b.score; });

      if (!scoredSection.items.empty()) { m_filteredSections.emplace_back(scoredSection); }
    }

    std::ranges::stable_sort(m_filteredSections,
                             [](auto &&a, auto &&b) { return a.bestScore > b.bestScore; });

    emit dataChanged();
  }

protected:
  virtual ItemData createItemData(const FilteredItemData &item) const override {
    return ItemData{
        .title = item.title.c_str(),
        .subtitle = item.subtitle.value_or("").c_str(),
        .icon = item.icon,
        .alias = item.alias.value_or("").c_str(),
        .accessories = item.accessories,
    };
  }

  virtual int sectionCount() const override { return m_filteredSections.size(); }
  virtual int sectionItemCount(int id) const override { return m_filteredSections.at(id).items.size(); }

  virtual FilteredItemData sectionItemAt(int id, int itemIdx) const override {
    return m_filteredSections.at(id).items.at(itemIdx).data;
  }

  virtual int sectionIdFromIndex(int idx) const override { return idx; }
  virtual std::string_view sectionName(int id) const override { return m_filteredSections.at(id).name; }
  virtual VListModel::StableID stableId(const FilteredItemData &item) const override { return hash(item.id); }

private:
  std::vector<Section> m_sections;
  std::vector<ScoredSection> m_filteredSections;
};

template <typename T> class FilteredTypedListView : public TypedListView<FilteredVerticalListModel> {
public:
  struct Section {
    std::string name;
    std::vector<T> items;
  };
  using DataSet = std::vector<Section>;

  virtual FilteredItemData mapFilteredData(const T &item) const = 0;
  virtual std::unique_ptr<ActionPanelState> createActionPanel(const T &item) const = 0;
  virtual QWidget *generateDetail(const T &item) const { return nullptr; }
  virtual std::unique_ptr<CompleterData> createCompleter(const T &item) const { return nullptr; }

  virtual std::unique_ptr<CompleterData> createCompleter(const ItemType &item) const override {
    if (auto it = m_objectMap.find(item.id); it != m_objectMap.end()) { return createCompleter(*it->second); }
    return nullptr;
  }

  QWidget *generateDetail(const ItemType &item) const override {
    if (auto it = m_objectMap.find(item.id); it != m_objectMap.end()) { return generateDetail(*it->second); }
    return nullptr;
  }

  std::unique_ptr<ActionPanelState> createActionPanel(const ItemType &item) const override {
    if (auto it = m_objectMap.find(item.id); it != m_objectMap.end()) {
      return createActionPanel(*it->second);
    }
    return {};
  }

  const T *currentItem() const {
    return TypedListView::currentItem()
        .transform([&](auto &&item) -> const T * {
          if (auto it = m_objectMap.find(item.id); it != m_objectMap.end()) { return &*it->second; }
          return nullptr;
        })
        .value_or(nullptr);
  }

  virtual DataSet initializeDataSet() = 0;

  void initialize() override final {
    TypedListView::initialize();
    setModel(new FilteredVerticalListModel);
    setDataSet(initializeDataSet());
  }

  void setDataSet(const DataSet &set) {
    m_data = set;
    m_objectMap.clear();

    std::vector<FilteredVerticalListModel::Section> modelSections;

    for (const auto &section : m_data) {
      FilteredVerticalListModel::Section msec{.name = section.name};

      for (const auto &item : section.items) {
        auto data = mapFilteredData(item);
        m_objectMap[data.id] = &item;
        msec.items.emplace_back(data);
      }

      modelSections.emplace_back(msec);
    }

    model()->setSections(modelSections);
    model()->setFilter(searchText().toStdString());
    model()->dataChanged();
  }

  void textChanged(const QString &text) override {
    model()->setFilter(text.toStdString());
    m_list->selectFirst();
  }

private:
  DataSet m_data;
  std::unordered_map<std::string, const T *> m_objectMap;
};

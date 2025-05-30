#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "timer.hpp"
#include "ui/omni-list-item-widget-wrapper.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/default-list-item-widget.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <cmath>
#include <cstdio>
#include <QPainterPath>
#include <functional>
#include <iterator>
#include <memory>
#include <qboxlayout.h>
#include <qfuture.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qwidget.h>
#include <ranges>
#include <stack>
#include <unordered_map>
#include <variant>

class OmniList : public QWidget {

public:
  enum ScrollBehaviour {
    ScrollRelative,
    ScrollAbsolute,
  };

  class AbstractVirtualItem {
    mutable QString m_id;

  public:
    enum ListRole { ListItem, ListSection, ListDivider };

    /**
     * Default implementation creates a widget to calculate height.
     * This is inefficient and should be overidden for items that are very
     * often used.
     */
    virtual int calculateHeight(int width) const {
      QWidget *ruler = createWidget();

      if (ruler) {
        ruler->deleteLater();
        return ruler->sizeHint().height();
      }

      return 0;
    }

    /**
     * Whether the virtual height of this item type can change across many items.
     * If set to true, the height returned by the `calculateHeight`	method is cached
     * until the width allocation for the widget changes.
     */
    virtual bool hasUniformHeight() const { return false; }

    virtual OmniListItemWidget *createWidget() const = 0;
    virtual void recycle(QWidget *base) const;
    virtual bool recyclable() const;
    virtual bool hasPartialUpdates() const { return false; }
    virtual void refresh(QWidget *widget) const {}
    virtual QString generateId() const = 0;

    QString id() const {
      if (m_id.isEmpty()) { m_id = generateId(); }
      return m_id;
    }

    virtual size_t typeId() const;
    virtual bool selectable() const;
    virtual ListRole role() const;

    /**
     * Whether this item provides context for the item right below it.
     * If set to true, the list will scroll to this item if the item right below it is selected and this item
     * is not in the viewport.
     */
    virtual bool isContextProvider() const { return false; }

    bool isListItem() const;
    bool isSection() const;
    virtual ~AbstractVirtualItem() {}
  };

  class DividerItem : public AbstractVirtualItem {
    QString m_id;

    class DividerWidget : public OmniListItemWidget {
      HDivider *divider = new HDivider(this);

    public:
      DividerWidget() {
        auto layout = new QVBoxLayout;

        layout->setContentsMargins(0, 5, 0, 5);
        layout->addWidget(divider);
        setLayout(layout);
      }
    };

  public:
    int calculateHeight(int width) const override {
      static DividerWidget widget;
      return widget.sizeHint().height();
    }
    OmniListItemWidget *createWidget() const override { return new DividerWidget; }
    QString generateId() const override { return m_id; }
    bool selectable() const override { return false; }
    ListRole role() const override { return ListRole::ListDivider; }

    DividerItem() : m_id(QUuid::createUuid().toString()) {}
  };

  class VirtualSection : public AbstractVirtualItem {
    QString _name;
    int _count;
    bool _showCount;

  public:
    ListRole role() const override { return ListRole::ListSection; }
    bool selectable() const override { return false; }

    virtual int spacing() const { return 0; }

    const QString &name() const;
    QString generateId() const override;

    /**
     * Whether or not this header itself should be displayed.
     * Usually hidden when the section is empty or has no name
     */
    virtual bool showHeader();

    virtual int calculateItemWidth(int width, int index) const;
    virtual int calculateItemX(int x, int index) const;
    virtual void initWidth(int width);

    void setCount(int count);
    virtual int count() const;

    OmniListItemWidget *createWidget() const override;
    int calculateHeight(int width) const override;
    void setShowCount(bool value);

    VirtualSection(const QString &name, bool showCount = true);
    ~VirtualSection() {}
  };

  class SectionWithoutCount : public VirtualSection {
  public:
    SectionWithoutCount(const QString &name) : VirtualSection(name) {}
  };

  class AbstractItemFilter {
  public:
    virtual bool matches(const AbstractVirtualItem &item) = 0;
  };

  using UpdateItemCallback = std::function<void(AbstractVirtualItem *)>;
  enum SelectionPolicy {
    SelectFirst,
    KeepSelection,
    PreserveSelection,
    SelectNone,
  };

  class Divider {
    std::unique_ptr<AbstractVirtualItem> m_item;

  public:
    AbstractVirtualItem *item() const { return m_item.get(); }
    Divider() { m_item = std::make_unique<DividerItem>(); }
  };

  struct Spacer {
    int value;

  public:
    Spacer(int n) : value(n) {}
  };

  /**
   * A structured list of items.
   * A section can have a specific header item that will only be shown if the section is not
   * otherwise empty.
   */
  class Section {
  public:
    using VirtualWidget = std::unique_ptr<AbstractVirtualItem>;
    using LayoutItem = std::variant<VirtualWidget, Spacer>;

  private:
    std::vector<LayoutItem> m_items;
    std::unique_ptr<AbstractVirtualItem> m_headerItem;
    QString m_title;
    QString m_subtitle;
    int m_columns = 1;
    int m_spacing = 0;

  public:
    Section(const QString &title = "", const QString &subtitle = "") : m_title(title), m_subtitle(subtitle) {
      m_headerItem = std::make_unique<VirtualSection>(title);
    }

    /**
     * Set spacing for this section's items.
     * This equally applies to horizontally and vertically laid out items.
     */
    Section &setSpacing(int n) {
      m_spacing = std::max(0, n);
      return *this;
    }

    int columns() const { return m_columns; }

    void setColumns(int n) { m_columns = std::max(1, n); }

    Section &withTitle(const QString &title) {
      m_title = title;
      return *this;
    }
    Section &withSubtitle(const QString &subtitle) {
      m_subtitle = subtitle;
      return *this;
    }

    Section &withCapacity(size_t n) {
      m_items.reserve(n);
      return *this;
    }

    Section &withHeaderItem(std::unique_ptr<AbstractVirtualItem> item) {
      m_headerItem = std::move(item);
      return *this;
    }

    const QString &title() const { return m_title; }

    int spacing() const { return m_spacing; }

    AbstractVirtualItem *headerItem() { return m_headerItem.get(); }

    const std::vector<LayoutItem> &items() const { return m_items; }

    Section &addItem(std::unique_ptr<AbstractVirtualItem> item) {
      m_items.emplace_back(std::move(item));
      return *this;
    }

    Section &addSpacing(int value) {
      m_items.emplace_back(Spacer(value));
      return *this;
    }

    Section &addDivider() {
      m_items.emplace_back(std::make_unique<DividerItem>());
      return *this;
    }

    Section &addItems(std::vector<std::unique_ptr<AbstractVirtualItem>> items) {
      m_items.insert(m_items.end(), std::make_move_iterator(items.begin()),
                     std::make_move_iterator(items.end()));
      return *this;
    }
  };

  using RootListItem = std::variant<std::unique_ptr<Section>, std::unique_ptr<AbstractVirtualItem>>;

  using ModelItem = std::variant<std::unique_ptr<Section>, Divider>;

  std::vector<ModelItem> m_model;

private:
  Q_OBJECT

  const size_t DEFAULT_SELECTION_INDEX = -1;

  struct {
    int lower = -1;
    int upper = -1;
  } visibleIndexRange;
  struct {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
  } margins;
  struct ListItemInfo {
    std::unique_ptr<AbstractVirtualItem> item;
    bool filtered = false;
    int vIndex = -1;
    int cachedHeight = 0;
    QString id;
    int sectionIndex = 0;
  };
  struct VirtualListWidgetInfo {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int index = 0;
    const AbstractVirtualItem *item = nullptr;
  };
  struct VirtualSectionInfo {
    int index = 0;
    int visibleCount = 0;
    std::unique_ptr<AbstractVirtualItem> section;
    std::vector<ListItemInfo> items;
  };
  struct SectionCalculationContext {
    VirtualSection *section = nullptr;
    int index = 0;
    int x = 0;
    int maxHeight = 0;
  };
  struct CachedWidget {
    OmniListItemWidgetWrapper *widget = nullptr;
    size_t recyclingId = -1;
  };
  struct NavigationBehaviour {};

  struct SortingConfig {
    bool enabled = false;
    bool preserveSectionOrder = false;
  };

  QScrollBar *scrollBar;
  std::vector<VirtualListWidgetInfo> _virtual_items;
  std::unordered_map<size_t, OmniListItemWidgetWrapper *> _visibleWidgets;
  std::unordered_map<QString, CachedWidget> _widgetCache;
  std::unordered_map<size_t, std::stack<OmniListItemWidgetWrapper *>> _widgetPools;
  std::unordered_map<QString, AbstractVirtualItem *> _idItemMap;
  std::unique_ptr<AbstractItemFilter> _filter;
  std::vector<VirtualSectionInfo> m_sections;

  SortingConfig m_sortingConfig;

  std::vector<std::pair<size_t, int>> m_cachedHeights;

  int _selected;
  QString _selectedId;
  bool _isUpdating;
  int _virtualHeight;

  void itemClicked(int index);
  void itemDoubleClicked(int index) const;

  void updateVisibleItems();
  void calculateHeights();

  bool isDividableContent(const ModelItem &item) {
    if (auto section = std::get_if<std::unique_ptr<Section>>(&item)) {
      return (*section)->items().size() > 0;
    }

    return false;
  }

  bool isInViewport(const QRect &bounds) {
    int low = scrollBar->value();
    int high = low + height();

    return bounds.y() >= low && bounds.y() <= high;
  }

  int calculateItemHeight(const AbstractVirtualItem *item, int width) {
    if (!item->hasUniformHeight()) { return item->calculateHeight(width); }

    auto pred = [&](auto &&pair) { return pair.first == item->typeId(); };
    auto match = std::ranges::find_if(m_cachedHeights, pred);
    size_t typeId = item->typeId();

    if (match != m_cachedHeights.end()) { return match->second; }

    int height = item->calculateHeight(width);

    qCritical() << "calculate height" << height;

    m_cachedHeights.push_back({typeId, height});

    return height;
  }

  void calculateHeightsFromModel() {
    Timer timer;
    _virtual_items.clear();

    int yOffset = 0;
    int availableWidth = width() - margins.left - margins.right;
    std::optional<SectionCalculationContext> sctx;
    std::unordered_map<QString, CachedWidget> updatedCache;

    auto view = m_model | std::views::filter([](const auto &item) {
                  return std::holds_alternative<std::unique_ptr<Section>>(item);
                }) |
                std::views::transform([](const auto &section) {
                  return std::get<std::unique_ptr<Section>>(section)->items().size();
                });
    auto totalSize = std::ranges::fold_left(view, 0, std::plus<size_t>());

    _virtual_items.reserve(totalSize);

    size_t i = -1;

    for (const auto &item : m_model) {
      ++i;
      if (auto divider = std::get_if<Divider>(&item)) {
        if (yOffset == 0) continue;
        if (i + 1 == m_model.size() || !isDividableContent(m_model.at(i + 1))) continue;

        VirtualListWidgetInfo vinfo{.x = 0,
                                    .y = yOffset,
                                    .width = width(),
                                    .height = divider->item()->calculateHeight(availableWidth),
                                    .item = divider->item()};

        _virtual_items.push_back(vinfo);
        yOffset += vinfo.height;

      } else if (auto p = std::get_if<std::unique_ptr<Section>>(&item)) {
        auto &section = *p;
        auto &items = section->items();
        int spaceWidth = section->spacing() * (section->columns() - 1);
        int columnWidth = (availableWidth - spaceWidth) / section->columns();

        if (items.empty()) continue;

        if (auto header = section->headerItem(); header && !section->title().isEmpty()) {
          VirtualListWidgetInfo vinfo{.x = margins.left,
                                      .y = yOffset,
                                      .width = availableWidth,
                                      .height = header->calculateHeight(availableWidth),
                                      .item = header};

          _virtual_items.push_back(vinfo);
          yOffset += vinfo.height;
        }

        sctx = {.section = nullptr, .x = margins.left, .maxHeight = 0};

        qDebug() << "rendering" << items.size() << "items";

        for (auto &sectionItem : items) {
          if (auto spacer = std::get_if<Spacer>(&sectionItem)) {
            yOffset += spacer->value;
            continue;
          }

          if (auto p = std::get_if<Section::VirtualWidget>(&sectionItem)) {
            auto &item = *p;

            if (_filter && !_filter->matches(*p->get())) continue;

            int vIndex = _virtual_items.size();
            int height = 0;
            int width = columnWidth;
            int x = margins.left;
            int y = yOffset;

            if (sctx) {
              // width = sctx->section->calculateItemWidth(width, sctx->index);
              //  x = sctx->section->calculateItemX(sctx->x, sctx->index);
              if (sctx->x == margins.left && sctx->index > 0) {
                yOffset += section->spacing();
                y = yOffset;
              }

              x = sctx->x;
              sctx->x = sctx->x + width + section->spacing();
              height = calculateItemHeight(item.get(), width);
              sctx->maxHeight = std::max(sctx->maxHeight, height);

              if (sctx->x >= availableWidth) {
                yOffset += sctx->maxHeight;
                sctx->x = margins.left;
                sctx->maxHeight = 0;
              }

              ++sctx->index;
            } else {
              height = item->calculateHeight(width);
              yOffset += height;
            }

            VirtualListWidgetInfo vinfo{.x = x, .y = y, .width = width, .height = height, .item = item.get()};
            QRect rect{x, y, width, height};

            if (isInViewport(rect)) {
              qWarning() << "in viewport" << item->id();
              if (auto it = _widgetCache.find(item->id()); it != _widgetCache.end()) {
                if (item->hasPartialUpdates()) { item->refresh(it->second.widget->widget()); }

                updatedCache[item->id()] = it->second;
              }
            }

            // TODO: if in viewport, look for cached entry

            _virtual_items.push_back(vinfo);
          }
        }

        if (sctx) { yOffset += sctx->maxHeight; }
      }
    }

    if (!_virtual_items.empty()) { yOffset += margins.bottom + margins.top; }

    for (auto &[key, cache] : _widgetCache) {
      if (auto it = updatedCache.find(key); it == updatedCache.end()) {
        if (cache.recyclingId) {
          moveToPool(cache.recyclingId, cache.widget);
        } else {
          cache.widget->deleteLater();
        }
      }
    }

    _visibleWidgets.clear();
    _widgetCache = updatedCache;

    timer.time("calculateHeightsFromModel");

    auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //  qDebug() << "calculateHeights() took" << duration << "ms";

    scrollBar->setMaximum(std::max(0, yOffset - height()));
    scrollBar->setMinimum(0);
    bool vchanged = _virtualHeight != yOffset;
    _virtualHeight = yOffset;
    updateVisibleItems();

    if (vchanged) { emit virtualHeightChanged(_virtualHeight); }
  }

  int indexOfItem(const QString &id) const;

  void clearVisibleWidgets();

  OmniListItemWidgetWrapper *takeFromPool(size_t type);
  void moveToPool(size_t type, OmniListItemWidgetWrapper *wrapper);

  bool isSelectionValid() const { return _selected >= 0 && _selected < _virtual_items.size(); }

  void setSelectedIndex(int index, ScrollBehaviour scrollBehaviour = ScrollRelative);
  int previousRowIndex(int index);
  int nextRowIndex(int index);

  void applySorting();

  void scrollTo(int idx, ScrollBehaviour behaviour = ScrollBehaviour::ScrollAbsolute);

protected:
  bool event(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *event) override { QWidget::showEvent(event); }

public:
  OmniList();
  ~OmniList();

  void addDivider() { m_model.emplace_back(Divider{}); }

  void setSorting(const SortingConfig &config);
  bool selectUp();
  bool selectDown();
  bool selectLeft();
  bool selectRight();
  const AbstractVirtualItem *firstSelectableItem() const;
  void activateCurrentSelection() const;

  void updateModel(const std::function<void()> &updater,
                   OmniList::SelectionPolicy policy = OmniList::SelectionPolicy::SelectFirst) {
    beginResetModel();
    updater();
    endResetModel(policy);
  }

  void beginResetModel() {
    m_model.clear();
    m_model.reserve(0xF);
    _idItemMap.clear();
  }

  Section &addSection(const QString &title = "", const QString &subtitle = "") {
    auto section = std::make_unique<Section>(title, subtitle);
    auto ptr = section.get();

    m_model.emplace_back(std::move(section));

    return *ptr;
  }

  void endResetModel(OmniList::SelectionPolicy selectionPolicy) {
    std::ranges::for_each(m_model, [this](const auto &item) {
      if (auto section = std::get_if<std::unique_ptr<Section>>(&item)) {
        if (auto header = (*section)->headerItem()) { _idItemMap.insert({header->id(), header}); }

        auto items = (*section)->items() | std::views::filter([](auto &&item) {
                       return std::holds_alternative<std::unique_ptr<AbstractVirtualItem>>(item);
                     });

        std::ranges::for_each(items, [this](auto &&variant) {
          auto &item = std::get<std::unique_ptr<AbstractVirtualItem>>(variant);
          _idItemMap.insert({item->id(), item.get()});
        });
      }
    });

    calculateHeightsFromModel();

    emit modelChanged();

    switch (selectionPolicy) {
    case SelectFirst:
      selectFirst();
      break;
    case KeepSelection:
      qDebug() << "update with keep selection";
      if (_selected == -1) {
        selectFirst();
      } else if (auto idx = indexOfItem(_selectedId); idx != -1) {
        qCritical() << "not implemented yet";
        // qDebug() << "idx of " << _selectedId << _items[idx].vIndex;
        // setSelectedIndex(_items[idx].vIndex);
      } else {
        qDebug() << "no index for" << _selectedId;
        setSelectedIndex(std::max(0, std::min(_selected, static_cast<int>(_virtual_items.size() - 1))));
      }
      break;
    case PreserveSelection: {
      int targetIndex = std::clamp(_selected, 0, (int)_virtual_items.size() - 1);
      int distance = 0;

      for (;;) {
        int lowTarget = targetIndex - distance;
        int highTarget = targetIndex + distance;
        bool hasLower = lowTarget >= 0 && lowTarget < _virtual_items.size();
        bool hasUpper = highTarget >= 0 && highTarget < _virtual_items.size();

        if (!hasLower && !hasUpper) {
          setSelectedIndex(-1);
          return;
        }

        if (hasLower && _virtual_items[lowTarget].item->selectable()) {
          setSelectedIndex(lowTarget);
          return;
        }

        if (hasUpper && _virtual_items[highTarget].item->selectable()) {
          setSelectedIndex(highTarget);
          return;
        }

        ++distance;
      }
      break;
    }
    case SelectNone:
      setSelectedIndex(-1);
      break;
    }
  }

  void newAddItem() {}

  void updateFromList(std::vector<std::unique_ptr<AbstractVirtualItem>> &nextList,
                      SelectionPolicy selectionPolicy = SelectionPolicy::KeepSelection);
  void invalidateCache();
  void invalidateCache(const QString &id);

  void beginUpdate();
  void commitUpdate();
  void addItem(std::unique_ptr<AbstractVirtualItem> item);
  const AbstractVirtualItem *selected() const;

  const AbstractVirtualItem *setSelected(const QString &id,
                                         ScrollBehaviour scrollBehaviour = ScrollBehaviour::ScrollAbsolute);

  void clearFilter();
  void setFilter(std::unique_ptr<AbstractItemFilter> filter);
  const AbstractItemFilter *filter() const;
  const AbstractVirtualItem *itemAt(const QString &id) const;
  bool selectFirst();
  bool updateItem(const QString &id, const UpdateItemCallback &cb);
  bool removeItem(const QString &id);

  void setMargins(int left, int top, int right, int bottom);
  void setMargins(int value);
  void clear();
  void clearSelection() { setSelectedIndex(-1); }
  bool isShowingEmptyState() const;

signals:
  void modelChanged() const;
  void rowChanged(int n) const;
  void itemUpdated(const AbstractVirtualItem &item) const;
  void itemActivated(const AbstractVirtualItem &item) const;
  void selectionChanged(const AbstractVirtualItem *next, const AbstractVirtualItem *previous) const;
  void emptyStateChanged(bool empty);
  void virtualHeightChanged(int height) const;
};

class DefaultVirtualSection : public OmniList::VirtualSection {
  bool showHeader() override { return false; }
  QString generateId() const override { return "omnicast.default-section"; }

public:
  DefaultVirtualSection() : OmniList::VirtualSection("unamed") {}
};

class AbstractDefaultListItem : public OmniList::AbstractVirtualItem {
public:
  struct ItemData {
    OmniIconUrl iconUrl;
    QString name;
    QString category;
    AccessoryList accessories;
  };
  virtual ItemData data() const = 0;

  bool hasUniformHeight() const override { return true; }

  bool hasPartialUpdates() const override { return true; }

  void refresh(QWidget *w) const override {
    if (auto widget = dynamic_cast<DefaultListItemWidget *>(w)) {
      auto itemData = data();

      widget->setName(itemData.name);
      widget->setCategory(itemData.category);
      widget->setIconUrl(itemData.iconUrl);
      widget->setAccessories(itemData.accessories);
    } else {
      qDebug() << "not a defaut list itemw widget" << typeid(*w).name();
    }
    auto widget = static_cast<DefaultListItemWidget *>(w);
  }

  bool recyclable() const override { return true; }

  void recycle(QWidget *base) const override {
    auto widget = static_cast<DefaultListItemWidget *>(base);
    auto itemData = data();

    widget->setName(itemData.name);
    widget->setCategory(itemData.category);
    widget->setIconUrl(itemData.iconUrl);
    widget->setAccessories(itemData.accessories);
  }

  OmniListItemWidget *createWidget() const override {
    auto d = data();
    auto item = new DefaultListItemWidget(d.iconUrl, d.name, d.category, d.accessories);

    return item;
  }
};

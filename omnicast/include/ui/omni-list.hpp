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
#include <qelapsedtimer.h>
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
  static constexpr const double SCROLL_FRAME_TIME = 1000 / 120.0;
  QElapsedTimer m_scrollBarElapsedTimer;

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

    // Hook fired when the virtual item is attached to a widget. This can be a new widget
    // it created itself or an existing one that could be repurposed.
    virtual void attached(QWidget *) {}

    /**
     * Hook fired when the virtual item is detached from a widget. If the widget is recyclable, it could
     * be that it was repurposed to display another item as this one went out of sight.
     * This is where you should think about disconnecting any link you might have between your
     * virtual item and the concrete widget it's associated to.
     * Note that the widget can be destroyed right after this function is called.
     */
    virtual void detached(QWidget *) {}

    virtual OmniListItemWidget *createWidget() const = 0;
    virtual void recycle(QWidget *base) const;
    virtual bool recyclable() const;
    virtual bool hasPartialUpdates() const { return false; }
    virtual void refresh(QWidget *widget) const {}
    virtual QString generateId() const { return QUuid::createUuid().toString(); }

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

  public:
    ListRole role() const override { return ListRole::ListSection; }
    bool selectable() const override { return false; }
    bool hasUniformHeight() const override { return true; }
    QString generateId() const override;
    OmniListItemWidget *createWidget() const override;

    VirtualSection(const QString &name);
    ~VirtualSection() {}
  };

  void handleDebouncedScroll();

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
    using VirtualWidget = std::shared_ptr<AbstractVirtualItem>;
    using LayoutItem = std::variant<VirtualWidget, Spacer>;

  private:
    std::vector<LayoutItem> m_layoutItems;
    std::unique_ptr<AbstractVirtualItem> m_headerItem;
    QString m_title;
    QString m_subtitle;
    size_t m_itemCount = 0;
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

    size_t itemCount() { return m_itemCount; }

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
      m_layoutItems.reserve(n);
      return *this;
    }

    Section &withHeaderItem(std::unique_ptr<AbstractVirtualItem> item) {
      m_headerItem = std::move(item);
      return *this;
    }

    const QString &title() const { return m_title; }

    int spacing() const { return m_spacing; }

    AbstractVirtualItem *headerItem() { return m_headerItem.get(); }

    const std::vector<LayoutItem> &layoutItems() const { return m_layoutItems; }

    Section &addItem(std::shared_ptr<AbstractVirtualItem> item) {
      m_layoutItems.emplace_back(std::move(item));
      ++m_itemCount;
      return *this;
    }

    Section &addSpacing(int value) {
      m_layoutItems.emplace_back(Spacer(value));
      return *this;
    }

    Section &addDivider() {
      m_layoutItems.emplace_back(std::make_unique<DividerItem>());
      return *this;
    }

    Section &addItems(std::vector<std::unique_ptr<AbstractVirtualItem>> items) {
      m_layoutItems.insert(m_layoutItems.end(), std::make_move_iterator(items.begin()),
                           std::make_move_iterator(items.end()));
      m_itemCount += items.size();
      return *this;
    }

    Section &addItems(std::vector<std::shared_ptr<AbstractVirtualItem>> items) {
      m_layoutItems.insert(m_layoutItems.end(), std::make_move_iterator(items.begin()),
                           std::make_move_iterator(items.end()));
      m_itemCount += items.size();
      return *this;
    }
  };

  using RootListItem = std::variant<std::unique_ptr<Section>, std::unique_ptr<AbstractVirtualItem>>;

  using ModelItem = std::variant<std::unique_ptr<Section>, Divider>;

  std::vector<ModelItem> m_model;

private:
  Q_OBJECT

  const size_t DEFAULT_SELECTION_INDEX = -1;

  struct VisibleRange {
    int lower = -1;
    int upper = -1;
  };

  VisibleRange visibleIndexRange;
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
  struct VirtualWidgetInfo {
    QRect bounds;
    AbstractVirtualItem *item = nullptr;
    bool enumerable = false;
  };
  struct SectionCalculationContext {
    int index = 0;
    int x = 0;
    int maxHeight = 0;
  };
  struct CachedWidget {
    OmniListItemWidgetWrapper *widget = nullptr;
    size_t recyclingId = -1;
  };

  struct VisibleWidget {
    size_t idx = -1;
    OmniListItemWidgetWrapper *widget = nullptr;
  };

  QScrollBar *scrollBar = nullptr;
  std::vector<VirtualWidgetInfo> m_items;
  std::vector<OmniListItemWidgetWrapper *> m_visibleWidgets;
  std::map<size_t, OmniListItemWidgetWrapper *> _visibleWidgets;
  std::unordered_map<QString, CachedWidget> _widgetCache;
  std::unordered_map<size_t, std::stack<OmniListItemWidgetWrapper *>> _widgetPools;
  std::unordered_map<QString, AbstractVirtualItem *> _idItemMap;
  std::vector<std::pair<size_t, int>> m_cachedHeights;

  int _selected;
  QString _selectedId;
  int _virtualHeight;

  void itemClicked(int index);
  void itemDoubleClicked(int index) const;
  void rightClicked(int index) const;

  void updateVisibleItems();

  bool isDividableContent(const ModelItem &item) {
    if (auto section = std::get_if<std::unique_ptr<Section>>(&item)) {
      return (*section)->layoutItems().size() > 0;
    }

    return false;
  }

  bool isInViewport(const QRect &bounds) {
    int low = scrollBar->value();
    int high = low + height();

    return bounds.y() + bounds.height() >= low && bounds.y() <= high;
  }

  /**
   * Calculate the height for width of a widget, using the internal height cache,
   * if applicable.
   */
  int calculateItemHeight(const AbstractVirtualItem *item, int width) {
    if (!item->hasUniformHeight()) { return item->calculateHeight(width); }

    auto pred = [&](auto &&pair) { return pair.first == item->typeId(); };
    auto match = std::ranges::find_if(m_cachedHeights, pred);
    size_t typeId = item->typeId();

    if (match != m_cachedHeights.end()) { return match->second; }

    int height = item->calculateHeight(width);

    m_cachedHeights.push_back({typeId, height});

    return height;
  }

  void calculateHeights() {
    Timer timer;

    int yOffset = 0;
    int availableWidth = width() - margins.left - margins.right;
    std::optional<SectionCalculationContext> sctx;
    std::unordered_map<QString, CachedWidget> updatedCache;

    auto view = m_model | std::views::filter([](const auto &item) {
                  return std::holds_alternative<std::unique_ptr<Section>>(item);
                }) |
                std::views::transform([](const auto &section) {
                  return std::get<std::unique_ptr<Section>>(section)->itemCount();
                });
    auto totalSize = std::ranges::fold_left(view, 0, std::plus<size_t>());

    m_items.reserve(totalSize);
    m_items.clear();

    size_t i = -1;

    for (const auto &item : m_model) {
      ++i;
      if (auto divider = std::get_if<Divider>(&item)) {
        if (yOffset == 0) continue;
        if (i + 1 == m_model.size() || !isDividableContent(m_model.at(i + 1))) continue;

        int height = divider->item()->calculateHeight(availableWidth);
        QRect bounds(0, yOffset, width(), height);
        VirtualWidgetInfo vinfo{.bounds = bounds, .item = divider->item()};

        m_items.push_back(vinfo);
        yOffset += height;
      } else if (auto p = std::get_if<std::unique_ptr<Section>>(&item)) {
        auto &section = *p;

        auto &items = section->layoutItems();
        int spaceWidth = section->spacing() * (section->columns() - 1);
        int columnWidth = (availableWidth - spaceWidth) / section->columns();

        if (items.empty()) continue;

        sctx = {.x = margins.left, .maxHeight = 0};

        size_t shownCount = 0;

        for (auto &sectionItem : items) {
          if (auto spacer = std::get_if<Spacer>(&sectionItem)) {
            yOffset += spacer->value;
            continue;
          }

          if (auto p = std::get_if<Section::VirtualWidget>(&sectionItem)) {
            auto &item = *p;

            // Show section header above the first actually displayed item (after filtering has been
            // considered)
            if (shownCount == 0) {
              if (auto header = section->headerItem(); header && !section->title().isEmpty()) {
                int height = header->calculateHeight(availableWidth);
                QRect geometry(margins.left, yOffset, availableWidth, height);

                VirtualWidgetInfo vinfo{.bounds = geometry, .item = header};

                m_items.push_back(vinfo);
                yOffset += height;
              }
            }

            ++shownCount;

            int vIndex = m_items.size();
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

            QRect geometry(x, y, width, height);
            VirtualWidgetInfo vinfo{.bounds = geometry, .item = item.get(), .enumerable = true};

            // TODO: if in viewport, look for cached entry
            if (isInViewport(geometry)) {
              if (auto it = _widgetCache.find(item->id()); it != _widgetCache.end()) {
                QWidget *widget = it->second.widget->widget();

                item->attached(widget);
                item->refresh(it->second.widget->widget());
                updatedCache[item->id()] = it->second;
              }
            }

            ++shownCount;
            m_items.push_back(vinfo);
          }
        }

        if (sctx) { yOffset += sctx->maxHeight; }
      }
    }

    if (!m_items.empty()) { yOffset += margins.bottom + margins.top; }

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

    emit virtualHeightChanged(_virtualHeight);
  }

  int indexOfItem(const QString &id) const;

  void clearVisibleWidgets();

  OmniListItemWidgetWrapper *takeFromPool(size_t type);
  void moveToPool(size_t type, OmniListItemWidgetWrapper *wrapper);

  bool isSelectionValid() const { return _selected >= 0 && _selected < m_items.size(); }

  void setSelectedIndex(int index, ScrollBehaviour scrollBehaviour = ScrollRelative);
  int previousRowIndex(int index);
  int nextRowIndex(int index);

  void scrollTo(int idx, ScrollBehaviour behaviour = ScrollBehaviour::ScrollAbsolute);

  /**
   * Moving widgets around doesn't properly keep track of the hover state, so we need
   * to recalculate it manually here.
   */
  void recalculateMousePosition();
  void updateFocusChain();

protected:
  bool event(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *event) override {
    recalculateMousePosition();
    QWidget::showEvent(event);
  }

public:
  /**
   * The list of items that are currently in the viewport. This does _NOT_
   * include section headers or other layout items.
   */
  std::vector<const AbstractVirtualItem *> visibleItems() const {
    return m_items | std::views::drop(visibleIndexRange.lower) |
           std::views::take(std::max(0, visibleIndexRange.upper - visibleIndexRange.lower + 1)) |
           std::views::filter([](auto &&v) { return v.enumerable; }) |
           std::views::transform([](auto &&v) -> const AbstractVirtualItem * { return v.item; }) |
           std::ranges::to<std::vector>();
  }

  std::vector<const AbstractVirtualItem *> items() const {
    return m_items | std::views::filter([](auto &&v) { return v.enumerable; }) |
           std::views::transform([](auto &&v) -> const AbstractVirtualItem * { return v.item; }) |
           std::ranges::to<std::vector>();
  }

  std::vector<Section const *> sections() const {
    return m_model |
           std::views::filter([](auto &&v) { return std::holds_alternative<std::unique_ptr<Section>>(v); }) |
           std::views::transform(
               [](auto &v) -> Section const * { return std::get<std::unique_ptr<Section>>(v).get(); }) |
           std::ranges::to<std::vector>();
  }

  OmniList();
  ~OmniList();

  void mouseMoveEvent(QMouseEvent *event) override { QWidget::mouseMoveEvent(event); }

  void addDivider() { m_model.emplace_back(Divider{}); }

  int virtualHeight() const { return _virtualHeight; }

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

        auto items = (*section)->layoutItems() | std::views::filter([](auto &&item) {
                       return std::holds_alternative<std::shared_ptr<AbstractVirtualItem>>(item);
                     });

        std::ranges::for_each(items, [this](auto &&variant) {
          auto &item = std::get<std::shared_ptr<AbstractVirtualItem>>(variant);
          _idItemMap.insert({item->id(), item.get()});
        });
      }
    });

    calculateHeights();

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
        setSelectedIndex(std::max(0, std::min(_selected, static_cast<int>(m_items.size() - 1))));
      }
      break;
    case PreserveSelection: {
      int targetIndex = std::clamp(_selected, 0, (int)m_items.size() - 1);
      int distance = 0;

      for (;;) {
        int lowTarget = targetIndex - distance;
        int highTarget = targetIndex + distance;
        bool hasLower = lowTarget >= 0 && lowTarget < m_items.size();
        bool hasUpper = highTarget >= 0 && highTarget < m_items.size();

        if (!hasLower && !hasUpper) {
          setSelectedIndex(-1);
          return;
        }

        if (hasLower && m_items[lowTarget].item->selectable()) {
          setSelectedIndex(lowTarget);
          return;
        }

        if (hasUpper && m_items[highTarget].item->selectable()) {
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

  void invalidateCache();
  void invalidateCache(const QString &id);

  const AbstractVirtualItem *selected() const;
  const AbstractVirtualItem *setSelected(const QString &id,
                                         ScrollBehaviour scrollBehaviour = ScrollBehaviour::ScrollAbsolute);

  const AbstractVirtualItem *itemAt(const QString &id) const;
  bool selectFirst();
  bool updateItem(const QString &id, const UpdateItemCallback &cb);
  bool removeItem(const QString &id);

  void setMargins(int left, int top, int right, int bottom);
  void setMargins(int value);
  void clear();
  void clearSelection() { setSelectedIndex(-1); }
  void refresh() const;

signals:
  void modelChanged() const;
  void rowChanged(int n) const;
  void itemUpdated(const AbstractVirtualItem &item) const;
  void itemActivated(const AbstractVirtualItem &item) const;
  void selectionChanged(const AbstractVirtualItem *next, const AbstractVirtualItem *previous) const;
  void itemRightClicked(const AbstractVirtualItem &item) const;
  void virtualHeightChanged(int height) const;
};

class AbstractDefaultListItem : public OmniList::AbstractVirtualItem {
public:
  struct ItemData {
    OmniIconUrl iconUrl;
    QString name;
    QString category;
    AccessoryList accessories;
    QString alias;
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
      widget->setAlias(itemData.alias);
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
    widget->setAlias(itemData.alias);
  }

  OmniListItemWidget *createWidget() const override {
    auto d = data();
    auto item = new DefaultListItemWidget(d.iconUrl, d.name, d.category, d.accessories);

    return item;
  }
};

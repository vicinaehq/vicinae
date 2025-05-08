#pragma once
#include "omni-icon.hpp"
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
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qwidget.h>
#include <ranges>
#include <stack>
#include <unordered_map>

class OmniList : public QWidget {
public:
  enum ScrollBehaviour {
    ScrollRelative,
    ScrollAbsolute,
  };

  class AbstractVirtualItem {
  public:
    enum ListRole { ListItem, ListSection, ListDivider };

    virtual int calculateHeight(int width) const = 0;
    virtual OmniListItemWidget *createWidget() const = 0;
    virtual double rankingScore() const { return NAN; }
    virtual bool queryFilter(const QString &query) { return true; }
    virtual void recycle(QWidget *base) const;
    virtual bool recyclable() const;
    /**
     * Whether this item can have some of its content changed while
     * still keeping the same ID.
     *
     * This will call `refresh` on the cached widget instead of leaving
     * it fully untouched.
     *
     * This is usually unneeded for native items. This is mostly used
     * to handle items constructed by extensions.
     */
    virtual bool hasPartialUpdates() const { return false; }
    virtual void refresh(QWidget *widget) const {}
    virtual QString id() const = 0;
    virtual size_t typeId() const;
    virtual bool selectable() const;
    virtual ListRole role() const;

    bool isListItem() const;
    bool isSection() const;
    virtual ~AbstractVirtualItem() {}
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
    QString id() const override;

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

  class Section {
    std::vector<std::unique_ptr<AbstractVirtualItem>> m_items;
    std::unique_ptr<VirtualSection> m_headerItem;
    QString m_title;
    QString m_subtitle;

  public:
    Section(const QString &title = "", const QString &subtitle = "") : m_title(title), m_subtitle(subtitle) {
      m_headerItem = std::make_unique<VirtualSection>(title);
    }

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

    Section &withHeaderItem(std::unique_ptr<VirtualSection> item) {
      m_headerItem = std::move(item);
      return *this;
    }

    VirtualSection *headerItem() { return m_headerItem.get(); }

    const std::vector<std::unique_ptr<AbstractVirtualItem>> &items() const { return m_items; }

    Section &addItem(std::unique_ptr<AbstractVirtualItem> item) {
      m_items.emplace_back(std::move(item));
      return *this;
    }

    Section &addItems(std::vector<std::unique_ptr<AbstractVirtualItem>> items) {
      m_items.insert(m_items.end(), std::make_move_iterator(items.begin()),
                     std::make_move_iterator(items.end()));
      return *this;
    }
  };

  using RootListItem = std::variant<std::unique_ptr<Section>, std::unique_ptr<AbstractVirtualItem>>;

  std::vector<std::unique_ptr<Section>> m_model;

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

  int _selected;
  QString _selectedId;
  bool _isUpdating;
  int _virtualHeight;

  void itemClicked(int index);
  void itemDoubleClicked(int index) const;

  void updateVisibleItems();
  void calculateHeights();

  void calculateHeightsFromModel() {
    _virtual_items.clear();

    int yOffset = 0;
    int availableWidth = width() - margins.left - margins.right;
    std::optional<SectionCalculationContext> sctx;
    std::unordered_map<QString, CachedWidget> updatedCache;

    auto view = m_model | std::views::transform([](const auto &section) { return section->items().size(); });
    auto totalSize = std::ranges::fold_left(view, 0, std::plus<size_t>());

    _virtual_items.reserve(totalSize);

    for (const auto &section : m_model) {
      auto &items = section->items();

      if (items.empty()) continue;

      if (auto header = section->headerItem()) {
        header->setCount(items.size());
        header->initWidth(availableWidth);

        if (header->showHeader()) {
          VirtualListWidgetInfo vinfo{.x = margins.left,
                                      .y = yOffset,
                                      .width = availableWidth,
                                      .height = header->calculateHeight(availableWidth),
                                      .item = header};

          _virtual_items.push_back(vinfo);
          yOffset += vinfo.height;
        }

        sctx = {.section = header, .x = margins.left};
      }

      for (auto &item : items) {
        int vIndex = _virtual_items.size();
        int height = 0;
        int width = availableWidth;
        int x = margins.left;
        int y = yOffset;

        if (vIndex >= visibleIndexRange.lower && vIndex <= visibleIndexRange.upper) {
          if (auto it = _widgetCache.find(item->id()); it != _widgetCache.end()) {
            if (item->hasPartialUpdates()) {
              qDebug() << "Refreshing" << it->first;
              item->refresh(it->second.widget->widget());
            }

            updatedCache[item->id()] = it->second;
          }
        }

        if (sctx) {
          width = sctx->section->calculateItemWidth(width, sctx->index);
          // x = sctx->section->calculateItemX(sctx->x, sctx->index);
          if (sctx->x == margins.left && sctx->index > 0) {
            yOffset += sctx->section->spacing();
            y = yOffset;
          }

          x = sctx->x;
          sctx->x = sctx->x + width + sctx->section->spacing();
          height = item->calculateHeight(width);
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

        _virtual_items.push_back(vinfo);
      }
    }

    if (sctx) { yOffset += sctx->maxHeight; }

    yOffset += margins.bottom;

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

    auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //  qDebug() << "calculateHeights() took" << duration << "ms";

    scrollBar->setMaximum(std::max(0, yOffset - height()));
    scrollBar->setMinimum(0);
    _virtualHeight = yOffset;
    updateVisibleItems();
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

public:
  OmniList();
  ~OmniList();

  void setSorting(const SortingConfig &config);
  bool selectUp();
  bool selectDown();
  bool selectLeft();
  bool selectRight();
  const AbstractVirtualItem *firstSelectableItem() const;
  void activateCurrentSelection() const;

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
    std::ranges::for_each(m_model, [this](const auto &section) {
      if (auto header = section->headerItem()) { _idItemMap.insert({header->id(), header}); }

      std::ranges::for_each(section->items(),
                            [this](const auto &item) { _idItemMap.insert({item->id(), item.get()}); });
    });

    calculateHeightsFromModel();

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
  void itemUpdated(const AbstractVirtualItem &item) const;
  void itemActivated(const AbstractVirtualItem &item) const;
  void selectionChanged(const AbstractVirtualItem *next, const AbstractVirtualItem *previous) const;
  void emptyStateChanged(bool empty);
};

class DefaultVirtualSection : public OmniList::VirtualSection {
  bool showHeader() override { return false; }
  QString id() const override { return "omnicast.default-section"; }

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

  int calculateHeight(int width) const override {
    static DefaultListItemWidget ruler(OmniIconUrl(""), "", "", {});

    return ruler.sizeHint().height();
  }

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

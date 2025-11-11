#pragma once
#include "common.hpp"
#include "../image/url.hpp"
#include "theme/colors.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/omni-list/omni-list-item-widget-wrapper.hpp"
#include "ui/default-list-item-widget/default-list-item-widget.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
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
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <ranges>
#include <stack>
#include <unordered_map>
#include <variant>

class OmniList : public QWidget {
  static constexpr const double SCROLL_FRAME_TIME = 1000 / 120.0;
  QTimer *m_scrollTimer = new QTimer;

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

    virtual size_t recyclingId() const { return -1; }

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

  struct VisibleRangeV2 {
    static VisibleRangeV2 empty() { return {0, 0}; }
    size_t start = 0;
    size_t size = 0;
  };

  VisibleRangeV2 visibleIndexRange;

  struct {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
  } margins;
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

  QScrollBar *scrollBar = new OmniScrollBar(this);
  std::vector<VirtualWidgetInfo> m_items;
  std::vector<OmniListItemWidgetWrapper *> m_visibleWidgets;
  std::map<size_t, OmniListItemWidgetWrapper *> _visibleWidgets;
  std::unordered_map<QString, CachedWidget> _widgetCache;
  std::unordered_map<size_t, std::stack<OmniListItemWidgetWrapper *>> _widgetPools;
  std::vector<std::pair<size_t, int>> m_cachedHeights;

  int m_selected = DEFAULT_SELECTION_INDEX;
  QString m_selectedId;
  int m_virtualHeight;

  void itemClicked(int index);
  void itemDoubleClicked(int index) const;
  void rightClicked(int index) const;

  void updateVisibleItems();
  bool isDividableContent(const ModelItem &item);
  bool isInViewport(const QRect &bounds);

  /**
   * Calculate the height for width of a widget, using the internal height cache,
   * if applicable.
   */
  int calculateItemHeight(const AbstractVirtualItem *item, int width);
  void calculateHeights();

  int indexOfItem(const QString &id) const;

  void clearVisibleWidgets();

  OmniListItemWidgetWrapper *takeFromPool(size_t type);
  void moveToPool(size_t type, OmniListItemWidgetWrapper *wrapper);

  bool isSelectionValid() const { return m_selected >= 0 && m_selected < m_items.size(); }

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

  void setSelected(SelectionPolicy policy);

  void clearWidgetPools();

public:
  /**
   * The list of items that are currently in the viewport. This does _NOT_
   * include section headers or other layout items.
   */
  std::vector<const AbstractVirtualItem *> visibleItems() const;
  std::vector<const AbstractVirtualItem *> items() const;

  /*
  std::vector<Section const *> sections() const {
    return m_model |
           std::views::filter([](auto &&v) { return std::holds_alternative<std::unique_ptr<Section>>(v); }) |
           std::views::transform(
               [](auto &v) -> Section const * { return std::get<std::unique_ptr<Section>>(v).get(); }) |
           std::ranges::to<std::vector>();
  }
  */

  OmniList();
  ~OmniList();

  void mouseMoveEvent(QMouseEvent *event) override { QWidget::mouseMoveEvent(event); }

  void addDivider() { m_model.emplace_back(Divider{}); }

  int virtualHeight() const { return m_virtualHeight; }

  /**
   * Select next selectable item.
   * If at the end of the list, loops back to the start.
   */
  void selectNext();

  bool selectUp();
  bool selectDown();
  bool selectLeft();
  bool selectRight();
  bool selectHome();
  bool selectEnd();
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
  }

  Section &addSection(const QString &title = "", const QString &subtitle = "") {
    auto section = std::make_unique<Section>(title, subtitle);
    auto ptr = section.get();

    m_model.emplace_back(std::move(section));

    return *ptr;
  }

  void endResetModel(OmniList::SelectionPolicy selectionPolicy);

  void invalidateCache();
  void invalidateCache(const QString &id);

  const AbstractVirtualItem *selected() const;
  const AbstractVirtualItem *setSelected(const QString &id,
                                         ScrollBehaviour scrollBehaviour = ScrollBehaviour::ScrollAbsolute);

  const AbstractVirtualItem *itemAt(const QString &id) const;
  AbstractVirtualItem *itemAt(const QString &id);
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
  // Text ellision is done differently depending on the subtitle type
  using ItemDataSubtitle = std::variant<QString, std::filesystem::path>;

  struct ItemData {
    std::optional<ImageURL> iconUrl;
    QString name;
    ItemDataSubtitle subtitle;
    AccessoryList accessories;
    QString alias;
    bool active = false;
    bool secondaryBackground = false;
  };
  virtual ItemData data() const = 0;

  int calculateHeight(int width) const override { return 41; }

  bool hasUniformHeight() const override { return true; }
  bool hasPartialUpdates() const override { return true; }

  void refresh(QWidget *w) const override {
    auto widget = static_cast<DefaultListItemWidget *>(w);
    auto itemData = data();

    if (itemData.secondaryBackground) {
      widget->setSelectedColor(SemanticColor::ListItemSecondarySelectionBackground);
      widget->setHoverColor(SemanticColor::ListItemSecondaryHoverBackground);
    }

    widget->setName(itemData.name);
    widget->setSubtitle(itemData.subtitle);
    widget->setIconUrl(itemData.iconUrl);
    widget->setAccessories(itemData.accessories);
    widget->setAlias(itemData.alias);
    widget->setActive(itemData.active);
  }

  OmniListItemWidget *createWidget() const override {
    auto item = new DefaultListItemWidget;

    refresh(item);

    return item;
  }
};

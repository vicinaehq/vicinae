#pragma once
#include "omni-icon.hpp"
#include "ui/omni-list-item-widget-wrapper.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/default-list-item-widget.hpp"
#include <cstdio>
#include <QPainterPath>
#include <functional>
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qtpreprocessorsupport.h>
#include <qwidget.h>
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
    virtual void recycle(QWidget *base) const;
    virtual bool recyclable() const;
    virtual QString id() const = 0;
    virtual size_t typeId() const;
    virtual bool selectable() const;
    virtual ListRole role() const;

    bool isListItem() const;
    bool isSection() const;
  };

  class VirtualSection : public AbstractVirtualItem {
    QString _name;
    int _count;

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

    VirtualSection(const QString &name);
  };

  class FixedCountSection : public VirtualSection {
    int _count = 0;

    int count() const override { return _count; }

  public:
    FixedCountSection(const QString &name, int count) : VirtualSection(name), _count(count) {}
  };

  class AbstractItemFilter {
  public:
    virtual bool matches(const AbstractVirtualItem &item) = 0;
  };

  using UpdateItemCallback = std::function<void(AbstractVirtualItem *)>;
  enum SelectionPolicy {
    SelectFirst,
    KeepSelection,
    SelectNone,
  };

private:
  Q_OBJECT

  const size_t DEFAULT_SELECTION_INDEX = -1;

  struct {
    int lower;
    int upper;
  } visibleIndexRange;
  struct {
    int left;
    int top;
    int right;
    int bottom;
  } margins;
  struct ListItemInfo {
    std::unique_ptr<AbstractVirtualItem> item;
    bool filtered;
    int vIndex;
    int cachedHeight;
    QString id;
  };
  struct VirtualListWidgetInfo {
    int x;
    int y;
    int width;
    int height;
    int index;
  };
  struct SectionCalculationContext {
    VirtualSection *section;
    int index;
    int x;
    int maxHeight;
  };
  struct CachedWidget {
    OmniListItemWidgetWrapper *widget;
    size_t recyclingId;
  };
  struct NavigationBehaviour {};

  QScrollBar *scrollBar;
  std::vector<ListItemInfo> _items;
  std::vector<VirtualListWidgetInfo> _virtual_items;
  std::unordered_map<size_t, OmniListItemWidgetWrapper *> _visibleWidgets;
  std::unordered_map<QString, CachedWidget> _widgetCache;
  std::unordered_map<size_t, std::stack<OmniListItemWidgetWrapper *>> _widgetPools;
  std::unordered_map<QString, size_t> _idMap;
  std::unique_ptr<AbstractItemFilter> _filter;
  int _selected;
  QString _selectedId;
  bool _isUpdating;
  int _virtualHeight;

  void itemClicked(int index);
  void itemDoubleClicked(int index) const;

  void updateVisibleItems();
  void calculateHeights();

  int indexOfItem(const QString &id) const;

  void clearVisibleWidgets();

  bool selectUp();
  bool selectDown();
  bool selectLeft();
  bool selectRight();

  OmniListItemWidgetWrapper *takeFromPool(size_t type);
  void moveToPool(size_t type, OmniListItemWidgetWrapper *wrapper);

  bool isSelectionValid() const { return _selected >= 0 && _selected < _items.size(); }

  void activateCurrentSelection() const;
  void setSelectedIndex(int index, ScrollBehaviour scrollBehaviour = ScrollRelative);
  int previousRowIndex(int index);
  int nextRowIndex(int index);

  void scrollTo(int idx, ScrollBehaviour behaviour = ScrollBehaviour::ScrollAbsolute);
  ListItemInfo &vmap(int vindex);
  const ListItemInfo &vmap(int vindex) const;

protected:
  bool event(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  OmniList();
  ~OmniList() { qDebug() << "destroy omnilist"; }

  void updateFromList(std::vector<std::unique_ptr<AbstractVirtualItem>> &nextList,
                      SelectionPolicy selectionPolicy = SelectionPolicy::KeepSelection);
  void addSection(const QString &name);
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
  void clearSelection() {
    _selected = -1;
    _selectedId.clear();
  }

signals:
  void itemUpdated(const AbstractVirtualItem &item) const;
  void itemActivated(const AbstractVirtualItem &item) const;
  void selectionChanged(const AbstractVirtualItem *next, const AbstractVirtualItem *previous) const;
};

class AbstractDefaultListItem : public OmniList::AbstractVirtualItem {
public:
  struct ItemData {
    OmniIconUrl iconUrl;
    QString name;
    QString category;
    QString kind;
    QColor _iconColor;
  };
  virtual ItemData data() const = 0;

  int calculateHeight(int width) const override {
    static DefaultListItemWidget ruler(OmniIconUrl(""), "", "", "");

    return ruler.sizeHint().height();
  }

  bool recyclable() const override { return true; }

  void recycle(QWidget *base) const override {
    auto widget = static_cast<DefaultListItemWidget *>(base);
    auto itemData = data();

    widget->setName(itemData.name);
    widget->setCategory(itemData.category);
    widget->setKind(itemData.kind);
    widget->setIconUrl(itemData.iconUrl);
  }

  OmniListItemWidget *createWidget() const override {
    auto d = data();
    auto item = new DefaultListItemWidget(d.iconUrl, d.name, d.category, d.kind);

    return item;
  }
};

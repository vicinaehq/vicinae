#pragma once
#include "ui/list-section-header.hpp"
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
    enum HeightCalculationPolicy { AlwaysRecalculate, Uniform };

    virtual int calculateHeight() const = 0;
    virtual OmniListItemWidget *createWidget() const = 0;
    virtual void recycle(QWidget *base) const {}
    virtual bool recyclable() const { return false; }
    virtual QString id() const = 0;
    virtual size_t typeId() const { return typeid(*this).hash_code(); }
    virtual bool selectable() const { return true; }
    virtual ListRole role() const { return ListRole::ListItem; }
    virtual HeightCalculationPolicy heightCalculationPolicy() {
      return HeightCalculationPolicy::AlwaysRecalculate;
    }

    bool isListItem() const { return role() == ListRole::ListItem; }
    bool isSection() const { return role() == ListRole::ListSection; }
  };

  class VirtualSection : public AbstractVirtualItem {
    QString _name;
    int _count;

  public:
    ListRole role() const override { return ListRole::ListSection; }
    bool selectable() const override { return false; }

    const QString &name() const { return _name; }
    QString id() const override { return _name; }

    virtual bool showHeader() { return count() > 0 && !_name.isEmpty(); }

    virtual int computeWidth(int width, int index) const { return width; }
    virtual void initWidth(int width) {}

    void setCount(int count) { _count = count; }
    int count() const { return _count; }

    OmniListItemWidget *createWidget() const override {
      return new OmniListSectionHeader(_name, "", count());
    }

    int calculateHeight() const override {
      static OmniListSectionHeader ruler("", "", 0);

      return ruler.sizeHint().height();
    }

    VirtualSection(const QString &name) : _name(name), _count(0) {}
  };

  class ItemFilter {
  public:
    virtual bool matches(const AbstractVirtualItem &item) { return true; }
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

  QScrollBar *scrollBar;
  std::vector<ListItemInfo> _items;
  std::vector<VirtualListWidgetInfo> _virtual_items;
  std::unordered_map<size_t, OmniListItemWidgetWrapper *> _visibleWidgets;
  std::unordered_map<QString, OmniListItemWidgetWrapper *> _widgetCache;
  std::unordered_map<size_t, std::stack<OmniListItemWidgetWrapper *>> _widgetPools;
  std::unordered_map<QString, size_t> _idMap;
  std::unique_ptr<ItemFilter> _filter;
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

  void selectUp();
  void selectDown();

  OmniListItemWidgetWrapper *takeFromPool(size_t type);
  void moveToPool(size_t type, OmniListItemWidgetWrapper *wrapper);

  bool isSelectionValid() const { return _selected >= 0 && _selected < _items.size(); }

  void activateCurrentSelection() const;
  void setSelectedIndex(int index, ScrollBehaviour scrollBehaviour = ScrollRelative);
  int previousRowIndex(int index);

  void scrollTo(int idx, ScrollBehaviour behaviour = ScrollBehaviour::ScrollAbsolute);
  ListItemInfo &vmap(int vindex);
  const ListItemInfo &vmap(int vindex) const;

protected:
  bool event(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  OmniList();

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
  void setFilter(std::unique_ptr<ItemFilter> filter);
  const ItemFilter *filter() const;
  const AbstractVirtualItem *itemAt(const QString &id) const;
  bool selectFirst();
  bool updateItem(const QString &id, const UpdateItemCallback &cb);
  bool removeItem(const QString &id);

  void setMargins(int left, int top, int right, int bottom);
  void setMargins(int value);
  void clear();

signals:
  void itemUpdated(const AbstractVirtualItem &item) const;
  void itemActivated(const AbstractVirtualItem &item) const;
  void selectionChanged(const AbstractVirtualItem *next, const AbstractVirtualItem *previous) const;
};

class AbstractDefaultListItem : public OmniList::AbstractVirtualItem {
public:
  struct ItemData {
    QString icon;
    QString name;
    QString category;
    QString kind;
  };
  virtual ItemData data() const = 0;

  int calculateHeight() const override {
    static DefaultListItemWidget ruler("", "", "", "");

    return ruler.sizeHint().height();
  }

  HeightCalculationPolicy heightCalculationPolicy() override { return HeightCalculationPolicy::Uniform; }

  bool recyclable() const override { return true; }

  void recycle(QWidget *base) const override {
    auto widget = static_cast<DefaultListItemWidget *>(base);
    auto itemData = data();

    widget->setName(itemData.name);
    widget->setCategory(itemData.category);
    widget->setKind(itemData.kind);
    widget->setIcon(itemData.icon);
  }

  OmniListItemWidget *createWidget() const override {
    auto d = data();
    auto item = new DefaultListItemWidget(d.icon, d.name, d.category, d.kind);

    return item;
  }
};

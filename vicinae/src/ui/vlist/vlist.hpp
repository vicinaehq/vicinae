#pragma once
#include <optional>
#include <qevent.h>
#include <qobject.h>
#include <qscrollbar.h>
#include <qwidget.h>
#include "ui/omni-list/omni-list-item-widget.hpp"

namespace vicinae::ui {

class VListModel : public QObject {
  Q_OBJECT

signals:
  /**
   * Signals to the list that the data changed and that the range of valid indexes
   * may have changed.
   */
  void dataChanged() const;

public:
  using StableID = size_t;
  using WidgetTag = size_t;
  using Index = int;
  using WidgetType = OmniListItemWidget;
  static constexpr const Index InvalidIndex = -1;
  static constexpr const WidgetTag InvalidTag = -1;

  VListModel(QObject *parent = nullptr) : QObject(parent) {}

  /**
   * The number of items this model currently has.
   * If count() returns `n` that implies a valid index range of`[0;n-1].
   */
  virtual int count() const = 0;

  virtual bool isEmpty() const { return height() == 0; }

  /**
   * The height that should be allocated for a widget if the item at this index
   * needs to be shown on screen.
   */
  virtual int height(Index idx) const = 0;

  virtual QSize allocateSize(VListModel::Index idx, QSize viewport) const {
    return QSize(viewport.width(), height(idx));
  }

  /**
   * Spacing between every element, horizontal and vertical.
   */
  virtual int spacing() const { return 0; }

  /**
   * Find what model item is located at a given height.
   * This is used to determine the first item in the viewport depending on the current scroll height.
   * Models that have items with mostly uniform heights can make this very fast and speed up viewport
   * updates.
   */
  virtual Index indexAtHeight(int height) const = 0;

  virtual int heightAtIndex(Index idx) const = 0;

  virtual int xAtIndex(Index index) const = 0;

  /**
   * Whether the item at `idx` can be selectable or not. This is used to implement things such as
   * section headers, which are part of the model and are associated with a specific widget, but should not
   * be considered during navigation.
   */
  virtual bool isSelectable(Index idx) const = 0;

  /**
   * Whether this item acts as an 'anchor'.
   * When an item gets selected, if the item above it is an anchor the list will scroll to the anchor instead
   * of scrolling to the item itself. This is used to implement e.g section headers where we always want to
   * make sure the header is visible.
   */
  virtual bool isAnchor(Index idx) const { return false; }

  /**
   * The full height of the list, computed on a full model change.
   */
  virtual int height() const = 0;

  /**
   * A stable ID that can be used to identify the item that is currently at index `idx`.
   * The returned ID should not be tied to `idx`.
   * This is used internally to reuse widgets across model changes.
   */
  virtual StableID stableId(Index idx) const = 0;

  virtual WidgetTag widgetTag(Index idx) const = 0;

  /**
   * Create a widget that represents the item at index `idx`.
   * This method should only take care of the initialization work and return
   * the widget as soon as possible.
   *
   * `refreshWidget` is called right after a new widget is created in order to apply
   * the data at the current index to it.
   */
  virtual WidgetType *createWidget(Index idx) const = 0;

  /**
   * Apply the data at `idx` to `widget`. The widget is one that has been created by `createWidget` for this
   * same index or another index that is represented using the same widget type. This is used to implement
   * proper widget recycling.
   * Widget types are compared using `typeid`.
   */
  virtual void refreshWidget(Index idx, WidgetType *widget) const = 0;

  virtual void viewportChanged(QSize size) {}

  virtual void onDataChanged() {}

protected:
  /**
   * Simple utility to generate a random ID, generally used to generate
   * widget tags or not so stable IDs.
   */
  size_t randomId() const { return m_serial++; }

  template <typename T> static size_t hash(const T &t) { return std::hash<T>()(t); }

private:
  mutable size_t m_serial = 0;
};

class WidgetWrapper : public QWidget {
  Q_OBJECT

signals:
  void clicked() const;
  void doubleClicked() const;

public:
  WidgetWrapper(QWidget *parent = nullptr) : QWidget(parent) { setAttribute(Qt::WA_Hover); }

  void setSelected(bool selected) { m_widget->selectionChanged(selected); }

  VListModel::Index index() const { return m_index; }
  VListModel::WidgetType *widget() const { return m_widget; }
  void setWidget(VListModel::WidgetType *w) {
    m_widget = w;
    m_widget->setParent(this);
    m_widget->setMouseTracking(true);
    m_widget->show();
    connect(m_widget, &VListModel::WidgetType::clicked, this, &WidgetWrapper::clicked);
    connect(m_widget, &VListModel::WidgetType::doubleClicked, this, &WidgetWrapper::doubleClicked);
  }
  void setIndex(VListModel::Index idx) { m_index = idx; }

protected:
  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    if (m_widget) m_widget->setFixedSize(event->size());
  }

private:
  VListModel::Index m_index = -1;
  VListModel::WidgetType *m_widget = nullptr;
};

enum class ScrollAnchor : std::uint8_t { Top, Bottom, Relative };

class VListWidget : public QWidget {
  Q_OBJECT

signals:
  /**
   * The currently selected item changed.
   */
  void itemSelected(std::optional<VListModel::Index> idx) const;

  /*
   * Item double-clicked or manually activated.
   */
  void itemActivated(VListModel::Index idx) const;

public:
  struct ViewportItem {
    QRect bounds;
    VListModel::StableID id;
    VListModel::Index index = -1;
    VListModel::WidgetTag tag = -1;
    WidgetWrapper *widget = nullptr;
  };

  VListWidget();

  void setModel(VListModel *model);
  void setMargins(const QMargins &margins);
  void setSelected(std::optional<VListModel::Index> idx);

  std::span<const ViewportItem> visibleItems() const;

  /**
   * Force widget at index to be refreshed.
   * If `idx` is not in the viewport then this is a no-op.
   * Returns whether a widget was refreshed or not.
   */
  bool refresh(VListModel::Index idx) const;

  void refreshAll();

  /**
   * Get the widget at the given index, if any.
   * This will only return a widget if the item at this index is currently in the viewport.
   * If it's not, then there is no visual representation for it, so nullptr is returned.
   * It is possible to manually make changes to this widget and then call `refresh`,
   */
  VListModel::WidgetType *widgetAt(VListModel::Index idx) const;

  void selectFirst();
  void selectLast();

  /**
   * Select next item. If we are the end of the list, go to the first.
   */
  void selectNext();

  bool activateCurrentSelection();

  /**
   * Select the item above the current selection.
   * `true` is selected if such an item exists and was selected, `false` otherwise.
   */
  bool selectUp();

  /**
   * Select the item below the current selection.
   * `true` is selected if such an item exists and was selected, `false` otherwise.
   */
  bool selectDown();

  bool selectLeft();

  bool selectRight();

  void pageDown();
  void pageUp();

  void setMargins(int n);

  std::optional<VListModel::Index> currentSelection() const;

protected:
  /**
   * Makes sure widgets that may have a specific appearance due to them having been hovered
   * are updated if this is no longer the case.
   */
  void recalculateMousePosition();

  void updateFocusChain();

  std::optional<VListModel::Index> getTopItem(VListModel::Index idx) const;

  void scrollToHeight(int height) { m_scrollBar->setValue(height); }
  void scrollToIndex(VListModel::Index idx, ScrollAnchor anchor = ScrollAnchor::Relative);

  void setupUI();
  void calculate();
  bool event(QEvent *event) override;
  void handleScrollChanged(int value) { updateViewport(); }
  void resizeEvent(QResizeEvent *event) override;

  std::optional<VListModel::Index> firstSelectableIndex() const;
  std::optional<VListModel::Index> lastSelectableIndex() const;

private:
  struct WidgetData {
    WidgetWrapper *widget = nullptr;
    VListModel::WidgetTag tag = 0;
  };

  struct Selection {
    VListModel::Index idx;
    VListModel::StableID id;
  };

  WidgetWrapper *getFromPool(VListModel::WidgetTag tag);
  void updateViewport();

  std::unordered_map<VListModel::StableID, WidgetData> m_widgetMap;
  std::unordered_map<VListModel::WidgetTag, std::vector<WidgetWrapper *>> m_recyclingPool;
  std::vector<ViewportItem> m_visibleItems;
  std::optional<Selection> m_selected;

  int m_height = 0;
  QMargins m_margins = DEFAULT_MARGINS;
  QScrollBar *m_scrollBar = nullptr;
  VListModel *m_model = nullptr;
  int m_count = 0;
  QTimer m_scrollTimer;

  static constexpr const int DEFAULT_PAGE_STEP = 40;
  static QMargins constexpr const DEFAULT_MARGINS = {5, 5, 5, 5};
};
}; // namespace vicinae::ui

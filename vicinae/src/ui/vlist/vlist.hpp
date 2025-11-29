#pragma once
#include "timer.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <numbers>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
#include <qtsqlglobal.h>
#include <qwidget.h>

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
  using Index = size_t;
  using WidgetType = OmniListItemWidget;
  static constexpr const Index InvalidIndex = -1;
  static constexpr const WidgetTag InvalidTag = -1;

  VListModel(QObject *parent = nullptr) : QObject(parent) {}

  /**
   * The number of items this model currently has.
   * If count() returns `n` that implies a valid index range of`[0;n-1].
   */
  virtual size_t count() const = 0;

  virtual bool isEmpty() const { return height() == 0; }

  /**
   * The height that should be allocated for a widget if the item at this index
   * needs to be shown on screen.
   */
  virtual size_t height(Index idx) const = 0;

  /**
   * Find what model item is located at a given height.
   * This is used to determine the first item in the viewport depending on the current scroll height.
   * Models that have items with mostly uniform heights can make this very fast and speed up viewport
   * updates.
   */
  virtual Index indexAtHeight(int height) const = 0;

  virtual size_t heightAtIndex(Index idx) const = 0;

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
  virtual size_t height() const = 0;

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

protected:
  /**
   * Simple utility to generate a random ID, generally used to generate
   * widget tags or not so stable IDs.
   */
  size_t randomId() const {
    static size_t random = 0;
    return random++;
  }

  template <typename T> static size_t hash(const T &t) { return std::hash<T>()(t); }
};

class WidgetWrapper : public QWidget {
  Q_OBJECT

signals:
  void clicked() const;
  void doubleClicked() const;

public:
  WidgetWrapper() {}

  VListModel::Index index() const { return m_index; }
  VListModel::WidgetType *widget() const { return m_widget; }
  void setWidget(VListModel::WidgetType *w) {
    m_widget = w;
    m_widget->setParent(this);
    m_widget->show();
    connect(m_widget, &VListModel::WidgetType::clicked, this, &WidgetWrapper::clicked);
    connect(m_widget, &VListModel::WidgetType::doubleClicked, this, &WidgetWrapper::doubleClicked);
  }
  void setIndex(VListModel::Index idx) { m_index = idx; }

protected:
  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    m_widget->setFixedSize(event->size());
  }

private:
  VListModel::Index m_index = -1;
  VListModel::WidgetType *m_widget = nullptr;
};

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

private:
public:
  VListWidget();

  void setModel(VListModel *model);
  void calculate();
  void setMargins(const QMargins &margins);
  void setSelected(VListModel::Index idx);

  void scrollToTop() { scrollToHeight(0); }

  void scrollToBottom() { scrollToHeight(m_scrollBar->maximum()); }

  void scrollToHeight(int height) { m_scrollBar->setValue(height); }
  void scrollToIndex(VListModel::Index idx);

  void selectFirst();

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

  void setMargins(int n);

  std::optional<VListModel::Index> currentSelection() const {
    return m_selected.transform([](auto &&s) { return s.idx; });
  };

protected:
  void setupUI();
  bool event(QEvent *event) override;
  void handleScrollChanged(int value) { updateViewport(); }
  void resizeEvent(QResizeEvent *event) override;

private:
  struct WidgetData {
    WidgetWrapper *widget = nullptr;
    VListModel::WidgetTag tag = 0;
  };

  struct Selection {
    VListModel::Index idx;
    VListModel::StableID id;
  };

  struct ViewportItem {
    QRect bounds;
    VListModel::StableID id;
    VListModel::Index index = -1;
    VListModel::WidgetTag tag = -1;
    WidgetWrapper *widget = nullptr;
  };

  WidgetWrapper *getFromPool(VListModel::WidgetTag tag);
  void updateViewport();

  std::unordered_map<VListModel::StableID, WidgetData> m_widgetMap;
  std::unordered_map<VListModel::WidgetTag, std::vector<WidgetWrapper *>> m_recyclingPool;
  std::vector<ViewportItem> m_visibleItems;
  std::optional<Selection> m_selected;

  int m_height = 0;
  QMargins m_margins;
  QScrollBar *m_scrollBar = nullptr;
  VListModel *m_model = nullptr;
  size_t m_count = 0;

  static constexpr const size_t DEFAULT_PAGE_STEP = 40;
};
}; // namespace vicinae::ui

#include "timer.hpp"
#include "ui/omni-list/omni-list-item-widget.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
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

  /**
   * The number of items this model currently has.
   * If count() returns `n` that implies a valid index range of`[0;n-1].
   */
  virtual size_t count() const = 0;

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

private:
public:
  VListWidget() {
    m_visibleItems.reserve(32);
    setupUI();
  }

  void setModel(VListModel *model) {
    connect(model, &VListModel::dataChanged, this, &VListWidget::calculate);
    m_model = model;
    calculate();
  }

  void calculate() {
    m_scrollBar->setMinimum(0);
    m_scrollBar->setMaximum(std::max(0, (int)m_model->height() - (int)size().height()));
    m_scrollBar->setPageStep(DEFAULT_PAGE_STEP);
    m_count = m_model->count();
    if (m_scrollBar->value() != 0) {
      m_scrollBar->setValue(0);
    } else {
      updateViewport();
    }
  }

  void setMargins(const QMargins &margins) {
    m_margins = margins;
    updateViewport();
  }

  void setSelected(VListModel::Index idx) {
    bool isInViewport =
        !m_visibleItems.empty() && idx > m_visibleItems.front().index && idx < m_visibleItems.back().index;

    if (!isInViewport) { scrollToIndex(idx); }

    m_selected = idx;
    updateViewport();
  }

  void scrollToTop() { scrollToHeight(0); }

  void scrollToBottom() { scrollToHeight(m_scrollBar->maximum()); }

  void scrollToHeight(int height) { m_scrollBar->setValue(height); }

  void scrollToIndex(VListModel::Index idx) {
    // int currentHeight = m_selected != -1 ? m_model->heightAtIndex(m_selected) : 0;
    int currentHeight = m_scrollBar->value();
    int newHeight = m_model->heightAtIndex(idx);

    m_scrollBar->setValue(m_scrollBar->value() + (newHeight - currentHeight));
    updateViewport();
  }

  void selectFirst() {
    for (int i = 0; i != m_count; ++i) {
      if (m_model->isSelectable(i)) {
        setSelected(i);
        break;
      }
    }
  }

  /**
   * Select the item above the current selection.
   * `true` is selected if such an item exists and was selected, `false` otherwise.
   */
  bool selectUp() {
    for (int i = m_selected - 1; i >= 0; --i) {
      if (m_model->isSelectable(i)) {
        setSelected(i);
        return true;
      }
    }
    return false;
  }

  /**
   * Select the item below the current selection.
   * `true` is selected if such an item exists and was selected, `false` otherwise.
   */
  bool selectDown() {
    for (int i = m_selected + 1; i < m_count; ++i) {
      if (m_model->isSelectable(i)) {
        setSelected(i);
        return true;
      }
    }
    return false;
  }

  void setMargins(int n) { setMargins(QMargins{n, n, n, n}); }

protected:
  void setupUI() {
    m_scrollBar = new OmniScrollBar(this);
    m_margins = QMargins(5, 0, 5, 0);
    connect(m_scrollBar, &QScrollBar::valueChanged, this, &VListWidget::handleScrollChanged);
  }

  bool event(QEvent *event) override {
    if (event->type() == QEvent::Wheel) {
      QApplication::sendEvent(m_scrollBar, event);
      return true;
    }
    return QWidget::event(event);
  }

  void handleScrollChanged(int value) { updateViewport(); }

  void resizeEvent(QResizeEvent *event) override {
    QSize scrollbarSize = m_scrollBar->sizeHint();
    QSize size = event->size();
    QWidget::resizeEvent(event);
    m_scrollBar->setPageStep(size.height());
    m_scrollBar->move(size.width() - scrollbarSize.width(), 0);
    m_scrollBar->setFixedHeight(size.height());
    m_scrollBar->show();
    calculate();
  }

private:
  struct WidgetData {
    WidgetWrapper *widget = nullptr;
    VListModel::WidgetTag tag = 0;
  };

  struct ViewportItem {
    QRect bounds;
    VListModel::StableID id;
    VListModel::Index index = -1;
    VListModel::WidgetTag tag = -1;
    WidgetWrapper *widget = nullptr;
  };

  WidgetWrapper *getFromPool(VListModel::WidgetTag tag) {
    auto pool = m_recyclingPool.find(tag);
    if (pool == m_recyclingPool.end() || pool->second.empty()) return nullptr;
    auto back = pool->second.back();
    pool->second.pop_back();
    return back;
  }

  void updateViewport() {
    Timer timer;
    std::unordered_map<VListModel::StableID, WidgetData> newMap;

    // TODO: handle top margin

    newMap.reserve(m_widgetMap.size());

    {
      m_visibleItems.clear();
      int x = m_margins.left();
      int y = 0;
      int scrollHeight = m_scrollBar->value();
      QSize viewport = size();
      int availableWidth = viewport.width() - m_margins.left() - m_margins.right();
      VListModel::Index idx = m_model->indexAtHeight(scrollHeight);

      qDebug() << "start index" << idx << viewport;

      y = -(scrollHeight % m_model->height(idx)); // account for partially visible item

      while (y < viewport.height() && idx < m_count) {
        bool directHit = false;
        ViewportItem item;

        item.id = m_model->stableId(idx);

        if (auto it = m_widgetMap.find(item.id); it != m_widgetMap.end()) {
          item.widget = it->second.widget;
          item.tag = it->second.tag;
          m_widgetMap.erase(it);
        }

        int height = m_model->height(idx);

        item.bounds = QRect{x, y, availableWidth, height};
        item.index = idx;
        y += height;
        ++idx;
        m_visibleItems.emplace_back(item);
      }
    }

    for (auto &item : m_visibleItems) {
      if (!item.widget) {
        item.tag = m_model->widgetTag(item.index);
        // before we consider querying recycling pools, try to reuse a similar widget
        // that is already on screen (cheaper)
        auto recyclable =
            std::ranges::find_if(m_widgetMap, [&](auto &&p) { return p.second.tag == item.tag; });

        if (recyclable != m_widgetMap.end()) {
          item.widget = recyclable->second.widget;
          m_widgetMap.erase(recyclable);
        } else {
          item.widget = getFromPool(item.tag);
          if (!item.widget) {
            auto wrapper = new WidgetWrapper;

            connect(wrapper, &WidgetWrapper::clicked, this,
                    [wrapper, this]() { setSelected(wrapper->index()); });
            connect(wrapper, &WidgetWrapper::doubleClicked, this,
                    [wrapper, this]() { setSelected(wrapper->index()); });

            wrapper->setWidget(m_model->createWidget(item.index));
            wrapper->setParent(this);
            item.widget = wrapper;
          } else {
          }
        }
      }

      item.widget->widget()->selectionChanged(item.index == m_selected);

      newMap[item.id] = {.widget = item.widget, .tag = item.tag};
      item.widget->setIndex(item.index);
      item.widget->setFixedSize(item.bounds.width(), item.bounds.height());
      item.widget->move(item.bounds.x(), item.bounds.y());
      m_model->refreshWidget(item.index, item.widget->widget());
      item.widget->show();
    }

    for (const auto &[id, w] : m_widgetMap) {
      if (!newMap.contains(id)) {
        w.widget->hide();
        m_recyclingPool[w.tag].emplace_back(w.widget);
      }
    }

    m_widgetMap = newMap;

    timer.time("update viewport");
  }

  std::unordered_map<VListModel::StableID, WidgetData> m_widgetMap;
  std::unordered_map<VListModel::WidgetTag, std::vector<WidgetWrapper *>> m_recyclingPool;
  std::vector<ViewportItem> m_visibleItems;

  int m_selected = -1;

  QMargins m_margins;
  QScrollBar *m_scrollBar = nullptr;
  VListModel *m_model = nullptr;
  size_t m_count = 0;

  static constexpr const size_t DEFAULT_PAGE_STEP = 40;
};

}; // namespace vicinae::ui

#include "timer.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <qobjectdefs.h>
#include <qscrollbar.h>
#include <qtsqlglobal.h>
#include <qwidget.h>

namespace vicinae::ui {

class VListModel {
public:
  using StableID = size_t;
  using Index = size_t;

  virtual size_t count() const = 0;
  virtual size_t height(Index idx) const = 0;
  virtual Index indexAtHeight(int height) const = 0;
  virtual size_t heightAtIndex(Index idx) const = 0;

  virtual bool isSelectable(Index idx) const = 0;

  /**
   * The full height of the list
   */
  virtual size_t height() const = 0;

  /**
   * A stable ID that can be used to identify the item that is currently at index `idx`.
   * The returned ID should not be tied to `idx`.
   * This is used internally to reuse widgets across model changes.
   */
  virtual StableID stableId(Index idx) const = 0;

  virtual void selectionChanged(Index idx, QWidget *w, bool selected) {}

  virtual QWidget *createWidget(Index idx) const = 0;
  virtual void refreshWidget(Index idx, QWidget *widget) const = 0;
};

class VListWidget : public QWidget {
  static constexpr const size_t DEFAULT_PAGE_STEP = 40;

public:
  VListWidget() { setupUI(); }

  void setModel(VListModel *model) {
    m_model = model;
    calculate();
  }

  void calculate() {
    m_visibleRange = {-1, -1};
    m_scrollBar->setMinimum(0);
    m_scrollBar->setMaximum(std::max(0, (int)m_model->height() - (int)size().height()));
    m_scrollBar->setPageStep(DEFAULT_PAGE_STEP);
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
    qDebug() << "select item at index" << idx;
    bool isInViewport = idx >= m_visibleRange.first && idx < m_visibleRange.second;

    if (m_selected != -1) {
      if (auto it = m_widgetMap.find(m_model->stableId(m_selected)); it != m_widgetMap.end()) {
        m_model->selectionChanged(idx, it->second, false);
      }
    }

    if (!isInViewport) {
      int currentHeight = m_selected != -1 ? m_model->heightAtIndex(m_selected) : 0;
      int newHeight = m_model->heightAtIndex(idx);

      m_scrollBar->setValue(m_scrollBar->value() + (newHeight - currentHeight));
      updateViewport();
    }

    m_selected = idx;

    if (auto it = m_widgetMap.find(m_model->stableId(idx)); it != m_widgetMap.end()) {
      m_model->selectionChanged(idx, it->second, true);
    }
  }

  void selectFirst() {
    for (int i = 0; i != m_model->count(); ++i) {
      if (m_model->isSelectable(i)) {
        setSelected(i);
        break;
      }
    }
  }

  bool selectUp() {
    for (int i = m_selected - 1; i >= 0; --i) {
      if (m_model->isSelectable(i)) {
        setSelected(i);
        return true;
      }
    }
    return false;
  }

  bool selectDown() {
    for (int i = m_selected + 1; i < m_model->count(); ++i) {
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

  void handleScrollChanged(int value) {
    qDebug() << "scroll value changed to" << value;
    updateViewport();
  }

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
  void updateViewport() {
    Timer timer;
    int x = m_margins.left(), y = 0;
    QSize viewport = size();
    int scrollHeight = m_scrollBar->value();
    int availableWidth = viewport.width() - m_margins.left() - m_margins.right();
    VListModel::Index idx = m_model->indexAtHeight(scrollHeight);
    int relativeScrollOffset = scrollHeight % viewport.height();
    int firstHeight = m_model->height(idx);

    qDebug() << ">>>>>>> FRAME <<<<<";
    qDebug() << "viewport height" << viewport.height() << "scroll height" << scrollHeight << "relative"
             << relativeScrollOffset << "h" << firstHeight;

    y = -(scrollHeight % m_model->height(idx));

    qDebug() << "computed y" << y;
    qDebug() << ">>>>>>> END FRAME <<<<<";

    std::unordered_map<VListModel::StableID, QWidget *> newMap;

    m_visibleRange.first = idx;

    newMap.reserve(m_widgetMap.size());

    while (y < viewport.height() && idx < m_model->count()) {
      qDebug() << "index" << idx;

      VListModel::StableID id = m_model->stableId(idx);
      QWidget *w = nullptr;

      if (auto it = m_widgetMap.find(id); it != m_widgetMap.end()) {
        w = it->second;
      } else {
        w = m_model->createWidget(idx);
        w->setParent(this);
      }

      newMap[id] = w;

      int height = m_model->height(idx);

      w->setFixedSize(availableWidth, height);
      w->move(x, y);
      m_model->refreshWidget(idx, w);
      w->show();
      y += height;
      ++idx;
    }

    m_visibleRange.second = idx - 1;

    for (const auto &[id, w] : m_widgetMap) {
      if (!newMap.contains(id)) { w->deleteLater(); }
    }

    m_widgetMap = newMap;

    timer.time("update viewport");
  }

  std::unordered_map<VListModel::StableID, QWidget *> m_widgetMap;
  int m_selected = -1;

  std::pair<VListModel::Index, VListModel::Index> m_visibleRange;

  QMargins m_margins;
  QScrollBar *m_scrollBar = nullptr;
  VListModel *m_model = nullptr;
};

}; // namespace vicinae::ui

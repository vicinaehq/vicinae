#include "timer.hpp"
#include <absl/time/time.h>
#include <qwidget.h>

namespace vicinae::ui {

class VListModel {
public:
  virtual size_t count() const = 0;
  virtual size_t height(size_t idx) const = 0;
  virtual QWidget *createWidget(size_t idx) const = 0;
  virtual void refreshWidget(size_t idx, QWidget *widget) const = 0;
};

class VListWidget : public QWidget {
public:
  void setModel(VListModel *model) {
    m_model = model;
    calculate();
  }

  void calculate() {
    for (const auto &w : m_widgets) {
      w->deleteLater();
    }
    m_widgets.clear();

    if (!m_model) return;

    int count = m_model->count();
    int x = 0, y = 0;
    QSize viewport = size();

    m_items.reserve(count);
    m_items.clear();

    for (int i = 0; i != m_model->count(); ++i) {
      int h = m_model->height(i);
      m_items.emplace_back(VirtualItem{.x = x, .y = y, .width = size().width(), .height = h});
      y += h;
    }
    m_vHeight = y;
    updateViewport();
  }

protected:
  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    calculate();
  }

private:
  struct VirtualItem {
    int x;
    int y;
    int width;
    int height;
  };

  void updateViewport() {
    Timer timer;
    int x = 0, y = 0;
    QSize viewport = size();

    qDebug() << "update viewport for" << m_items.size() << "items";

    for (int i = 0; i != m_items.size() && y < viewport.height(); ++i) {
      auto &item = m_items[i];
      QWidget *w = m_model->createWidget(i);
      w->setParent(this);
      qDebug() << "item" << item.width << item.height << item.x << item.y;
      w->setFixedSize(item.width, item.height);
      w->move(x, y);
      m_model->refreshWidget(i, w);
      w->show();
      y += item.height;
      m_widgets.emplace_back(w);
    }

    timer.time("update viewport");
  }

  std::vector<VirtualItem> m_items;
  std::vector<QWidget *> m_widgets;
  int m_vHeight = 0;
  VListModel *m_model = nullptr;
};

}; // namespace vicinae::ui

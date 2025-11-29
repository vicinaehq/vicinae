#include "vlist.hpp"
#include <absl/base/call_once.h>

namespace vicinae::ui {
VListWidget::VListWidget() {
  m_visibleItems.reserve(32);
  setupUI();
}

void VListWidget::setModel(VListModel *model) {
  connect(model, &VListModel::dataChanged, this, &VListWidget::calculate);
  m_model = model;
  calculate();
}

void VListWidget::calculate() {
  if (!m_model) return;

  m_height = m_model->height() + m_margins.top() + m_margins.bottom();
  m_count = m_model->count();
  m_scrollBar->setMinimum(0);
  m_scrollBar->setMaximum(std::max(0, m_height - (int)size().height()));
  m_scrollBar->setSingleStep(DEFAULT_PAGE_STEP);
  m_scrollBar->setVisible(m_height > size().height());

  if (m_selected) {
    if (m_selected->idx >= m_count) {
      m_selected.reset();
      selectFirst();
    } else {
      setSelected(m_selected->idx);
    }
  } else {
    selectFirst();
  }

  updateViewport();
}

void VListWidget::selectNext() {
  int startIdx = m_selected.transform([](const Selection &s) { return s.idx + 1; }).value_or(0);

  for (int i = startIdx; i < m_count; ++i) {
    if (m_model->isSelectable(i)) {
      setSelected(i);
      return;
    }
  }

  selectFirst();
}

void VListWidget::selectFirst() {
  for (int i = 0; i != m_count; ++i) {
    if (m_model->isSelectable(i)) {
      setSelected(i);
      return;
    }
  }

  m_selected.reset();
  emit itemSelected({});
}

bool VListWidget::activateCurrentSelection() {
  if (!m_selected) return false;
  emit itemActivated(m_selected->idx);
  return true;
}

bool VListWidget::selectUp() {
  if (!m_selected) {
    selectFirst();
    return true;
  }

  for (int i = m_selected->idx - 1; i >= 0; --i) {
    if (m_model->isSelectable(i)) {
      setSelected(i);
      return true;
    }
  }

  return false;
}

bool VListWidget::selectDown() {
  if (!m_selected) {
    selectFirst();
    return true;
  }
  for (int i = m_selected->idx + 1; i < m_count; ++i) {
    if (m_model->isSelectable(i)) {
      setSelected(i);
      return true;
    }
  }
  return false;
}

void VListWidget::setMargins(const QMargins &margins) {
  m_margins = margins;
  calculate();
}

void VListWidget::setMargins(int n) { setMargins(QMargins{n, n, n, n}); }

void VListWidget::setSelected(VListModel::Index idx) {
  auto id = m_model->stableId(idx);

  if (!m_selected || m_selected->id != id) {
    m_selected = Selection{.idx = idx, .id = id};
    emit itemSelected(idx);
  }

  scrollToIndex(idx);
  updateViewport();
}

void VListWidget::scrollToIndex(VListModel::Index idx, ScrollAnchor anchor) {
  if (idx == 0) {
    m_scrollBar->setValue(0);
    return;
  }

  int startY = m_margins.top() + m_model->heightAtIndex(idx);
  int itemHeight = m_model->height(idx);

  switch (anchor) {
  case ScrollAnchor::Top: {
    m_scrollBar->setValue(startY - height());
    break;
  }
  case ScrollAnchor::Bottom: {
    m_scrollBar->setValue(startY);
    break;
  }
  case ScrollAnchor::Relative: {
    int distance = 0;
    int endY = startY + itemHeight;
    int scrollHeight = m_scrollBar->value();
    bool isFullyInViewport = startY >= scrollHeight && endY <= scrollHeight + height();

    if (endY > scrollHeight + height()) {
      if (isFullyInViewport) return;
      distance = endY - scrollHeight - height();
    } else {
      if (idx > 0 && m_model->isAnchor(idx - 1)) { return scrollToIndex(idx - 1, anchor); }
      if (isFullyInViewport) return;
      distance = startY - scrollHeight;
    }
    m_scrollBar->setValue(scrollHeight + distance);
    break;
  }
  }
}

void VListWidget::setupUI() {
  m_scrollBar = new OmniScrollBar(this);
  m_scrollBar->raise();
  connect(m_scrollBar, &QScrollBar::valueChanged, this, &VListWidget::handleScrollChanged);
}

bool VListWidget::event(QEvent *event) {
  if (event->type() == QEvent::Wheel) {
    QApplication::sendEvent(m_scrollBar, event);
    return true;
  }
  return QWidget::event(event);
}

void VListWidget::resizeEvent(QResizeEvent *event) {
  QSize scrollbarSize = m_scrollBar->sizeHint();
  QSize size = event->size();
  QWidget::resizeEvent(event);
  m_scrollBar->setPageStep(size.height());
  m_scrollBar->move(size.width() - scrollbarSize.width(), 0);
  m_scrollBar->setFixedHeight(size.height());
  m_scrollBar->show();
  calculate();
}

WidgetWrapper *VListWidget::getFromPool(VListModel::WidgetTag tag) {
  auto pool = m_recyclingPool.find(tag);
  if (pool == m_recyclingPool.end() || pool->second.empty()) return nullptr;
  auto back = pool->second.back();
  pool->second.pop_back();
  return back;
}

void VListWidget::updateViewport() {
  Timer timer;
  std::unordered_map<VListModel::StableID, WidgetData> newMap;

  newMap.reserve(m_widgetMap.size());

  {
    m_visibleItems.clear();
    int x = m_margins.left();
    int y = 0;
    int scrollHeight = m_scrollBar->value();
    QSize viewport = size();
    int availableWidth = viewport.width() - m_margins.left() - m_margins.right();
    VListModel::Index idx = m_model->indexAtHeight(scrollHeight);

    if (idx < m_count) { y = -(scrollHeight - (m_margins.top() + m_model->heightAtIndex(idx))); }

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

      if (item.tag != VListModel::InvalidTag) {
        // before we consider querying recycling pools, try to reuse a similar widget
        // that is already on screen (cheaper)
        auto recyclable =
            std::ranges::find_if(m_widgetMap, [&](auto &&p) { return p.second.tag == item.tag; });

        if (recyclable != m_widgetMap.end()) {
          item.widget = recyclable->second.widget;
          m_widgetMap.erase(recyclable);
        } else {
          item.widget = getFromPool(item.tag);
        }
      }

      if (!item.widget) {
        auto wrapper = new WidgetWrapper;

        connect(wrapper, &WidgetWrapper::clicked, this, [wrapper, this]() { setSelected(wrapper->index()); });
        connect(wrapper, &WidgetWrapper::doubleClicked, this,
                [wrapper, this]() { emit itemActivated(wrapper->index()); });

        wrapper->setWidget(m_model->createWidget(item.index));
        wrapper->setParent(this);
        item.widget = wrapper;
      }
    }

    item.widget->widget()->selectionChanged(m_selected && item.index == m_selected->idx);

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

}; // namespace vicinae::ui

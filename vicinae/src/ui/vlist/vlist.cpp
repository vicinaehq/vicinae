#include "vlist.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <ranges>

namespace vicinae::ui {

static constexpr const int SCROLL_FPS = 120;
static constexpr const double SCROLL_FRAME_TIME = 1000.0 / SCROLL_FPS;

VListWidget::VListWidget() {
  m_visibleItems.reserve(32);
  setupUI();
}

void VListWidget::setModel(VListModel *model) {
  m_model = model;
  connect(model, &VListModel::dataChanged, this, [this]() {
    m_model->onDataChanged();
    calculate();
  });
  calculate();
}

void VListWidget::recalculateMousePosition() {
  QPoint globalPos = QCursor::pos();

  for (const auto &info : visibleItems()) {
    QPoint localPos = info.widget->mapFromGlobal(globalPos);
    bool isUnderCursor = info.widget->rect().contains(localPos);

    if (!isUnderCursor) { info.widget->widget()->clearTransientState(); }

    if (info.widget->underMouse()) {
      info.widget->hide();
      info.widget->show();
    }
  }
}

void VListWidget::updateFocusChain() {
  int lastIdx = std::max(0, static_cast<int>(m_visibleItems.size() - 1));

  for (int i = 0; i != lastIdx; ++i) {
    QWidget *current = m_visibleItems.at(i).widget;
    QWidget *next = m_visibleItems.at(i + 1).widget;
    setTabOrder(current, next);
  }
}

void VListWidget::calculate() {
  if (!m_model) {
    m_count = 0;
    return;
  }

  if (m_model) m_model->viewportChanged(QSize(width() - m_margins.left() - m_margins.right(), height()));
  m_height = m_model->height() + m_margins.top() + m_margins.bottom();
  m_count = m_model->count();
  m_scrollBar->setMinimum(0);
  m_scrollBar->setMaximum(std::max(0, m_height - size().height()));
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
  if (!m_model) return;
  int startIdx = m_selected.transform([](const Selection &s) { return s.idx + 1; }).value_or(0);

  for (int i = startIdx; i < m_count; ++i) {
    if (m_model->isSelectable(i)) {
      setSelected(i);
      return;
    }
  }

  selectFirst();
}

std::optional<VListModel::Index> VListWidget::firstSelectableIndex() const {
  if (!m_model) return std::nullopt;
  for (int i = 0; i != m_count; ++i) {
    if (m_model->isSelectable(i)) { return i; }
  }
  return std::nullopt;
}

std::optional<VListModel::Index> VListWidget::lastSelectableIndex() const {
  if (!m_model) return std::nullopt;
  for (int i = m_count - 1; i >= 0; --i) {
    if (m_model->isSelectable(i)) { return i; }
  }
  return std::nullopt;
}

void VListWidget::selectFirst() { setSelected(firstSelectableIndex()); }

void VListWidget::selectLast() { setSelected(lastSelectableIndex()); }

std::span<const VListWidget::ViewportItem> VListWidget::visibleItems() const { return m_visibleItems; }

bool VListWidget::refresh(VListModel::Index idx) const {
  auto w = widgetAt(idx);
  if (!w) return false;
  m_model->refreshWidget(idx, w);
  return w;
}

void VListWidget::refreshAll() {
  for (const auto &item : m_visibleItems) {
    m_model->refreshWidget(item.index, item.widget->widget());
  }
}

VListModel::WidgetType *VListWidget::widgetAt(VListModel::Index idx) const {
  if (auto it = std::ranges::find_if(m_visibleItems, [&](auto &&w) { return w.index == idx; });
      it != m_visibleItems.end()) {
    return it->widget->widget();
  }
  return nullptr;
}

bool VListWidget::activateCurrentSelection() {
  if (!m_selected) return false;
  emit itemActivated(m_selected->idx);
  return true;
}

bool VListWidget::selectLeft() {
  if (!m_model) return false;
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

bool VListWidget::selectRight() {
  if (!m_model) return false;
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

bool VListWidget::selectUp() {
  if (!m_model) return false;
  if (!m_selected) {
    selectFirst();
    return true;
  }

  const auto first = firstSelectableIndex();
  if (first && m_selected->idx == *first) {
    selectLast();
    return true;
  }

  int idx = m_selected->idx;
  int currentX = m_model->xAtIndex(idx);
  int currentY = m_model->heightAtIndex(idx);
  std::optional<std::pair<VListModel::Index, int>> firstSelectable;

  for (int i = idx - 1; i >= 0; --i) {
    int newX = m_model->xAtIndex(i);
    int newY = m_model->heightAtIndex(i);

    if (firstSelectable && newY < firstSelectable->second) {
      setSelected(firstSelectable->first);
      return true;
    }
    if (newY < currentY) {
      if (m_model->isSelectable(i)) {
        if (newX <= currentX) {
          setSelected(i);
          return true;
        }
        if (!firstSelectable) { firstSelectable = {i, newY}; }
      }
    }
  }

  return false;
}

bool VListWidget::selectDown() {
  if (!m_model) return false;
  if (!m_selected) {
    selectFirst();
    return true;
  }

  const auto last = lastSelectableIndex();
  if (last && m_selected->idx == *last) {
    selectFirst();
    return true;
  }

  int currentY = m_model->heightAtIndex(m_selected->idx);
  int currentX = m_model->xAtIndex(m_selected->idx);
  std::optional<std::pair<VListModel::Index, int>> firstSelectable;

  for (int i = m_selected->idx + 1; i < m_count; ++i) {
    int newX = m_model->xAtIndex(i);
    int newY = m_model->heightAtIndex(i);

    if (firstSelectable && newY > firstSelectable->second) {
      setSelected(firstSelectable->first);
      return true;
    }

    if (newY > currentY) {
      if (m_model->isSelectable(i)) {
        if (newX >= currentX) {
          setSelected(i);
          return true;
        }
        if (!firstSelectable) { firstSelectable = {i, newY}; }
      }
    }
  }

  return false;
}

void VListWidget::pageDown() {
  m_scrollBar->setValue(
      std::min(m_scrollBar->value() + m_scrollBar->pageStep(), m_scrollBar->maximumHeight()));
  if (!m_visibleItems.empty()) { setSelected(m_visibleItems.front().index); }
}

void VListWidget::pageUp() {
  m_scrollBar->setValue(std::max(0, m_scrollBar->value() - m_scrollBar->pageStep()));
  if (!m_visibleItems.empty()) { setSelected(m_visibleItems.front().index); }
}

void VListWidget::setMargins(const QMargins &margins) {
  m_margins = margins;
  calculate();
}

void VListWidget::setMargins(int n) { setMargins(QMargins{n, n, n, n}); }

void VListWidget::setSelected(std::optional<VListModel::Index> idx) {
  if (!m_model) {
    if (m_selected) {
      m_selected.reset();
      emit itemSelected(std::nullopt);
    }
    return;
  }

  if (!idx) {
    if (m_selected) {
      m_selected.reset();
      emit itemSelected(std::nullopt);
    }
    return;
  }

  auto id = m_model->stableId(idx.value());

  if (!m_selected || m_selected->id != id) {
    m_selected = Selection{.idx = idx.value(), .id = id};
    emit itemSelected(idx);
  }

  scrollToIndex(idx.value());
  updateViewport();
}

std::optional<VListModel::Index> VListWidget::getTopItem(VListModel::Index idx) const {
  int currentX = m_model->xAtIndex(idx);
  int currentY = m_model->heightAtIndex(idx);

  for (int i = idx - 1; i >= 0; --i) {
    int newX = m_model->xAtIndex(i);
    int newY = m_model->heightAtIndex(i);

    if (newY < currentY && newX <= currentX) { return i; }
  }

  return std::nullopt;
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
      if (auto top = getTopItem(idx); top && m_model->isAnchor(top.value())) {
        return scrollToIndex(top.value(), anchor);
      }
      if (isFullyInViewport) return;
      distance = startY - scrollHeight;
    }
    m_scrollBar->setValue(scrollHeight + distance);
    break;
  }
  }
}
std::optional<VListModel::Index> VListWidget::currentSelection() const {
  return m_selected.transform([](auto &&s) { return s.idx; });
};

void VListWidget::setupUI() {
  m_scrollBar = new OmniScrollBar(this);
  m_scrollBar->raise();
  m_scrollTimer.setSingleShot(true);
  connect(&m_scrollTimer, &QTimer::timeout, this, [this]() { handleScrollChanged(m_scrollBar->value()); });
  connect(m_scrollBar, &QScrollBar::valueChanged, this, [this]() {
    if (!m_scrollTimer.isActive()) m_scrollTimer.start(SCROLL_FRAME_TIME);
  });
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
  if (m_model) m_model->viewportChanged(QSize(width() - m_margins.left() - m_margins.right(), height()));
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
  if (!size().isValid() || size().isNull() || size().isEmpty()) return;

  setUpdatesEnabled(false);

  // Timer timer;
  std::unordered_map<VListModel::StableID, WidgetData> newMap;

  newMap.reserve(m_widgetMap.size());

  {
    m_visibleItems.clear();
    int x = m_margins.left();
    int scrollHeight = m_scrollBar->value();
    QSize viewport = size();
    int availableWidth = viewport.width() - m_margins.left() - m_margins.right();
    QSize allocatableViewport(availableWidth, height());

    if (m_count > 0) {
      VListModel::Index idx = m_model->indexAtHeight(scrollHeight);
      if (idx != VListModel::InvalidIndex) {
        int maxHeight = 0;
        int offset = 0;
        int startingY = m_margins.top() + m_model->heightAtIndex(idx);

        if (idx < m_count) {
          int heightAtIndex = m_model->heightAtIndex(idx);
          offset = -(scrollHeight - (m_margins.top() + m_model->heightAtIndex(idx)));
        }

        int max = startingY + viewport.height() - offset;

        while (idx < m_count) {
          int currentY = m_margins.top() + m_model->heightAtIndex(idx);
          if (currentY > max) break;

          ViewportItem item;

          item.id = m_model->stableId(idx);

          if (auto it = m_widgetMap.find(item.id); it != m_widgetMap.end()) {
            item.widget = it->second.widget;
            item.tag = it->second.tag;
            m_widgetMap.erase(it);
          }

          QSize allocatedSize = m_model->allocateSize(idx, allocatableViewport);

          item.bounds = QRect{m_margins.left() + m_model->xAtIndex(idx), currentY - startingY + offset,
                              allocatedSize.width(), allocatedSize.height()};
          item.index = idx;
          m_visibleItems.emplace_back(item);
          ++idx;
        }
      }
    }
  }

  for (auto &item : m_visibleItems) {
    bool wasCached = item.widget;

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
        auto wrapper = new WidgetWrapper(this);

        wrapper->setWidget(m_model->createWidget(item.index));
        wrapper->stackUnder(m_scrollBar);
        item.widget = wrapper;

        connect(wrapper, &WidgetWrapper::clicked, this, [wrapper, this]() { setSelected(wrapper->index()); });
        connect(wrapper, &WidgetWrapper::doubleClicked, this,
                [wrapper, this]() { emit itemActivated(wrapper->index()); });
      }
    }

    if (!wasCached) { m_model->refreshWidget(item.index, item.widget->widget()); }

    item.widget->setIndex(item.index);
    item.widget->setSelected(m_selected && item.index == m_selected->idx);
    item.widget->setFixedSize(item.bounds.width(), item.bounds.height());
    item.widget->move(item.bounds.x(), item.bounds.y());
    item.widget->show();

    newMap[item.id] = {.widget = item.widget, .tag = item.tag};
  }

  for (const auto &[id, w] : m_widgetMap) {
    if (!newMap.contains(id)) {
      w.widget->hide();
      m_recyclingPool[w.tag].emplace_back(w.widget);
    }
  }

  m_widgetMap = newMap;

  recalculateMousePosition();
  updateFocusChain();
  setUpdatesEnabled(true);
  update();

  // timer.time("update viewport");
}

}; // namespace vicinae::ui

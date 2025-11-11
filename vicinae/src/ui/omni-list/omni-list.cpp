#include "omni-list.hpp"
#include "timer.hpp"
#include "ui/list-section-header.hpp"
#include "ui/omni-list/omni-list-item-widget-wrapper.hpp"
#include "utils.hpp"
#include <algorithm>
#include <chrono>
#include <numeric>
#include <qabstractitemview.h>
#include <qapplication.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>

QString OmniList::VirtualSection::generateId() const { return QUuid::createUuid().toString(); }

OmniListItemWidget *OmniList::VirtualSection::createWidget() const {
  return new OmniListSectionHeader(_name, "", 0);
}

OmniList::VirtualSection::VirtualSection(const QString &name) : _name(name) {}

bool OmniList::AbstractVirtualItem::recyclable() const { return false; }

bool OmniList::AbstractVirtualItem::selectable() const { return true; }

OmniList::AbstractVirtualItem::ListRole OmniList::AbstractVirtualItem::role() const {
  return ListRole::ListItem;
}

void OmniList::AbstractVirtualItem::recycle(QWidget *base) const {}

bool OmniList::AbstractVirtualItem::isListItem() const { return role() == ListRole::ListItem; };

bool OmniList::AbstractVirtualItem::isSection() const { return role() == ListRole::ListSection; }

size_t OmniList::AbstractVirtualItem::typeId() const { return typeid(*this).hash_code(); }

void OmniList::itemClicked(int index) { setSelectedIndex(index); }

void OmniList::itemDoubleClicked(int index) const { emit itemActivated(*m_items[index].item); }

void OmniList::rightClicked(int index) const { emit itemRightClicked(*m_items[index].item); }

void OmniList::updateFocusChain() {
  if (m_visibleWidgets.empty()) return;

  for (QWidget *widget : m_visibleWidgets) {
    setTabOrder(widget, nullptr);
  }

  for (int i = 0; i != m_visibleWidgets.size() - 1; ++i) {
    QWidget *current = m_visibleWidgets[i];
    QWidget *next = m_visibleWidgets[i + 1];

    setTabOrder(current, next);
  }
}

void OmniList::updateVisibleItems() {

  m_visibleWidgets.clear();
  if (m_items.empty()) return;
  setUpdatesEnabled(false);

  int scrollHeight = scrollBar->value();
  size_t startIndex = 0;

  while (startIndex < m_items.size() && !isInViewport(m_items.at(startIndex).bounds)) {
    ++startIndex;
  }

  if (startIndex >= m_items.size()) {
    visibleIndexRange = VisibleRangeV2::empty();
    return;
  }

  auto &cell = m_items[startIndex];
  int marginOffset = std::max(0, margins.top - scrollHeight);
  QSize viewportSize = size();
  size_t endIndex = startIndex;

  std::unordered_set<QString> reused;

  // we perform a first scan to mark all cached widgets with an ID in range
  // so that we can reuse them directly. Calling refresh() on a widget with the same ID
  // is very likely to be a no-op.

  int viewportY = marginOffset + cell.bounds.y() - scrollHeight;
  int lastY = -1;

  while (endIndex < m_items.size()) {
    auto &vinfo = m_items[endIndex];
    auto cacheIt = _widgetCache.find(vinfo.item->id());

    if (lastY != -1 && lastY != vinfo.bounds.y()) { viewportY += vinfo.bounds.y() - lastY; }

    if (viewportY >= viewportSize.height()) break;

    if (cacheIt != _widgetCache.end()) reused.insert(vinfo.item->id());

    lastY = vinfo.bounds.y();
    ++endIndex;
  }

  lastY = -1;
  viewportY = marginOffset + cell.bounds.y() - scrollHeight;
  endIndex = startIndex;

  while (endIndex < m_items.size()) {
    auto &vinfo = m_items[endIndex];
    auto cacheIt = _widgetCache.find(vinfo.item->id());
    OmniListItemWidgetWrapper *widget = nullptr;

    if (lastY != -1 && lastY != vinfo.bounds.y()) { viewportY += vinfo.bounds.y() - lastY; }

    if (viewportY >= viewportSize.height()) break;

    bool isWidgetCreated = false;

    auto start = std::chrono::high_resolution_clock::now();

    if (cacheIt == _widgetCache.end()) {
      bool recycled = false;
      // TODO: find a more efficient solution, this is doing too much map stuff
      for (const auto &[key, cache] : _widgetCache) {
        bool reserved = reused.find(key) != reused.end();

        if (!reserved && vinfo.item->recyclable() && vinfo.item->recyclingId() == cache.recyclingId) {
          vinfo.item->recycle(cache.widget->widget());
          vinfo.item->attached(cache.widget->widget());
          widget = cache.widget;
          _widgetCache[vinfo.item->id()] = cache;
          reused.insert(vinfo.item->id());
          // qDebug() << "placed to reused" << vinfo.item->id();
          _widgetCache.erase(key);

          // qInfo() << "recycled in place" << vinfo.item->id();
          recycled = true;
          break;
        }
      }

      if (!recycled) {
        if (auto wrapper = takeFromPool(vinfo.item->recyclingId())) {
          wrapper->setParent(this);
          vinfo.item->recycle(wrapper->widget());
          vinfo.item->attached(wrapper->widget());
          wrapper->blockSignals(false);
          wrapper->setUpdatesEnabled(true);
          widget = wrapper;
        } else {
          widget = new OmniListItemWidgetWrapper(this);
          connect(widget, &OmniListItemWidgetWrapper::clicked, this, &OmniList::itemClicked,
                  Qt::UniqueConnection);
          connect(widget, &OmniListItemWidgetWrapper::doubleClicked, this, &OmniList::itemDoubleClicked,
                  Qt::UniqueConnection);
          connect(widget, &OmniListItemWidgetWrapper::rightClicked, this, &OmniList::rightClicked,
                  Qt::UniqueConnection);
          widget->stackUnder(scrollBar);
          OmniListItemWidget *w = vinfo.item->createWidget();
          widget->setWidget(w);
          vinfo.item->attached(w);
        }

        CachedWidget cache{.widget = widget, .recyclingId = 0};

        if (vinfo.item->recyclable()) { cache.recyclingId = vinfo.item->recyclingId(); }

        isWidgetCreated = true;
        reused.insert(vinfo.item->id());
        _widgetCache[vinfo.item->id()] = cache;
      }
    } else {
      widget = cacheIt->second.widget;
    }

    QPoint pos(vinfo.bounds.x(), viewportY);
    QSize size(vinfo.bounds.width(), vinfo.bounds.height());

    // qDebug() << "pos" << pos << "size" << size << "index" << endIndex << vinfo.item->id();

    widget->blockSignals(true);
    widget->setIndex(endIndex);
    widget->setSelected(endIndex == m_selected);
    if (widget->size() != size) { widget->resize(size); }
    widget->move(pos);
    widget->show();

    /*
qDebug() << "Widget" << vinfo.item->id() << "visible:" << widget->isVisible()
         << "parent:" << widget->parent() << "geometry:" << widget->geometry()
         << "enabled:" << widget->isEnabled();
                     */
    widget->blockSignals(false);

    lastY = vinfo.bounds.y();
    m_visibleWidgets.emplace_back(widget);
    reused.insert(vinfo.item->id());

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6;
    ++endIndex;
  }

  for (auto it = _widgetCache.begin(); it != _widgetCache.end();) {
    if (auto match = reused.find(it->first); match == reused.end()) {
      it->second.widget->deleteLater();
      it = _widgetCache.erase(it);
    } else {
      ++it;
    }
  }

  setUpdatesEnabled(true);
  recalculateMousePosition();
  updateFocusChain();
  this->visibleIndexRange = {startIndex, endIndex - startIndex};
}

bool OmniList::isDividableContent(const ModelItem &item) {
  if (auto section = std::get_if<std::unique_ptr<Section>>(&item)) {
    return (*section)->layoutItems().size() > 0;
  }

  return false;
}

void OmniList::calculateHeights() {
  Timer timer;

  int yOffset = 0;
  int availableWidth = width() - margins.left - margins.right;
  std::optional<SectionCalculationContext> sctx;
  std::unordered_map<QString, CachedWidget> updatedCache;

  auto view = m_model | std::views::filter([](const auto &item) {
                return std::holds_alternative<std::unique_ptr<Section>>(item);
              }) |
              std::views::transform([](const auto &section) {
                return std::get<std::unique_ptr<Section>>(section)->itemCount();
              });
  auto totalSize = std::accumulate(view.begin(), view.end(), 0, std::plus<size_t>());

  m_items.reserve(totalSize);
  m_items.clear();
  visibleIndexRange = VisibleRangeV2::empty();

  size_t i = -1;

  for (const auto &item : m_model) {
    ++i;
    if (auto divider = std::get_if<Divider>(&item)) {
      if (yOffset == 0) continue;
      if (i + 1 == m_model.size() || !isDividableContent(m_model.at(i + 1))) continue;

      int height = divider->item()->calculateHeight(availableWidth);
      QRect bounds(0, yOffset, width(), height);
      VirtualWidgetInfo vinfo{.bounds = bounds, .item = divider->item()};

      m_items.push_back(vinfo);
      yOffset += height;
    } else if (auto p = std::get_if<std::unique_ptr<Section>>(&item)) {
      auto &section = *p;

      auto &items = section->layoutItems();
      int spaceWidth = section->spacing() * (section->columns() - 1);
      int columnWidth = (availableWidth - spaceWidth) / section->columns();

      if (items.empty()) continue;

      sctx = {.x = margins.left, .maxHeight = 0};

      size_t shownCount = 0;

      for (auto &sectionItem : items) {
        if (auto spacer = std::get_if<Spacer>(&sectionItem)) {
          yOffset += spacer->value;
          continue;
        }

        if (auto p = std::get_if<Section::VirtualWidget>(&sectionItem)) {
          auto &item = *p;

          // Show section header above the first actually displayed item (after filtering has been
          // considered)
          if (shownCount == 0) {
            if (auto header = section->headerItem(); header && !section->title().isEmpty()) {
              int height = header->calculateHeight(availableWidth);
              QRect geometry(margins.left, yOffset, availableWidth, height);

              VirtualWidgetInfo vinfo{.bounds = geometry, .item = header};

              m_items.push_back(vinfo);
              yOffset += height;
            }
          }

          ++shownCount;

          int vIndex = m_items.size();
          int height = 0;
          int width = columnWidth;
          int x = margins.left;
          int y = yOffset;

          if (sctx) {
            // width = sctx->section->calculateItemWidth(width, sctx->index);
            //  x = sctx->section->calculateItemX(sctx->x, sctx->index);
            if (sctx->x == margins.left && sctx->index > 0) {
              yOffset += section->spacing();
              y = yOffset;
            }

            x = sctx->x;
            sctx->x = sctx->x + width + section->spacing();
            height = calculateItemHeight(item.get(), width);
            sctx->maxHeight = std::max(sctx->maxHeight, height);

            if (sctx->x >= availableWidth) {
              yOffset += sctx->maxHeight;
              sctx->x = margins.left;
              sctx->maxHeight = 0;
            }

            ++sctx->index;
          } else {
            height = item->calculateHeight(width);
            yOffset += height;
          }

          QRect geometry(x, y, width, height);
          VirtualWidgetInfo vinfo{.bounds = geometry, .item = item.get(), .enumerable = true};

          // TODO: if in viewport, look for cached entry
          if (isInViewport(geometry)) {
            if (auto it = _widgetCache.find(item->id()); it != _widgetCache.end()) {
              QWidget *widget = it->second.widget->widget();

              widget->setUpdatesEnabled(false);
              item->attached(widget);
              item->refresh(widget);
              widget->setUpdatesEnabled(true);
              updatedCache[item->id()] = it->second;
              _widgetCache.erase(it->first);
            } else {
              for (const auto &[key, cache] : _widgetCache) {
                if (cache.recyclingId == item->recyclingId()) {
                  QWidget *widget = cache.widget->widget();

                  widget->setUpdatesEnabled(false);
                  item->attached(widget);
                  item->recycle(widget);
                  widget->setUpdatesEnabled(true);
                  updatedCache[item->id()] = cache;
                  _widgetCache.erase(key);
                  break;
                }
              }
            }
          }

          ++shownCount;
          m_items.push_back(vinfo);
        }
      }

      if (sctx) { yOffset += sctx->maxHeight; }
    }
  }

  if (!m_items.empty()) { yOffset += margins.bottom + margins.top; }

  for (auto &[key, cache] : _widgetCache) {
    if (cache.recyclingId) {
      moveToPool(cache.recyclingId, cache.widget);
    } else {
      cache.widget->deleteLater();
    }
  }

  _visibleWidgets.clear();
  _widgetCache = updatedCache;
  scrollBar->setMaximum(std::max(0, yOffset - height()));
  scrollBar->setMinimum(0);
  m_virtualHeight = yOffset;

  // timer.time("calculateHeights");

  updateVisibleItems();

  emit virtualHeightChanged(m_virtualHeight);
}

bool OmniList::isInViewport(const QRect &bounds) {
  int low = scrollBar->value();
  int high = low + height();

  return bounds.y() + bounds.height() >= low && bounds.y() <= high;
}

int OmniList::calculateItemHeight(const AbstractVirtualItem *item, int width) {
  if (!item->hasUniformHeight()) { return item->calculateHeight(width); }

  auto pred = [&](auto &&pair) { return pair.first == item->recyclingId(); };
  auto match = std::ranges::find_if(m_cachedHeights, pred);
  size_t typeId = item->recyclingId();

  if (match != m_cachedHeights.end()) { return match->second; }

  int height = item->calculateHeight(width);

  m_cachedHeights.push_back({typeId, height});

  return height;
}

std::vector<const OmniList::AbstractVirtualItem *> OmniList::items() const {
  std::vector<const AbstractVirtualItem *> items;

  items.reserve(m_items.size());

  for (const auto &item : m_items) {
    if (item.enumerable) items.emplace_back(item.item);
  }

  return items;
}

std::vector<const OmniList::AbstractVirtualItem *> OmniList::visibleItems() const {
  return ranges_to<std::vector>(
      m_items | std::views::drop(visibleIndexRange.start) | std::views::take(visibleIndexRange.size) |
      std::views::filter([](auto &&v) { return v.enumerable; }) |
      std::views::transform([](auto &&v) -> const AbstractVirtualItem * { return v.item; }));
}

void OmniList::setSelected(SelectionPolicy policy) {
  switch (policy) {
  case SelectFirst:
    selectFirst();
    break;
  case KeepSelection:
    qDebug() << "update with keep selection";
    if (m_selected == -1) {
      selectFirst();
    } else if (auto idx = indexOfItem(m_selectedId); idx != -1) {
      qCritical() << "not implemented yet";
      // qDebug() << "idx of " << _selectedId << _items[idx].vIndex;
      // setSelectedIndex(_items[idx].vIndex);
    } else {
      qDebug() << "no index for" << m_selectedId;
      setSelectedIndex(std::max(0, std::min(m_selected, static_cast<int>(m_items.size() - 1))));
    }
    break;
  case PreserveSelection: {
    int targetIndex = m_selected;
    int distance = 0;

    if (m_selected >= m_items.size()) { targetIndex = m_items.empty() ? -1 : 0; }

    for (;;) {
      int lowTarget = targetIndex - distance;
      int highTarget = targetIndex + distance;
      bool hasLower = lowTarget >= 0 && lowTarget < m_items.size();
      bool hasUpper = highTarget >= 0 && highTarget < m_items.size();

      if (!hasLower && !hasUpper) {
        setSelectedIndex(-1);
        return;
      }

      if (hasLower && m_items[lowTarget].item->selectable()) {
        setSelectedIndex(lowTarget);
        return;
      }

      if (hasUpper && m_items[highTarget].item->selectable()) {
        setSelectedIndex(highTarget);
        return;
      }

      ++distance;
    }
    break;
  }
  case SelectNone:
    setSelectedIndex(-1);
    break;
  }
}

void OmniList::endResetModel(OmniList::SelectionPolicy selectionPolicy) {
  calculateHeights();
  setSelected(selectionPolicy);
  emit modelChanged();
}

OmniListItemWidgetWrapper *OmniList::takeFromPool(size_t type) {
  if (auto it = _widgetPools.find(type); it != _widgetPools.end() && !it->second.empty()) {
    auto widget = it->second.top();

    it->second.pop();

    return widget;
  }

  return nullptr;
}

void OmniList::moveToPool(size_t type, OmniListItemWidgetWrapper *widget) {
  widget->hide();
  widget->setParent(nullptr);
  widget->blockSignals(true);
  widget->setUpdatesEnabled(false);
  _widgetPools[type].push(widget);
}

int OmniList::indexOfItem(const QString &id) const {
  // if (auto it = _idMap.find(id); it != _idMap.end()) return it->second;

  return -1;
}

const OmniList::AbstractVirtualItem *OmniList::firstSelectableItem() const {
  for (int i = 0; i < m_items.size(); ++i) {
    auto item = m_items[i].item;

    if (item->selectable()) { return item; }
  }

  return nullptr;
}

void OmniList::clearVisibleWidgets() {
  for (const auto &[_, cache] : _widgetCache) {
    cache.widget->deleteLater();
  }
  _visibleWidgets.clear();
  _widgetCache.clear();
}

bool OmniList::selectDown() {
  if (m_items.empty()) return false;
  if (m_selected == -1) {
    setSelectedIndex(0, ScrollBehaviour::ScrollRelative);
    return true;
  }

  auto &current = m_items[m_selected];
  int next = m_selected;

  while (next < m_items.size() &&
         (m_items[next].bounds.y() == current.bounds.y() || !m_items[next].item->selectable())) {
    ++next;
  }

  int endNext = next;

  while (endNext < m_items.size() && m_items[endNext].bounds.y() == m_items[next].bounds.y()) {
    ++endNext;
  }

  for (int i = endNext - 1; i >= next; --i) {
    auto &vItem = m_items[i];

    if (vItem.bounds.x() <= current.bounds.x() && vItem.item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
      return true;
    }
  }

  return false;
}

void OmniList::selectNext() {
  for (int i = m_selected + 1; i < m_items.size(); ++i) {
    if (m_items[i].item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
      return;
    }
  }

  selectFirst();
}

bool OmniList::selectUp() {
  if (m_items.empty()) return false;
  if (m_selected == -1) {
    m_selected = 0;
    return true;
  }

  auto &current = m_items[m_selected];

  for (int i = m_selected - 1; i >= 0; --i) {
    auto &vitem = m_items[i];

    if (vitem.bounds.y() < current.bounds.y() && vitem.bounds.x() <= current.bounds.x() &&
        vitem.item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
      return true;
    }
  };

  return false;
}

bool OmniList::selectLeft() {
  auto base = m_items[m_selected];
  int availableWidth = width() - margins.left - margins.right;

  for (int i = m_selected - 1; i >= 0; --i) {
    auto &vItem = m_items[i];

    if (!vItem.item->selectable()) { continue; }
    if (vItem.bounds.y() < base.bounds.y() && vItem.bounds.width() == base.bounds.width() &&
        base.bounds.width() == availableWidth) {
      return false;
    }

    setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
    return true;
  }

  return false;
}

bool OmniList::selectRight() {
  auto base = m_items[m_selected];
  int availableWidth = width() - margins.left - margins.right;

  for (int i = m_selected + 1; i < m_items.size(); ++i) {
    auto &vItem = m_items[i];

    if (!vItem.item->selectable()) { continue; }

    if (vItem.bounds.y() > base.bounds.y() && vItem.bounds.width() == base.bounds.width() &&
        base.bounds.width() == availableWidth)
      return false;

    setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
    return true;
  }

  return false;
}

bool OmniList::selectHome() {
  if (m_items.empty()) return false;

  // Find the first selectable item
  for (int i = 0; i < m_items.size(); ++i) {
    if (m_items[i].item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollAbsolute);
      return true;
    }
  }

  return false;
}

bool OmniList::selectEnd() {
  if (m_items.empty()) return false;

  // Find the last selectable item
  for (int i = m_items.size() - 1; i >= 0; --i) {
    if (m_items[i].item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollAbsolute);
      return true;
    }
  }

  return false;
}

void OmniList::setSelectedIndex(int index, ScrollBehaviour scrollBehaviour) {
  int oldIndex = m_selected;

  if (index != m_selected) { scrollTo(index, scrollBehaviour); }

  auto previous = m_selected >= 0 && m_selected < m_items.size() ? m_items.at(m_selected).item : nullptr;
  auto next = index >= 0 && index < m_items.size() ? m_items.at(index).item : nullptr;

  m_selected = index;
  updateVisibleItems();

  if (index == -1) {
    if (!m_selectedId.isEmpty()) {
      m_selectedId.clear();
      emit selectionChanged(next, previous);
    }
    return;
  }

  if (next && m_selectedId != next->id()) {
    // qDebug() << "seletion changed" << next->id() << "previous" << (previous ? previous->id() : "<none>");
    m_selectedId = next->id();
    emit selectionChanged(next, previous);
  }

  if (oldIndex != index) emit rowChanged(index);
}

int OmniList::previousRowIndex(int index) {
  auto &base = m_items[index];

  for (int i = index - 1; i >= 0; --i) {
    auto &info = m_items[i];

    if (info.bounds.y() < base.bounds.y()) { return i; }
  }

  return -1;
}

int OmniList::nextRowIndex(int index) {
  auto &base = m_items[index];

  for (int i = index + 1; i < m_items.size(); ++i) {
    auto &info = m_items[i];

    if (info.bounds.y() > base.bounds.y()) { return i; }
  }

  return -1;
}

void OmniList::scrollTo(int idx, ScrollBehaviour behaviour) {
  if (idx < 0 || idx >= m_items.size()) return;

  auto &item = m_items[idx];
  auto &bounds = item.bounds;

  int previousIdx = previousRowIndex(idx);

  /*
  if (previousIdx != -1 && m_items[previousIdx].item->isSection()) {
    return scrollTo(previousIdx, behaviour);
  }
  */

  int newScroll = scrollBar->value();

  if (behaviour == ScrollBehaviour::ScrollRelative) {
    int scrollHeight = scrollBar->value();

    // qDebug()
    //    << QString("%1 + %2 - %3 >
    //    %4").arg(bounds.y()).arg(bounds.height()).arg(scrollHeight).arg(height());

    if (bounds.y() + bounds.height() - scrollHeight > height()) {
      newScroll = (bounds.y() + bounds.height() - height());
    } else if (bounds.y() - scrollHeight < 0) {
      newScroll = (scrollHeight - (scrollHeight - bounds.y()));
    }
  }

  if (behaviour == ScrollBehaviour::ScrollAbsolute) { newScroll = bounds.y(); }

  if (previousIdx != -1 && m_items[previousIdx].item->isSection()) {
    auto &anchor = m_items[previousIdx];
    int low = newScroll;
    int high = low + height();
    bool isAnchorVisible = anchor.bounds.y() >= low && anchor.bounds.y() <= high;

    if (!isAnchorVisible) { return scrollTo(previousIdx, behaviour); }
  }

  scrollBar->setValue(newScroll);
}

void OmniList::activateCurrentSelection() const {
  if (!isSelectionValid()) return;

  auto &selected = m_items[m_selected].item;

  if (auto it = _widgetCache.find(selected->id()); it != _widgetCache.end()) {
    it->second.widget->widget()->activated();
  }

  emit itemActivated(*m_items[m_selected].item);
}

bool OmniList::event(QEvent *event) {
  if (event->type() == QEvent::Wheel) {
    QApplication::sendEvent(scrollBar, event);

    return true;
  }

  if (event->type() == QEvent::KeyPress) {
    auto kev = static_cast<QKeyEvent *>(event);

    switch (kev->key()) {
    case Qt::Key_Down:
      return selectDown();
    case Qt::Key_Up:
      return selectUp();
    case Qt::Key_Left:
      return selectLeft();
    case Qt::Key_Right:
      return selectRight();
    case Qt::Key_Home:
      return selectHome();
    case Qt::Key_End:
      return selectEnd();
    case Qt::Key_Return:
      activateCurrentSelection();
      return true;
    }
  }

  return QWidget::event(event);
}

void OmniList::recalculateMousePosition() {
  QPoint globalPos = QCursor::pos();

  for (const auto &wrapper : m_visibleWidgets) {
    QPoint localPos = wrapper->mapFromGlobal(globalPos);
    bool isUnderCursor = wrapper->rect().contains(localPos);

    if (!isUnderCursor) { wrapper->widget()->clearTransientState(); }

    if (wrapper->underMouse()) {
      wrapper->hide();
      wrapper->show();
    }
  }
}

void OmniList::resizeEvent(QResizeEvent *event) {
  auto size = event->size();

  QWidget::resizeEvent(event);
  scrollBar->setPageStep(size.height());
  scrollBar->setFixedHeight(size.height());
  scrollBar->move(size.width() - scrollBar->sizeHint().width(), 0);
  m_cachedHeights.clear();
  calculateHeights();
}

const OmniList::AbstractVirtualItem *OmniList::selected() const {
  if (m_selected >= 0 && m_selected < m_items.size()) return m_items[m_selected].item;

  return nullptr;
}

void OmniList::refresh() const {
  for (const auto &item : visibleItems()) {
    if (auto it = _widgetCache.find(item->id()); it != _widgetCache.end()) {
      item->refresh(it->second.widget->widget());
    }
  }
}

const OmniList::AbstractVirtualItem *OmniList::itemAt(const QString &id) const {
  for (const auto &entry : m_items) {
    if (entry.item->id() == id) return entry.item;
  }

  return nullptr;
}

OmniList::AbstractVirtualItem *OmniList::itemAt(const QString &id) {
  for (const auto &entry : m_items) {
    if (entry.item->id() == id) return entry.item;
  }

  return nullptr;
}

bool OmniList::selectFirst() {
  for (int i = 0; i < m_items.size(); ++i) {
    auto item = m_items[i].item;

    if (item->selectable()) {
      setSelectedIndex(i);
      scrollTo(i, ScrollBehaviour::ScrollAbsolute);
      return true;
    }
  }

  setSelectedIndex(-1);
  return false;
}

void OmniList::setMargins(int left, int top, int right, int bottom) { margins = {left, top, right, bottom}; }

void OmniList::setMargins(int value) { setMargins(value, value, value, value); }

void OmniList::clear() {
  m_model.clear();
  m_selected = DEFAULT_SELECTION_INDEX;
  m_virtualHeight = 0;

  calculateHeights();
}

bool OmniList::removeItem(const QString &id) {
  qCritical() << "remove item not implemented";

  return false;
}

bool OmniList::updateItem(const QString &id, const UpdateItemCallback &cb) {
  auto item = itemAt(id);

  if (!item) return false;

  cb(item);

  if (item->recyclable()) {
    if (auto it2 = _widgetCache.find(id); it2 != _widgetCache.end()) {
      item->recycle(it2->second.widget->widget());
    }
  }

  return true;
}

void OmniList::invalidateCache() {
  for (const auto &[id, cached] : _widgetCache) {
    auto item = itemAt(id);

    if (item->recyclable()) {
      moveToPool(item->recyclingId(), cached.widget);
    } else {
      cached.widget->deleteLater();
    }
  }

  _widgetCache.clear();
  _visibleWidgets.clear();

  // if (!_isUpdating) { updateVisibleItems(); }
}

void OmniList::invalidateCache(const QString &id) {
  auto it = _widgetCache.find(id);

  if (it == _widgetCache.end()) { return; }

  auto item = itemAt(id);

  if (item->recyclable()) {
    moveToPool(item->recyclingId(), it->second.widget);
  } else {
    it->second.widget->deleteLater();
  }

  _widgetCache.erase(it);
}

const OmniList::AbstractVirtualItem *OmniList::setSelected(const QString &id,
                                                           ScrollBehaviour scrollBehaviour) {
  for (int i = 0; i != m_items.size(); ++i) {
    if (m_items[i].item->id() == id) {
      setSelectedIndex(i, scrollBehaviour);

      return m_items[i].item;
    }
  }

  qWarning() << "setSelected called on id" << id << "does not match any valid list item";

  return nullptr;
}

void OmniList::clearWidgetPools() {
  for (auto [type, stack] : _widgetPools) {
    while (!stack.empty()) {
      stack.top()->deleteLater();
      stack.pop();
    }
  }
}

void OmniList::handleDebouncedScroll() { updateVisibleItems(); }

OmniList::OmniList() {
  scrollBar->setSingleStep(40);
  m_scrollTimer->setSingleShot(true);
  connect(scrollBar, &QScrollBar::valueChanged, this, [this]() {
    if (!m_scrollTimer->isActive()) { m_scrollTimer->start(16); }
  });
  connect(scrollBar, &QScrollBar::sliderReleased, this, [this]() { updateVisibleItems(); });
  connect(m_scrollTimer, &QTimer::timeout, this, [this]() { updateVisibleItems(); });

  int scrollBarWidth = scrollBar->sizeHint().width();

  setMargins(8, 5, 8, 5);
  _widgetCache.reserve(20);
  setMouseTracking(true);
}

OmniList::~OmniList() { clearWidgetPools(); }

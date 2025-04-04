#include "ui/omni-list.hpp"
#include "ui/list-section-header.hpp"
#include "ui/omni-list-item-widget-wrapper.hpp"
#include "ui/omni-scroll-bar.hpp"
#include <chrono>
#include <qabstractitemview.h>
#include <qapplication.h>
#include <qevent.h>
#include <qnamespace.h>

const QString &OmniList::VirtualSection::name() const { return _name; }

QString OmniList::VirtualSection::id() const { return name(); }

bool OmniList::VirtualSection::showHeader() { return count() > 0 && !_name.isEmpty(); }

int OmniList::VirtualSection::calculateItemWidth(int width, int index) const { return width; }

int OmniList::VirtualSection::calculateItemX(int x, int index) const { return x; }

void OmniList::VirtualSection::initWidth(int width) {}

void OmniList::VirtualSection::setCount(int count) { _count = count; }

int OmniList::VirtualSection::count() const { return _count; }

OmniListItemWidget *OmniList::VirtualSection::createWidget() const {
  return new OmniListSectionHeader(_name, "", _showCount ? count() : 0);
}

void OmniList::VirtualSection::setShowCount(bool value) { _showCount = value; }

int OmniList::VirtualSection::calculateHeight(int width) const {
  static OmniListSectionHeader ruler("", "", 0);

  return ruler.sizeHint().height();
}

OmniList::VirtualSection::VirtualSection(const QString &name, bool showCount)
    : _name(name), _count(0), _showCount(showCount) {}

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

void OmniList::itemDoubleClicked(int index) const { emit itemActivated(*vmap(index).item.get()); }

void OmniList::updateVisibleItems() {
  auto start = std::chrono::high_resolution_clock::now();
  if (_virtual_items.empty()) return;
  setUpdatesEnabled(false);

  int scrollHeight = scrollBar->value();
  int startIndex = 0;

  while (startIndex < _virtual_items.size() &&
         _virtual_items[startIndex].y + _virtual_items[startIndex].height < scrollHeight) {
    ++startIndex;
  }

  if (startIndex >= _virtual_items.size()) return;

  int marginOffset = std::max(0, margins.top - scrollHeight);
  int viewportY = marginOffset + _virtual_items[startIndex].y - scrollHeight;
  QSize viewportSize = size();
  int endIndex = startIndex;
  int lastY = -1;

  for (; endIndex < _virtual_items.size() && viewportY < viewportSize.height(); ++endIndex) {
    auto &vinfo = _virtual_items[endIndex];
    auto &info = vmap(endIndex);
    auto cacheIt = _widgetCache.find(info.id);
    OmniListItemWidgetWrapper *widget = nullptr;

    if (lastY != -1 && lastY != vinfo.y) { viewportY += vinfo.y - lastY; }

    bool isWidgetCreated = false;

    auto start = std::chrono::high_resolution_clock::now();
    if (cacheIt == _widgetCache.end()) {
      if (auto wrapper = takeFromPool(info.item->typeId())) {
        info.item->recycle(wrapper->widget());
        widget = wrapper;
      } else {
        widget = new OmniListItemWidgetWrapper(this);
        connect(widget, &OmniListItemWidgetWrapper::clicked, this, &OmniList::itemClicked);
        connect(widget, &OmniListItemWidgetWrapper::doubleClicked, this, &OmniList::itemDoubleClicked);
        widget->stackUnder(scrollBar);
        widget->setWidget(info.item->createWidget());
      }

      CachedWidget cache{.widget = widget, .recyclingId = 0};

      if (info.item->recyclable()) { cache.recyclingId = info.item->typeId(); }

      isWidgetCreated = true;
      _widgetCache[info.id] = cache;
    } else {
      widget = cacheIt->second.widget;
    }

    QPoint pos(vinfo.x, viewportY);
    QSize size(vinfo.width, vinfo.height);

    // qDebug() << "pos" << pos << "size" << size << info.item->id();

    widget->blockSignals(true);
    widget->setIndex(endIndex);
    widget->setSelected(endIndex == _selected);
    if (widget->size() != size) { widget->resize(size); }
    // widget->setFixedSize(size);
    widget->move(pos);
    widget->show();
    widget->blockSignals(false);

    lastY = vinfo.y;
    _visibleWidgets[endIndex] = widget;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6;
  }

  visibleIndexRange = {startIndex, endIndex - 1};

  for (auto it = _visibleWidgets.begin(); it != _visibleWidgets.end();) {
    auto current = it++;

    if (current->first < visibleIndexRange.lower || current->first > visibleIndexRange.upper) {
      auto &item = vmap(current->first);
      auto widget = current->second;

      if (item.item->recyclable()) {
        moveToPool(item.item->typeId(), widget);
      } else {
        widget->deleteLater();
      }

      _visibleWidgets.erase(current);
      _widgetCache.erase(item.id);
    }
  }

  setUpdatesEnabled(true);
  update();
}

bool OmniList::insertAtSectionStart(const QString &name, std::unique_ptr<AbstractVirtualItem> newItem) {
  // section lookup could be sped up using a static section map, but its rare remain rare enough
  // so that we don't really have to.

  for (const auto &item : _items) {
    if (item.item->isSection()) {
      auto section = static_cast<const VirtualSection *>(item.item.get());

      if (section->name() == name) { return insertAfter(section->id(), std::move(newItem)); }
    }
  }

  return false;
}

bool OmniList::insertAfter(const QString &id, std::unique_ptr<AbstractVirtualItem> item) {
  int index = indexOfItem(id);

  if (index == -1) return false;

  ListItemInfo info;

  info.filtered = _filter && item->isListItem() && _filter->matches(*item.get());
  info.cachedHeight = -1;
  info.id = item->id();
  info.item = std::move(item);

  int targetIndex = index + 1;

  for (int i = targetIndex; i < _items.size(); ++i) {
    _idMap[_items[i].item->id()] = i + 1;
  }

  _idMap[info.item->id()] = targetIndex;
  _items.insert(_items.begin() + targetIndex, std::move(info));

  if (!_isUpdating) { calculateHeights(); }
  if (_selected == targetIndex) { setSelectedIndex(targetIndex + 1); }

  return true;
}

bool OmniList::isShowingEmptyState() const { return _virtual_items.empty(); }

void OmniList::calculateHeights() {
  auto start = std::chrono::high_resolution_clock::now();
  int yOffset = 0;
  int availableWidth = width() - margins.left - margins.right;
  std::optional<SectionCalculationContext> sctx;
  std::unordered_map<QString, CachedWidget> updatedCache;

  _virtual_items.clear();

  for (int i = 0; i < _items.size(); ++i) {
    auto &item = _items[i];
    int role = item.item->role();

    item.vIndex = -1;

    if (role == AbstractVirtualItem::ListSection) {
      if (sctx) { yOffset += sctx->maxHeight; }

      int count = 0;
      auto section = static_cast<VirtualSection *>(item.item.get());
      int x = margins.left;
      int y = 0;

      for (int j = i + 1; j < _items.size() && !_items[j].item->isSection(); ++j) {
        if (!_items[j].filtered) ++count;
      }

      section->setCount(count);
      section->initWidth(availableWidth);

      if (section->showHeader()) {
        item.vIndex = _virtual_items.size();
        VirtualListWidgetInfo vinfo{.x = margins.left,
                                    .y = yOffset,
                                    .width = availableWidth,
                                    .height = item.item->calculateHeight(availableWidth),
                                    .index = i};
        _virtual_items.push_back(vinfo);
        yOffset += vinfo.height;
      }

      sctx = {.section = section, .x = margins.left};
    } else if (role == AbstractVirtualItem::ListItem && !item.filtered) {
      int height = 0;
      int width = availableWidth;
      int x = margins.left;
      int y = yOffset;

      item.vIndex = _virtual_items.size();

      if (item.vIndex >= visibleIndexRange.lower && item.vIndex <= visibleIndexRange.upper) {
        if (auto it = _widgetCache.find(item.id); it != _widgetCache.end()) {
          updatedCache[item.id] = it->second;
        }
      }

      if (sctx) {
        width = sctx->section->calculateItemWidth(width, sctx->index);
        // x = sctx->section->calculateItemX(sctx->x, sctx->index);
        if (sctx->x == margins.left && sctx->index > 0) {
          yOffset += sctx->section->spacing();
          y = yOffset;
        }

        x = sctx->x;
        sctx->x = sctx->x + width + sctx->section->spacing();
        height = item.item->calculateHeight(width);
        sctx->maxHeight = std::max(sctx->maxHeight, height);

        if (sctx->x >= availableWidth) {
          yOffset += sctx->maxHeight;
          sctx->x = margins.left;
          sctx->maxHeight = 0;
        }

        ++sctx->index;
      } else {
        height = item.item->calculateHeight(width);
        yOffset += height;
      }

      VirtualListWidgetInfo vinfo{.x = x, .y = y, .width = width, .height = height, .index = i};

      _virtual_items.push_back(vinfo);
    }
  }

  if (sctx) { yOffset += sctx->maxHeight; }

  yOffset += margins.bottom;

  for (auto &[key, cache] : _widgetCache) {
    if (auto it = updatedCache.find(key); it == updatedCache.end()) {
      if (cache.recyclingId) {
        moveToPool(cache.recyclingId, cache.widget);
      } else {
        cache.widget->deleteLater();
      }
    }
  }

  _visibleWidgets.clear();
  _widgetCache = updatedCache;

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  qDebug() << "calculateHeights() took" << duration << "ms";

  scrollBar->setMaximum(std::max(0, yOffset - height()));
  scrollBar->setMinimum(0);
  _virtualHeight = yOffset;
  updateVisibleItems();
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
  _widgetPools[type].push(widget);
}

int OmniList::indexOfItem(const QString &id) const {
  if (auto it = _idMap.find(id); it != _idMap.end()) return it->second;

  return -1;
}

void OmniList::updateFromList(std::vector<std::unique_ptr<AbstractVirtualItem>> &nextList,
                              SelectionPolicy selectionPolicy) {
  _items.clear();
  _virtual_items.clear();
  _idMap.clear();

  beginUpdate();

  for (int i = 0; i != nextList.size(); ++i) {
    auto &item = nextList[i];

    addItem(std::move(item));
  }

  commitUpdate();

  switch (selectionPolicy) {
  case SelectFirst:
    qDebug() << "update with select first";
    selectFirst();
    break;
  case KeepSelection:
    qDebug() << "update with keep selection";
    if (_selected == -1) {
      selectFirst();
    } else if (auto idx = indexOfItem(_selectedId); idx != -1) {
      qDebug() << "idx of " << _selectedId << _items[idx].vIndex;
      setSelectedIndex(_items[idx].vIndex);
    } else {
      setSelectedIndex(std::max(0, std::min(_selected, static_cast<int>(_virtual_items.size() - 1))));
    }
    break;
  case SelectNone:
    setSelectedIndex(-1);
    break;
  }
}

void OmniList::clearVisibleWidgets() {
  for (const auto &[_, cache] : _widgetCache) {
    cache.widget->deleteLater();
  }
  _visibleWidgets.clear();
  _widgetCache.clear();
}

bool OmniList::selectDown() {
  if (_virtual_items.empty()) return false;
  if (_selected == -1) {
    _selected = 0;
    return true;
  }

  auto &current = _virtual_items[_selected];
  int next = _selected;

  while (next < _virtual_items.size() &&
         (_virtual_items[next].y == current.y || !vmap(next).item->selectable())) {
    ++next;
  }

  int endNext = next;

  while (endNext < _virtual_items.size() && _virtual_items[endNext].y == _virtual_items[next].y) {
    ++endNext;
  }

  for (int i = endNext - 1; i >= next; --i) {
    auto &vItem = _virtual_items[i];
    auto &item = _items[vItem.index];

    if (vItem.x <= current.x && item.item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
      return true;
    }
  }

  return false;
}

bool OmniList::selectUp() {
  if (_virtual_items.empty()) return false;
  if (_selected == -1) {
    _selected = 0;
    return true;
  }

  auto &current = _virtual_items[_selected];

  for (int i = _selected - 1; i >= 0; --i) {
    auto &vitem = _virtual_items[i];
    auto &item = _items[vitem.index];

    if (vitem.y < current.y && vitem.x <= current.x && item.item->selectable()) {
      setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
      return true;
    }
  };

  return false;
}

bool OmniList::selectLeft() {
  auto base = _virtual_items[_selected];
  int availableWidth = width() - margins.left - margins.right;

  for (int i = _selected - 1; i >= 0; --i) {
    auto &vItem = _virtual_items[i];

    if (!vmap(i).item->selectable()) { continue; }
    if (vItem.y < base.y && vItem.width == base.width && base.width == availableWidth) { return false; }

    setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
    return true;
  }

  return false;
}

bool OmniList::selectRight() {
  auto base = _virtual_items[_selected];
  int availableWidth = width() - margins.left - margins.right;

  for (int i = _selected + 1; i < _virtual_items.size(); ++i) {
    auto &vItem = _virtual_items[i];

    if (!vmap(i).item->selectable()) { continue; }

    if (vItem.y > base.y && vItem.width == base.width && base.width == availableWidth) return false;

    setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
    return true;
  }

  return false;
}

void OmniList::setSelectedIndex(int index, ScrollBehaviour scrollBehaviour) {
  scrollTo(index, scrollBehaviour);

  qDebug() << "set selected index" << index;

  auto previous = _selected >= 0 && _selected < _virtual_items.size() ? vmap(_selected).item.get() : nullptr;
  auto next = index >= 0 && index < _virtual_items.size() ? vmap(index).item.get() : nullptr;

  _selected = index;
  updateVisibleItems();

  if (index == -1) {
    if (!_selectedId.isEmpty()) {
      _selectedId.clear();
      emit selectionChanged(next, previous);
    }
    return;
  }

  if (next && _selectedId != next->id()) {
    _selectedId = next->id();
    emit selectionChanged(next, previous);
  }
}

int OmniList::previousRowIndex(int index) {
  auto &base = _virtual_items[index];

  for (int i = index - 1; i >= 0; --i) {
    auto &info = _virtual_items[i];

    if (info.y < base.y) { return i; }
  }

  return -1;
}

int OmniList::nextRowIndex(int index) {
  auto &base = _virtual_items[index];

  for (int i = index + 1; i < _virtual_items.size(); ++i) {
    auto &info = _virtual_items[i];

    if (info.y > base.y) { return i; }
  }

  return -1;
}

OmniList::ListItemInfo &OmniList::vmap(int vindex) { return _items.at(_virtual_items.at(vindex).index); }
const OmniList::ListItemInfo &OmniList::vmap(int vindex) const {
  return _items.at(_virtual_items.at(vindex).index);
}

void OmniList::scrollTo(int idx, ScrollBehaviour behaviour) {
  if (idx < 0 || idx >= _virtual_items.size()) return;

  auto &item = _virtual_items[idx];

  int previousIdx = previousRowIndex(idx);

  if (previousIdx != -1 && _items[previousIdx].item->isSection()) { return scrollTo(previousIdx, behaviour); }

  if (behaviour == ScrollBehaviour::ScrollRelative) {
    int scrollHeight = scrollBar->value();

    if (item.y + item.height - scrollHeight > height()) {
      scrollBar->setValue(item.y + item.height - height());
    } else if (item.y - scrollHeight < 0) {
      scrollBar->setValue(scrollHeight - (scrollHeight - item.y));
    }
  }

  if (behaviour == ScrollBehaviour::ScrollAbsolute) { scrollBar->setValue(item.y); }
}

void OmniList::activateCurrentSelection() const {
  if (!isSelectionValid()) return;

  qDebug() << "activate selected=" << _selected;

  emit itemActivated(*vmap(_selected).item.get());
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
    case Qt::Key_Return:
      activateCurrentSelection();
      return true;
    }
  }

  return QWidget::event(event);
}

void OmniList::resizeEvent(QResizeEvent *event) {
  auto size = event->size();

  scrollBar->setPageStep(size.height());
  scrollBar->setFixedHeight(size.height());
  scrollBar->move(size.width() - scrollBar->sizeHint().width(), 0);
  calculateHeights();
  QWidget::resizeEvent(event);
}

void OmniList::addSection(const QString &name) { addItem(std::make_unique<OmniList::VirtualSection>(name)); }

void OmniList::beginUpdate() { _isUpdating = true; }

void OmniList::commitUpdate() {
  _isUpdating = false;
  calculateHeights();
}

const OmniList::AbstractVirtualItem *OmniList::selected() const {
  if (_selected >= 0 && _selected < _virtual_items.size()) return vmap(_selected).item.get();

  return nullptr;
}

void OmniList::clearFilter() {
  if (!_filter) return;

  _filter.reset();

  for (auto &info : _items) {
    info.filtered = false;
  }

  if (!_isUpdating) calculateHeights();
}

const OmniList::AbstractItemFilter *OmniList::filter() const { return _filter.get(); }

void OmniList::setFilter(std::unique_ptr<AbstractItemFilter> filter) {
  for (int i = 0; i != _items.size(); ++i) {
    auto &info = _items[i];

    info.filtered = info.item->isListItem() && !filter->matches(*info.item.get());
  }

  _selected = -1;
  _filter = std::move(filter);
  calculateHeights();
  selectFirst();
  scrollBar->setValue(0);
}

const OmniList::AbstractVirtualItem *OmniList::itemAt(const QString &id) const {
  int idx = indexOfItem(id);

  if (idx == -1) return nullptr;

  return _items[idx].item.get();
}

bool OmniList::selectFirst() {
  for (int i = 0; i < _virtual_items.size(); ++i) {
    auto &item = vmap(i).item;

    if (item->selectable()) {
      setSelectedIndex(i);
      return true;
    }
  }

  setSelectedIndex(-1);
  return false;
}

void OmniList::setMargins(int left, int top, int right, int bottom) {
  margins = {left, top, right, bottom};
  if (!_isUpdating) { calculateHeights(); }
}

void OmniList::setMargins(int value) { setMargins(value, value, value, value); }

void OmniList::clear() {
  _items.clear();
  _idMap.clear();
  _isUpdating = false;
  _filter.reset();
  _selected = DEFAULT_SELECTION_INDEX;
  _virtualHeight = 0;

  calculateHeights();
}

bool OmniList::removeItem(const QString &id) {
  qDebug() << "remove id" << id;
  int index = indexOfItem(id);

  if (index == -1) { return false; };
  if (auto it = _idMap.find(_items[index].item->id()); it != _idMap.end()) {
    bool shouldRecalculate = !_items[index].filtered;

    qDebug() << "remove item at index" << index;

    for (int i = index + 1; i < _items.size(); ++i) {
      _idMap[_items[i].item->id()] -= 1;
    }

    _items.erase(_items.begin() + index);
    _idMap.erase(it);

    // if erased item is filtered there is no need to recompute
    if (shouldRecalculate) {
      calculateHeights();
      if (_selected >= _virtual_items.size()) {
        setSelectedIndex(_virtual_items.size() - 1);
      } else {
        setSelectedIndex(_selected);
      }
    }
  }

  return true;
}

bool OmniList::updateItem(const QString &id, const UpdateItemCallback &cb) {
  int idx = indexOfItem(id);

  if (idx == -1) { return false; }

  auto &info = _items[idx];

  qDebug() << "idx update" << idx;

  cb(info.item.get());

  if (auto it = _visibleWidgets.find(info.vIndex); it != _visibleWidgets.end()) { _visibleWidgets.erase(it); }

  if (auto it = _widgetCache.find(info.id); it != _widgetCache.end()) {
    it->second.widget->deleteLater();
    _widgetCache.erase(it);
  }

  if (!_isUpdating) { updateVisibleItems(); };

  emit itemUpdated(*info.item.get());

  return true;
}

void OmniList::invalidateCache() {
  for (const auto &[id, cached] : _widgetCache) {
    auto item = itemAt(id);

    if (item->recyclable()) {
      moveToPool(item->typeId(), cached.widget);
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

  auto idx = indexOfItem(id);
  auto &info = _items.at(idx);

  if (info.item->recyclable()) {
    moveToPool(info.item->typeId(), it->second.widget);
  } else {
    it->second.widget->deleteLater();
  }

  _widgetCache.erase(it);

  if (info.vIndex != -1) { _visibleWidgets.erase(info.vIndex); }

  if (!_isUpdating) { updateVisibleItems(); }
}

const OmniList::AbstractVirtualItem *OmniList::setSelected(const QString &id,
                                                           ScrollBehaviour scrollBehaviour) {
  int idx = indexOfItem(id);

  if (idx == -1) return nullptr;

  auto &item = _items.at(idx);

  setSelectedIndex(item.vIndex, scrollBehaviour);

  return item.item.get();
}

void OmniList::addItem(std::unique_ptr<AbstractVirtualItem> item) {
  ListItemInfo info;

  info.filtered = _filter && item->isListItem() && !_filter->matches(*item.get());
  info.cachedHeight = -1;
  info.id = item->id();
  info.item = std::move(item);

  _idMap.insert({info.item->id(), _items.size()});
  _items.push_back(std::move(info));

  if (!_isUpdating) { calculateHeights(); }
}

OmniList::OmniList()
    : scrollBar(new OmniScrollBar(this)), _selected(DEFAULT_SELECTION_INDEX), _isUpdating(false),
      margins({0, 0, 0, 0}) {
  scrollBar->setSingleStep(40);
  connect(scrollBar, &QScrollBar::valueChanged, this, [this]() { updateVisibleItems(); });

  int scrollBarWidth = scrollBar->sizeHint().width();

  setMargins(5, 5, 5, 5);
  _virtual_items.reserve(100);
  _items.reserve(100);
  _visibleWidgets.reserve(100);
  _widgetCache.reserve(100);
}

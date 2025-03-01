#include "ui/omni-list.hpp"
#include "ui/omni-list-item-widget-wrapper.hpp"
#include "ui/omni-scroll-bar.hpp"
#include <qapplication.h>
#include <qevent.h>

void OmniList::itemClicked(int index) { setSelectedIndex(index); }

void OmniList::itemDoubleClicked(int index) const { emit itemActivated(*vmap(index).item.get()); }

void OmniList::updateVisibleItems() {
  if (_virtual_items.empty()) return;

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

  for (; endIndex < _virtual_items.size() && viewportY < viewportSize.height(); ++endIndex) {
    auto &vinfo = _virtual_items[endIndex];
    auto &info = vmap(endIndex);
    auto widget = _widgetCache[info.id];

    if (!widget) {
      if (auto wrapper = takeFromPool(info.item->typeId())) {
        qDebug() << "recycled";
        info.item->recycle(wrapper->widget());
        wrapper->setParent(this);
        widget = wrapper;
      } else {
        widget = new OmniListItemWidgetWrapper(this);
        connect(widget, &OmniListItemWidgetWrapper::clicked, this, &OmniList::itemClicked);
        connect(widget, &OmniListItemWidgetWrapper::doubleClicked, this, &OmniList::itemDoubleClicked);
        widget->setWidget(info.item->createWidget());
      }

      _widgetCache[info.id] = widget;
    }

    QPoint pos(vinfo.x, viewportY);
    QSize size(vinfo.width, vinfo.height);

    // qDebug() << info.id << pos << size;

    widget->setIndex(endIndex);
    widget->setSelected(endIndex == _selected);
    widget->setFixedSize(size);
    widget->stackUnder(scrollBar);
    widget->move(pos);
    widget->show();
    viewportY += vinfo.height;
    _visibleWidgets[endIndex] = widget;
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
}

void OmniList::calculateHeights() {
  auto start = std::chrono::high_resolution_clock::now();
  int yOffset = 0;
  int availableWidth = width() - margins.left - margins.right;
  std::optional<SectionCalculationContext> sctx;
  std::unordered_map<QString, OmniListItemWidgetWrapper *> updatedCache;

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

      if (section->showHeader()) {
        item.vIndex = _virtual_items.size();
        VirtualListWidgetInfo vinfo{.x = margins.left,
                                    .y = yOffset,
                                    .width = availableWidth,
                                    .height = item.item->calculateHeight(),
                                    .index = i};
        _virtual_items.push_back(vinfo);
        yOffset += vinfo.height;
      }

      sctx = {.section = section, .x = margins.left};
    } else if (role == AbstractVirtualItem::ListItem && !item.filtered) {
      int height = item.item->calculateHeight();
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
        width = sctx->section->computeWidth(width, sctx->index);
        x = sctx->x;
        sctx->x += width;
        sctx->maxHeight = std::max(sctx->maxHeight, height);

        if (sctx->x >= availableWidth) {
          yOffset += sctx->maxHeight;
          sctx->x = margins.left;
          sctx->maxHeight = 0;
        }

        ++sctx->index;
      } else {
        yOffset += height;
      }

      VirtualListWidgetInfo vinfo{.x = x, .y = y, .width = width, .height = height, .index = i};

      _virtual_items.push_back(vinfo);
    }
  }

  if (sctx) { yOffset += sctx->maxHeight; }

  yOffset += margins.bottom;

  for (auto &[key, widget] : _widgetCache) {
    if (auto it = updatedCache.find(key); it == updatedCache.end()) { widget->deleteLater(); }
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
  widget->setParent(nullptr);
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
    selectFirst();
    break;
  case KeepSelection:
    setSelectedIndex(_selected);
    break;
  case SelectNone:
    setSelectedIndex(-1);
    break;
  }
}

void OmniList::clearVisibleWidgets() {
  for (const auto &[_, widget] : _widgetCache) {
    widget->deleteLater();
  }
  _visibleWidgets.clear();
  _widgetCache.clear();
}

void OmniList::selectDown() {
  if (_virtual_items.empty()) return;
  if (_selected == -1) {
    _selected = 0;
    return;
  }

  auto &current = _virtual_items[_selected];
  auto nextSelection = _selected + 1;

  while (nextSelection < _virtual_items.size()) {
    auto &info = _virtual_items[nextSelection];

    if (info.y > current.y && _items[info.index].item->selectable()) {
      setSelectedIndex(nextSelection, ScrollBehaviour::ScrollRelative);
      return;
    }

    ++nextSelection;
  }
}

void OmniList::selectUp() {
  if (_virtual_items.empty()) return;
  if (_selected == -1) {
    _selected = 0;
    return;
  }

  auto &current = _virtual_items[_selected];
  int nextSelection = _selected - 1;

  while (nextSelection >= 0) {
    auto &info = _virtual_items[nextSelection];

    if (info.y < current.y && _items[info.index].item->selectable()) {
      setSelectedIndex(nextSelection, ScrollBehaviour::ScrollRelative);
      return;
    }

    --nextSelection;
  }
}

void OmniList::setSelectedIndex(int index, ScrollBehaviour scrollBehaviour) {
  scrollTo(index, scrollBehaviour);

  qDebug() << "set selected index" << index;

  if (index == -1) return;

  auto previous = _selected >= 0 && _selected < _virtual_items.size() ? vmap(_selected).item.get() : nullptr;
  auto next = index >= 0 && index < _virtual_items.size() ? vmap(index).item.get() : nullptr;

  _selected = index;
  updateVisibleItems();

  if (_selectedId != next->id()) {
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
      selectDown();
      return true;
    case Qt::Key_Up:
      selectUp();
      return true;
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
  if (_selected >= 0 && _selected < _items.size()) return _items[_selected].item.get();

  return nullptr;
}

void OmniList::clearFilter() {
  _filter.reset();

  for (auto &info : _items) {
    info.filtered = false;
  }

  if (!_isUpdating) calculateHeights();
}

const OmniList::ItemFilter *OmniList::filter() const { return _filter.get(); }

void OmniList::setFilter(std::unique_ptr<ItemFilter> filter) {
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
    it->second->deleteLater();
    _widgetCache.erase(it);
  }

  if (!_isUpdating) { updateVisibleItems(); };

  emit itemUpdated(*info.item.get());

  return true;
}

void OmniList::invalidateCache() {
  for (const auto &[id, widget] : _widgetCache) {
    auto item = itemAt(id);

    if (item->recyclable()) {
      moveToPool(item->typeId(), widget);
    } else {
      widget->deleteLater();
    }
  }

  _widgetCache.clear();
  _visibleWidgets.clear();

  if (!_isUpdating) { updateVisibleItems(); }
}

void OmniList::invalidateCache(const QString &id) {
  auto it = _widgetCache.find(id);

  if (it == _widgetCache.end()) { return; }

  auto idx = indexOfItem(id);
  auto &info = _items.at(idx);

  if (info.item->recyclable()) {
    moveToPool(info.item->typeId(), it->second);
  } else {
    it->second->deleteLater();
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

  info.filtered = _filter && item->isListItem() && _filter->matches(*item.get());
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
}

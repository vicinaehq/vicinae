#include "ui/omni-list.hpp"
#include "ui/list-section-header.hpp"
#include "ui/omni-list-item-widget-wrapper.hpp"
#include "ui/omni-scroll-bar.hpp"
#include <algorithm>
#include <chrono>
#include <qabstractitemview.h>
#include <qapplication.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>

QString OmniList::VirtualSection::generateId() const { return _name; }

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

void OmniList::itemDoubleClicked(int index) const { emit itemActivated(*_virtual_items[index].item); }

void OmniList::updateVisibleItems() {
  auto start = std::chrono::high_resolution_clock::now();
  if (_virtual_items.empty()) return;
  setUpdatesEnabled(false);

  int scrollHeight = scrollBar->value();
  int startIndex = 0;

  // XXX - use binsearch to figure out starting point, or infer using last update and scroll direction
  while (startIndex < _virtual_items.size() &&
         _virtual_items[startIndex].geometry.y() + _virtual_items[startIndex].geometry.height() <
             scrollHeight) {
    ++startIndex;
  }

  if (startIndex >= _virtual_items.size()) return;

  auto &cell = _virtual_items[startIndex];
  int marginOffset = std::max(0, margins.top - scrollHeight);
  int viewportY = marginOffset + cell.geometry.y() - scrollHeight;
  QSize viewportSize = size();
  int endIndex = startIndex;
  int lastY = -1;

  while (endIndex < _virtual_items.size()) {
    auto &vinfo = _virtual_items[endIndex];
    auto cacheIt = _widgetCache.find(vinfo.item->id());
    OmniListItemWidgetWrapper *widget = nullptr;

    if (lastY != -1 && lastY != vinfo.geometry.y()) { viewportY += vinfo.geometry.y() - lastY; }

    if (viewportY >= viewportSize.height()) break;

    bool isWidgetCreated = false;

    auto start = std::chrono::high_resolution_clock::now();
    if (cacheIt == _widgetCache.end()) {
      if (auto wrapper = takeFromPool(vinfo.item->typeId())) {
        vinfo.item->recycle(wrapper->widget());
        wrapper->blockSignals(false);
        widget = wrapper;
      } else {
        widget = new OmniListItemWidgetWrapper(this);
        connect(widget, &OmniListItemWidgetWrapper::clicked, this, &OmniList::itemClicked,
                Qt::UniqueConnection);
        connect(widget, &OmniListItemWidgetWrapper::doubleClicked, this, &OmniList::itemDoubleClicked,
                Qt::UniqueConnection);
        widget->stackUnder(scrollBar);
        widget->setWidget(vinfo.item->createWidget());
      }

      CachedWidget cache{.widget = widget, .recyclingId = 0};

      if (vinfo.item->recyclable()) { cache.recyclingId = vinfo.item->typeId(); }

      isWidgetCreated = true;
      _widgetCache[vinfo.item->id()] = cache;
    } else {
      widget = cacheIt->second.widget;
    }

    QPoint pos(vinfo.geometry.x(), viewportY);
    QSize size(vinfo.geometry.width(), vinfo.geometry.height());

    qDebug() << "pos" << pos << "size" << size << "index" << endIndex << vinfo.item->id();

    widget->blockSignals(true);
    widget->setIndex(endIndex);
    widget->setSelected(endIndex == _selected);
    if (widget->size() != size) { widget->resize(size); }
    // widget->setFixedSize(size);
    widget->move(pos);
    widget->show();
    widget->blockSignals(false);

    lastY = vinfo.geometry.y();
    _visibleWidgets[endIndex] = widget;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6;
    ++endIndex;
  }

  visibleIndexRange = {startIndex, endIndex - 1};

  for (auto it = _visibleWidgets.begin(); it != _visibleWidgets.end();) {
    auto current = it++;

    if (current->first < visibleIndexRange.lower || current->first > visibleIndexRange.upper) {
      auto item = _virtual_items[current->first].item;
      auto widget = current->second;

      if (item->recyclable()) {
        moveToPool(item->typeId(), widget);
      } else {
        widget->deleteLater();
      }

      _visibleWidgets.erase(current);
      _widgetCache.erase(item->id());
    }
  }

  setUpdatesEnabled(true);
  update();
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
  widget->blockSignals(true);
  _widgetPools[type].push(widget);
}

int OmniList::indexOfItem(const QString &id) const {
  // if (auto it = _idMap.find(id); it != _idMap.end()) return it->second;

  return -1;
}

const OmniList::AbstractVirtualItem *OmniList::firstSelectableItem() const {
  for (int i = 0; i < _virtual_items.size(); ++i) {
    auto item = _virtual_items[i].item;

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
  if (_virtual_items.empty()) return false;
  if (_selected == -1) {
    setSelectedIndex(0, ScrollBehaviour::ScrollRelative);
    return true;
  }

  auto &current = _virtual_items[_selected];
  int next = _selected;

  while (next < _virtual_items.size() && (_virtual_items[next].geometry.y() == current.geometry.y() ||
                                          !_virtual_items[next].item->selectable())) {
    ++next;
  }

  int endNext = next;

  while (endNext < _virtual_items.size() &&
         _virtual_items[endNext].geometry.y() == _virtual_items[next].geometry.y()) {
    ++endNext;
  }

  for (int i = endNext - 1; i >= next; --i) {
    auto &vItem = _virtual_items[i];

    if (vItem.geometry.x() <= current.geometry.x() && vItem.item->selectable()) {
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

    if (vitem.geometry.y() < current.geometry.y() && vitem.geometry.x() <= current.geometry.x() &&
        vitem.item->selectable()) {
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

    if (!vItem.item->selectable()) { continue; }
    if (vItem.geometry.y() < base.geometry.y() && vItem.geometry.width() == base.geometry.width() &&
        base.geometry.width() == availableWidth) {
      return false;
    }

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

    if (!vItem.item->selectable()) { continue; }

    if (vItem.geometry.y() > base.geometry.y() && vItem.geometry.width() == base.geometry.width() &&
        base.geometry.width() == availableWidth)
      return false;

    setSelectedIndex(i, ScrollBehaviour::ScrollRelative);
    return true;
  }

  return false;
}

void OmniList::setSelectedIndex(int index, ScrollBehaviour scrollBehaviour) {
  int oldIndex = _selected;

  if (index != _selected) { scrollTo(index, scrollBehaviour); }

  // qDebug() << "set selected index" << index;

  auto previous =
      _selected >= 0 && _selected < _virtual_items.size() ? _virtual_items.at(_selected).item : nullptr;
  auto next = index >= 0 && index < _virtual_items.size() ? _virtual_items.at(index).item : nullptr;

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
    // qDebug() << "seletion changed" << next->id() << "previous" << (previous ? previous->id() : "<none>");
    _selectedId = next->id();
    emit selectionChanged(next, previous);
  }

  if (oldIndex != index) emit rowChanged(index);
}

int OmniList::previousRowIndex(int index) {
  auto &base = _virtual_items[index];

  for (int i = index - 1; i >= 0; --i) {
    auto &info = _virtual_items[i];

    if (info.geometry.y() < base.geometry.y()) { return i; }
  }

  return -1;
}

int OmniList::nextRowIndex(int index) {
  auto &base = _virtual_items[index];

  for (int i = index + 1; i < _virtual_items.size(); ++i) {
    auto &info = _virtual_items[i];

    if (info.geometry.y() > base.geometry.y()) { return i; }
  }

  return -1;
}

void OmniList::scrollTo(int idx, ScrollBehaviour behaviour) {
  if (idx < 0 || idx >= _virtual_items.size()) return;

  auto &item = _virtual_items[idx];
  auto &bounds = item.geometry;

  int previousIdx = previousRowIndex(idx);

  if (previousIdx != -1 && _virtual_items[previousIdx].item->isSection()) {
    return scrollTo(previousIdx, behaviour);
  }

  if (behaviour == ScrollBehaviour::ScrollRelative) {
    int scrollHeight = scrollBar->value();

    if (bounds.y() + bounds.height() - scrollHeight > height()) {
      scrollBar->setValue(bounds.y() + bounds.height() - height());
    } else if (bounds.y() - scrollHeight < 0) {
      scrollBar->setValue(scrollHeight - (scrollHeight - bounds.y()));
    }
  }

  if (behaviour == ScrollBehaviour::ScrollAbsolute) { scrollBar->setValue(bounds.y()); }
}

void OmniList::activateCurrentSelection() const {
  if (!isSelectionValid()) return;

  emit itemActivated(*_virtual_items[_selected].item);
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

  qDebug() << "LIST RESIZED =>" << size;

  QWidget::resizeEvent(event);
  scrollBar->setPageStep(size.height());
  scrollBar->setFixedHeight(size.height());
  scrollBar->move(size.width() - scrollBar->sizeHint().width(), 0);
  m_cachedHeights.clear();
  calculateHeights();
}

const OmniList::AbstractVirtualItem *OmniList::selected() const {
  if (_selected >= 0 && _selected < _virtual_items.size()) return _virtual_items[_selected].item;

  return nullptr;
}

void OmniList::clearFilter() {
  if (!_filter) return;

  _filter.reset();
  calculateHeights();
}

const OmniList::AbstractItemFilter *OmniList::filter() const { return _filter.get(); }

void OmniList::setFilter(std::unique_ptr<AbstractItemFilter> filter) {
  if (!m_model.empty()) {
    _selected = -1;
    _filter = std::move(filter);
    calculateHeights();
    selectFirst();
    scrollBar->setValue(0);
    return;
  }

  _selected = -1;
  _filter = std::move(filter);
  calculateHeights();
  selectFirst();
  scrollBar->setValue(0);
}

const OmniList::AbstractVirtualItem *OmniList::itemAt(const QString &id) const {
  if (auto it = _idItemMap.find(id); it != _idItemMap.end()) return it->second;

  return nullptr;
}

bool OmniList::selectFirst() {
  for (int i = 0; i < _virtual_items.size(); ++i) {
    auto item = _virtual_items[i].item;

    if (item->selectable()) {
      setSelectedIndex(i);
      scrollTo(i);
      return true;
    }
  }

  setSelectedIndex(-1);
  return false;
}

void OmniList::setMargins(int left, int top, int right, int bottom) { margins = {left, top, right, bottom}; }

void OmniList::setMargins(int value) { setMargins(value, value, value, value); }

void OmniList::clear() {
  _idItemMap.clear();
  m_model.clear();
  _filter.reset();
  _selected = DEFAULT_SELECTION_INDEX;
  _virtualHeight = 0;

  calculateHeights();
}

bool OmniList::removeItem(const QString &id) {
  qCritical() << "remove item not implemented";

  return false;
}

bool OmniList::updateItem(const QString &id, const UpdateItemCallback &cb) {
  if (auto it = _idItemMap.find(id); it != _idItemMap.end()) {
    cb(it->second);

    if (it->second->recyclable()) {
      if (auto it2 = _widgetCache.find(id); it2 != _widgetCache.end()) {
        it->second->recycle(it2->second.widget->widget());
      }
    }

    return true;
  }

  return false;
}

void OmniList::invalidateCache() {
  for (const auto &[id, cached] : _widgetCache) {
    auto item = itemAt(id);

    qDebug() << "looking for id" << id;

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

  auto itemIt = _idItemMap.find(id);

  if (itemIt == _idItemMap.end()) return;

  auto item = itemIt->second;

  if (item->recyclable()) {
    moveToPool(item->typeId(), it->second.widget);
  } else {
    it->second.widget->deleteLater();
  }

  _widgetCache.erase(it);

  // if (info.vIndex != -1) { _visibleWidgets.erase(info.vIndex); }
}

const OmniList::AbstractVirtualItem *OmniList::setSelected(const QString &id,
                                                           ScrollBehaviour scrollBehaviour) {
  for (int i = 0; i != _virtual_items.size(); ++i) {
    if (_virtual_items[i].item->id() == id) {
      setSelectedIndex(i, scrollBehaviour);

      return _virtual_items[i].item;
    }
  }

  return nullptr;
}

OmniList::OmniList()
    : scrollBar(new OmniScrollBar(this)), _selected(DEFAULT_SELECTION_INDEX), margins({0, 0, 0, 0}) {
  scrollBar->setSingleStep(40);
  connect(scrollBar, &QScrollBar::valueChanged, this, [this]() { updateVisibleItems(); });

  int scrollBarWidth = scrollBar->sizeHint().width();

  setMargins(5, 5, 5, 5);
  _visibleWidgets.reserve(20);
  _widgetCache.reserve(20);
}

OmniList::~OmniList() {}

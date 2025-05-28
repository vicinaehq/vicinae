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

const QString &OmniList::VirtualSection::name() const { return _name; }

QString OmniList::VirtualSection::generateId() const { return name(); }

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

void OmniList::itemDoubleClicked(int index) const { emit itemActivated(*_virtual_items[index].item); }

void OmniList::updateVisibleItems() {
  auto start = std::chrono::high_resolution_clock::now();
  if (_virtual_items.empty()) return;
  setUpdatesEnabled(false);

  int scrollHeight = scrollBar->value();
  int startIndex = 0;

  // XXX - use binsearch to figure out starting point, or infer using last update and scroll direction
  while (startIndex < _virtual_items.size() &&
         _virtual_items[startIndex].y + _virtual_items[startIndex].height < scrollHeight) {
    ++startIndex;
  }

  if (startIndex >= _virtual_items.size()) return;

  qDebug() << "margin top" << margins.top;

  int marginOffset = std::max(0, margins.top - scrollHeight);
  int viewportY = marginOffset + _virtual_items[startIndex].y - scrollHeight;
  QSize viewportSize = size();
  int endIndex = startIndex;
  int lastY = -1;

  for (; endIndex < _virtual_items.size() && viewportY < viewportSize.height(); ++endIndex) {
    auto &vinfo = _virtual_items[endIndex];
    auto cacheIt = _widgetCache.find(vinfo.item->id());
    OmniListItemWidgetWrapper *widget = nullptr;

    if (lastY != -1 && lastY != vinfo.y) { viewportY += vinfo.y - lastY; }

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

    QPoint pos(vinfo.x, viewportY);
    QSize size(vinfo.width, vinfo.height);

    // qDebug() << "pos" << pos << "size" << size << "index" << endIndex << vinfo.item->id();

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

bool OmniList::isShowingEmptyState() const {
  qDebug() << "vcount" << _virtual_items.size();
  return _virtual_items.empty();
}

void OmniList::applySorting() {
  if (!m_sortingConfig.enabled) return;
}

void OmniList::calculateHeights() {
  if (!m_model.empty()) return calculateHeightsFromModel();

  auto start = std::chrono::high_resolution_clock::now();
  int yOffset = 0;
  int availableWidth = width() - margins.left - margins.right;
  std::optional<SectionCalculationContext> sctx;
  std::unordered_map<QString, CachedWidget> updatedCache;

  _virtual_items.clear();

  for (VirtualSectionInfo &section : m_sections) {
    auto sectionHeader = static_cast<VirtualSection *>(section.section.get());
    if (sctx) { yOffset += sctx->maxHeight; }

    int count = 1;
    int x = margins.left;
    int y = 0;

    sectionHeader->setCount(section.visibleCount);
    sectionHeader->initWidth(availableWidth);

    if (sectionHeader->showHeader()) {
      VirtualListWidgetInfo vinfo{.x = margins.left,
                                  .y = yOffset,
                                  .width = availableWidth,
                                  .height = sectionHeader->calculateHeight(availableWidth),
                                  .item = sectionHeader};

      _virtual_items.push_back(vinfo);
      yOffset += vinfo.height;
    }

    sctx = {.section = sectionHeader, .x = margins.left};

    for (auto &item : section.items) {
      if (item.filtered) continue;

      item.vIndex = _virtual_items.size();

      int height = 0;
      int width = availableWidth;
      int x = margins.left;
      int y = yOffset;

      if (item.vIndex >= visibleIndexRange.lower && item.vIndex <= visibleIndexRange.upper) {
        if (auto it = _widgetCache.find(item.id); it != _widgetCache.end()) {
          if (item.item->hasPartialUpdates()) { item.item->refresh(it->second.widget->widget()); }

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

      VirtualListWidgetInfo vinfo{.x = x, .y = y, .width = width, .height = height, .item = item.item.get()};

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
  // qDebug() << "calculateHeights() took" << duration << "ms";

  scrollBar->setMaximum(std::max(0, yOffset - height()));
  scrollBar->setMinimum(0);
  _virtualHeight = yOffset;
  updateVisibleItems();
  emit virtualHeightChanged(_virtualHeight);
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

void OmniList::updateFromList(std::vector<std::unique_ptr<AbstractVirtualItem>> &nextList,
                              SelectionPolicy selectionPolicy) {
  _virtual_items.clear();
  _idItemMap.clear();
  m_sections.clear();

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
    qDebug() << "update with keep selection";
    if (_selected == -1) {
      selectFirst();
    } else if (auto idx = indexOfItem(_selectedId); idx != -1) {
      qCritical() << "not implemented yet";
      // qDebug() << "idx of " << _selectedId << _items[idx].vIndex;
      // setSelectedIndex(_items[idx].vIndex);
    } else {
      qDebug() << "no index for" << _selectedId;
      setSelectedIndex(std::max(0, std::min(_selected, static_cast<int>(_virtual_items.size() - 1))));
    }
    break;
  case PreserveSelection: {
    int targetIndex = std::clamp(_selected, 0, (int)_virtual_items.size() - 1);
    int distance = 0;

    for (;;) {
      int lowTarget = targetIndex - distance;
      int highTarget = targetIndex + distance;
      bool hasLower = lowTarget >= 0 && lowTarget < _virtual_items.size();
      bool hasUpper = highTarget >= 0 && highTarget < _virtual_items.size();

      if (!hasLower && !hasUpper) {
        setSelectedIndex(-1);
        return;
      }

      if (hasLower && _virtual_items[lowTarget].item->selectable()) {
        setSelectedIndex(lowTarget);
        return;
      }

      if (hasUpper && _virtual_items[highTarget].item->selectable()) {
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

  while (next < _virtual_items.size() &&
         (_virtual_items[next].y == current.y || !_virtual_items[next].item->selectable())) {
    ++next;
  }

  int endNext = next;

  while (endNext < _virtual_items.size() && _virtual_items[endNext].y == _virtual_items[next].y) {
    ++endNext;
  }

  for (int i = endNext - 1; i >= next; --i) {
    auto &vItem = _virtual_items[i];

    if (vItem.x <= current.x && vItem.item->selectable()) {
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

    if (vitem.y < current.y && vitem.x <= current.x && vitem.item->selectable()) {
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

    if (!vItem.item->selectable()) { continue; }

    if (vItem.y > base.y && vItem.width == base.width && base.width == availableWidth) return false;

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

void OmniList::scrollTo(int idx, ScrollBehaviour behaviour) {
  if (idx < 0 || idx >= _virtual_items.size()) return;

  auto &item = _virtual_items[idx];

  int previousIdx = previousRowIndex(idx);

  if (previousIdx != -1 && _virtual_items[previousIdx].item->isSection()) {
    return scrollTo(previousIdx, behaviour);
  }

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
  calculateHeights();
}

void OmniList::beginUpdate() { _isUpdating = true; }

void OmniList::commitUpdate() {
  _isUpdating = false;
  if (m_sortingConfig.enabled) applySorting();
  calculateHeights();
}

const OmniList::AbstractVirtualItem *OmniList::selected() const {
  if (_selected >= 0 && _selected < _virtual_items.size()) return _virtual_items[_selected].item;

  return nullptr;
}

void OmniList::clearFilter() {
  if (!_filter) return;

  _filter.reset();

  std::ranges::for_each(m_sections, [](VirtualSectionInfo &section) {
    std::ranges::for_each(section.items, [](ListItemInfo &info) { info.filtered = false; });
  });

  if (!_isUpdating) calculateHeights();
}

const OmniList::AbstractItemFilter *OmniList::filter() const { return _filter.get(); }

void OmniList::setFilter(std::unique_ptr<AbstractItemFilter> filter) {
  for (auto &section : m_sections) {
    for (auto &item : section.items) {
      bool tmp = item.filtered;

      item.filtered = item.item->isListItem() && !filter->matches(*item.item.get());
      if (item.filtered && !tmp)
        section.visibleCount -= 1;
      else if (!item.filtered && tmp)
        section.visibleCount += 1;
    }
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

void OmniList::setMargins(int left, int top, int right, int bottom) {
  margins = {left, top, right, bottom};
  if (!_isUpdating) { calculateHeights(); }
}

void OmniList::setMargins(int value) { setMargins(value, value, value, value); }

void OmniList::clear() {
  _idItemMap.clear();
  m_sections.clear();
  _isUpdating = false;
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
  /*
int idx = indexOfItem(id);

if (idx == -1) { return false; }

auto &info = _items[idx];

qDebug() << "idx update" << idx;

auto it = _idItemMap.find(id);

if (it == _idItemMap.end()) return false;

cb(it->second);

if (auto it = _visibleWidgets.find(info.vIndex); it != _visibleWidgets.end()) { _visibleWidgets.erase(it); }

if (auto it = _widgetCache.find(info.id); it != _widgetCache.end()) {
it->second.widget->deleteLater();
_widgetCache.erase(it);
}

if (!_isUpdating) { updateVisibleItems(); };

emit itemUpdated(*info.item.get());

return true;
*/
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

  if (!_isUpdating) { updateVisibleItems(); }
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

void OmniList::setSorting(const SortingConfig &config) {
  bool isSame = m_sortingConfig.enabled == config.enabled &&
                m_sortingConfig.preserveSectionOrder == config.preserveSectionOrder;

  if (isSame) return;

  m_sortingConfig = config;
  applySorting();
  calculateHeights();
}

void OmniList::addItem(std::unique_ptr<AbstractVirtualItem> item) {

  _idItemMap.insert({item->id(), item.get()});

  if (item->isSection()) {
    VirtualSectionInfo info;

    info.visibleCount = 0;
    info.section = std::move(item);
    info.index = m_sections.size();
    m_sections.emplace_back(std::move(info));
    return;
  }

  if (m_sections.empty()) {
    VirtualSectionInfo defaultSection;

    defaultSection.visibleCount = 0;
    defaultSection.section = std::make_unique<DefaultVirtualSection>();
    _idItemMap.insert({defaultSection.section->id(), defaultSection.section.get()});
    defaultSection.index = m_sections.size();
    m_sections.emplace_back(std::move(defaultSection));
  }

  ListItemInfo info;
  auto &currentSection = m_sections.at(m_sections.size() - 1);

  info.filtered = _filter && item->isListItem() && !_filter->matches(*item.get());
  info.cachedHeight = -1;
  info.id = item->id();
  info.item = std::move(item);
  info.sectionIndex = currentSection.index;

  if (!info.filtered) currentSection.visibleCount += 1;

  currentSection.items.emplace_back(std::move(info));

  if (!_isUpdating) { calculateHeights(); }
}

OmniList::OmniList()
    : scrollBar(new OmniScrollBar(this)), _selected(DEFAULT_SELECTION_INDEX), _isUpdating(false),
      margins({0, 0, 0, 0}) {
  scrollBar->setSingleStep(40);
  connect(scrollBar, &QScrollBar::valueChanged, this, [this]() { updateVisibleItems(); });

  int scrollBarWidth = scrollBar->sizeHint().width();

  setMargins(5, 5, 5, 5);
  _visibleWidgets.reserve(20);
  _widgetCache.reserve(20);
}

OmniList::~OmniList() {}

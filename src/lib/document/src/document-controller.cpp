#include <QClipboard>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QInputMethodEvent>
#include <QInputMethodQueryEvent>
#include <QStyleHints>
#include <QScopedValueRollback>
#include <algorithm>
#include <cmath>
#include <utility>
#include <ranges>
#include <tuple>
#include "document-controller.hpp"
#include "document-selection.hpp"

namespace vicinae::document {

static constexpr qreal DRAG_THRESHOLD = 5.0;
static constexpr int AUTO_SCROLL_INTERVAL_MS = 16;
static constexpr qreal AUTO_SCROLL_MARGIN = 60.0;
static constexpr qreal AUTO_SCROLL_MAX_SPEED = 12.0;

DocumentController::DocumentController(QObject *parent) : QObject(parent) {
  m_autoScrollTimer.setInterval(AUTO_SCROLL_INTERVAL_MS);
  connect(&m_autoScrollTimer, &QTimer::timeout, this, &DocumentController::autoScrollTick);
}

DocumentController::~DocumentController() { setFlickable(nullptr); }

void DocumentController::setFlickable(QQuickItem *item) {
  if (m_flickable == item) return;
  if (m_flickable) {
    m_flickable->removeEventFilter(this);
    m_flickable->setFlag(QQuickItem::ItemAcceptsInputMethod, m_acceptedInputMethod);
  }
  m_flickable = item;
  if (m_flickable) {
    m_acceptedInputMethod = m_flickable->flags().testFlag(QQuickItem::ItemAcceptsInputMethod);
    m_flickable->installEventFilter(this);
    updateInputMethod();
  }
  emit flickableChanged();
}

void DocumentController::setTypingTarget(QQuickItem *item) {
  if (m_typingTarget == item) return;
  disconnect(m_typingTargetDestroyed);
  m_typingTarget = item;
  if (item) {
    m_typingTargetDestroyed = connect(item, &QObject::destroyed, this, [this] {
      updateInputMethod();
      emit typingTargetChanged();
    });
  }
  updateInputMethod();
  emit typingTargetChanged();
}

void DocumentController::updateInputMethod() {
  if (m_flickable)
    m_flickable->setFlag(QQuickItem::ItemAcceptsInputMethod, m_acceptedInputMethod || m_typingTarget);
}

bool DocumentController::redirectInput(QEvent *event) {
  if (m_redirectingInput || !m_typingTarget || m_typingTarget == m_flickable ||
      !m_typingTarget->isEnabled() || !m_typingTarget->isVisible())
    return false;
  QScopedValueRollback guard(m_redirectingInput, true);
  m_typingTarget->forceActiveFocus(Qt::OtherFocusReason);
  clearSelection();
  QCoreApplication::sendEvent(m_typingTarget, event);
  return true;
}

bool DocumentController::eventFilter(QObject *obj, QEvent *event) {
  if (obj != m_flickable) return false;
  switch (event->type()) {
  // NOLINTBEGIN(cppcoreguidelines-pro-type-static-cast-downcast)
  case QEvent::MouseButtonPress: {
    auto *me = static_cast<QMouseEvent *>(event);
    if (me->button() != Qt::LeftButton) return false;
    handlePress(me->position().x(), me->position().y(), me->modifiers().testFlag(Qt::ShiftModifier));
    return true;
  }
  case QEvent::MouseMove: {
    auto *me = static_cast<QMouseEvent *>(event);
    handleMove(me->position().x(), me->position().y());
    return true;
  }
  case QEvent::MouseButtonRelease: {
    auto *me = static_cast<QMouseEvent *>(event);
    if (me->button() != Qt::LeftButton) return false;
    handleRelease(me->position().x(), me->position().y());
    return true;
  }
  case QEvent::MouseButtonDblClick: {
    auto *me = static_cast<QMouseEvent *>(event);
    if (me->button() != Qt::LeftButton) return false;
    handleDoubleClick(me->position().x(), me->position().y());
    return true;
  }
  case QEvent::KeyPress: {
    auto *key = static_cast<QKeyEvent *>(event);
    if (key->matches(QKeySequence::Copy) && hasSelection()) {
      copy();
      return true;
    }
    if (key->matches(QKeySequence::SelectAll)) {
      selectAll();
      return true;
    }
    const bool shortcut =
        key->modifiers().testFlag(Qt::MetaModifier) ||
        (key->modifiers().testFlag(Qt::ControlModifier) && !key->modifiers().testFlag(Qt::AltModifier) &&
         !key->modifiers().testFlag(Qt::GroupSwitchModifier));
    const bool printable =
        std::ranges::any_of(key->text(), [](QChar ch) { return ch.isPrint() || ch.isHighSurrogate(); });
    const bool deadKey = key->key() >= Qt::Key_Dead_Grave && key->key() <= Qt::Key_Dead_Longsolidusoverlay;
    if (key->matches(QKeySequence::Paste) || (!shortcut && (printable || deadKey)))
      return redirectInput(event);
    return false;
  }
  case QEvent::InputMethod: {
    auto *input = static_cast<QInputMethodEvent *>(event);
    if (!input->preeditString().isEmpty() || !input->commitString().isEmpty() || input->replacementLength())
      return redirectInput(event);
    return false;
  }
  case QEvent::InputMethodQuery: {
    if (!m_typingTarget || m_typingTarget == m_flickable) return false;
    auto *query = static_cast<QInputMethodQueryEvent *>(event);
    QCoreApplication::sendEvent(m_typingTarget, event);
    for (const auto rectangle :
         {Qt::ImCursorRectangle, Qt::ImAnchorRectangle, Qt::ImInputItemClipRectangle}) {
      if (query->queries().testFlag(rectangle))
        query->setValue(rectangle,
                        m_typingTarget->mapRectToItem(m_flickable, query->value(rectangle).toRectF()));
    }
    return true;
  }
  case QEvent::UngrabMouse:
    stopAutoScroll();
    setPressed(false);
    return false;
  // NOLINTEND(cppcoreguidelines-pro-type-static-cast-downcast)
  default:
    return false;
  }
}

void DocumentController::registerSelection(DocumentSelection *selection) {
  if (m_entries.size() == m_entries.capacity())
    m_entries.reserve(std::max<std::size_t>(32, m_entries.size() * 2));
  m_entries.emplace_back(SelectableEntry{selection, 0.0, 0.0, 0.0, 0.0});
  m_orderDirty = true;
  if (m_model) {
    if (m_hasSelection) applyCurrentSelection(true);
    scheduleRestore();
  }
}

void DocumentController::unregisterSelection(DocumentSelection *selection) {
  const auto it = std::ranges::find(m_entries, selection, &SelectableEntry::selection);
  if (it == m_entries.end()) return;
  const int index = static_cast<int>(std::distance(m_entries.begin(), it));
  const bool endpointRemoved = m_anchor.entryIndex == index || m_current.entryIndex == index;
  m_entries.erase(it);
  if (index < m_selectedFrom) --m_selectedFrom;
  if (index <= m_selectedTo) --m_selectedTo;
  if (m_selectedTo < m_selectedFrom) m_selectedFrom = m_selectedTo = -1;
  if (m_model) return;
  if (endpointRemoved) {
    clearSelection();
  } else {
    if (m_anchor.entryIndex > index) --m_anchor.entryIndex;
    if (m_current.entryIndex > index) --m_current.entryIndex;
  }
}

QPointF DocumentController::toContainerCoords(qreal viewportX, qreal viewportY) const {
  if (m_container && m_flickable) return m_container->mapFromItem(m_flickable, QPointF(viewportX, viewportY));
  return {viewportX, viewportY};
}

std::optional<DocumentController::ReadingAnchor> DocumentController::readingAnchor(int row) const {
  if (!m_model || !m_flickable) return {};
  DocumentSelection *nearest = nullptr;
  qreal distance = m_flickable->height();
  for (const auto &entry : m_entries) {
    auto *selection = entry.selection;
    auto *target = selection->target();
    if (!target || selection->row() != row || !selection->length()) continue;
    const auto bounds = target->mapRectToItem(m_flickable, target->boundingRect());
    if (bounds.bottom() <= 0 || bounds.top() >= m_flickable->height()) continue;
    const auto candidate = std::max(qreal(0), bounds.top());
    if (!nearest || candidate < distance) {
      nearest = selection;
      distance = candidate;
    }
  }
  if (!nearest) return {};
  auto *target = nearest->target();
  const auto top = target->mapFromItem(m_flickable, QPointF()).y();
  const int position = nearest->positionAt({0, std::max(qreal(0), top) + 1});
  const auto y = target->mapToItem(m_flickable, nearest->rectangleAt(position).topLeft()).y();
  return ReadingAnchor{m_model->index(row, 0), nearest->part(), position, y};
}

std::optional<QRectF> DocumentController::positionRectangle(int row, int part, int position) const {
  if (!m_flickable) return {};
  for (const auto &entry : m_entries) {
    const auto *selection = entry.selection;
    if (selection->row() == row && selection->part() == part && selection->target())
      return selection->target()->mapRectToItem(m_flickable, selection->rectangleAt(position));
  }
  return {};
}

void DocumentController::revealHorizontalPosition(int row, int part, int position, int length) {
  for (const auto &entry : m_entries) {
    const auto *selection = entry.selection;
    auto *target = selection->target();
    if (!target || selection->row() != row || selection->part() != part) continue;
    for (auto *parent = target->parentItem(); parent && parent != m_flickable;
         parent = parent->parentItem()) {
      const auto contentX = parent->property("contentX");
      if (!contentX.isValid()) continue;
      const auto rect = target->mapRectToItem(
          parent, selection->rectangleAt(position).united(selection->rectangleAt(position + length)));
      const auto offset =
          rect.left() < 8 ? rect.left() - 8 : std::max(qreal(0), rect.right() - parent->width() + 8);
      const auto maximum = std::max(qreal(0), parent->property("contentWidth").toReal() - parent->width());
      parent->setProperty("contentX", std::clamp(contentX.toReal() + offset, qreal(0), maximum));
    }
    return;
  }
}

std::optional<qreal> DocumentController::readingAnchorOffset(const ReadingAnchor &anchor) const {
  if (!anchor.index.isValid() || !m_flickable) return {};
  for (const auto &entry : m_entries) {
    const auto *selection = entry.selection;
    if (!selection->target() || selection->row() != anchor.index.row() || selection->part() != anchor.part ||
        anchor.position > selection->length())
      continue;
    const auto y =
        selection->target()->mapToItem(m_flickable, selection->rectangleAt(anchor.position).topLeft()).y();
    return y - anchor.viewportY;
  }
  return {};
}

void DocumentController::refreshGeometry() {
  if (!m_container) return;
  bool orderChanged = std::exchange(m_orderDirty, false);
  for (auto &entry : m_entries) {
    const auto *target = entry.selection->target();
    const auto bounds = target->mapRectToItem(m_container, target->boundingRect());
    orderChanged |= entry.cachedX != bounds.x() || entry.cachedY != bounds.y();
    entry.cachedY = bounds.y();
    entry.cachedHeight = bounds.height();
    entry.cachedX = bounds.x();
    entry.cachedWidth = bounds.width();
  }
  if (!orderChanged) return;
  auto *anchor = !m_model && m_anchor.entryIndex >= 0 ? m_entries[m_anchor.entryIndex].selection : nullptr;
  auto *current = !m_model && m_current.entryIndex >= 0 ? m_entries[m_current.entryIndex].selection : nullptr;
  std::ranges::stable_sort(m_entries, [](const auto &a, const auto &b) {
    if (a.cachedY != b.cachedY) return a.cachedY < b.cachedY;
    return a.cachedX < b.cachedX;
  });
  const auto indexOf = [this](DocumentSelection *selection) {
    if (!selection) return -1;
    return static_cast<int>(std::distance(
        m_entries.begin(), std::ranges::find(m_entries, selection, &SelectableEntry::selection)));
  };
  if (!m_model) {
    m_anchor.entryIndex = indexOf(anchor);
    m_current.entryIndex = indexOf(current);
  }
  m_selectedFrom = m_selectedTo = -1;
  for (int i = 0; std::cmp_less(i, m_entries.size()); ++i) {
    if (!m_entries[i].selection->hasSelection()) continue;
    if (m_selectedFrom < 0) m_selectedFrom = i;
    m_selectedTo = i;
  }
}

int DocumentController::entryAt(qreal containerX, qreal containerY) const {
  if (m_entries.empty()) return -1;

  int const n = static_cast<int>(m_entries.size());

  int hit = -1;
  for (int i = 0; i < n; ++i) {
    if (containerY <= m_entries[i].cachedY + m_entries[i].cachedHeight) {
      hit = i;
      break;
    }
  }
  if (hit < 0) hit = n - 1;

  qreal const hitY = m_entries[hit].cachedY;
  int bestIdx = hit;
  qreal bestDist = std::abs(containerX - (m_entries[hit].cachedX + m_entries[hit].cachedWidth * 0.5));

  for (int i = hit - 1; i >= 0 && std::abs(m_entries[i].cachedY - hitY) < 1.0; --i) {
    qreal const dist = std::abs(containerX - (m_entries[i].cachedX + m_entries[i].cachedWidth * 0.5));
    if (dist < bestDist) {
      bestDist = dist;
      bestIdx = i;
    }
  }
  for (int i = hit + 1; i < n && std::abs(m_entries[i].cachedY - hitY) < 1.0; ++i) {
    qreal const dist = std::abs(containerX - (m_entries[i].cachedX + m_entries[i].cachedWidth * 0.5));
    if (dist < bestDist) {
      bestDist = dist;
      bestIdx = i;
    }
  }

  return bestIdx;
}

int DocumentController::positionAt(DocumentSelection *selection, qreal containerX, qreal containerY) const {
  const auto *item = selection->target();
  const auto localPos = item->mapFromItem(m_container, QPointF(containerX, containerY));
  if (localPos.y() < 0) return 0;
  if (localPos.y() > item->height()) return selection->length();
  return selection->positionAt(localPos);
}

QString DocumentController::linkAt(DocumentSelection *selection, qreal containerX, qreal containerY) const {
  const auto *item = selection->target();
  const auto localPos = item->mapFromItem(m_container, QPointF(containerX, containerY));
  if (!item->boundingRect().contains(localPos)) return {};
  return selection->linkAtPosition(localPos);
}

void DocumentController::handlePress(qreal x, qreal y, bool extend) {
  if (m_flickable) m_flickable->forceActiveFocus(Qt::MouseFocusReason);
  refreshGeometry();
  m_pressPos = {x, y};
  m_mouseX = x;
  m_mouseY = y;
  m_pressLink.clear();
  if (extend && validAnchor()) {
    setPressed(true);
    m_dragging = true;
    handleMove(x, y);
    return;
  }

  if (m_doubleClickTimer.isValid() &&
      m_doubleClickTimer.elapsed() < QGuiApplication::styleHints()->mouseDoubleClickInterval() &&
      std::hypot(x - m_doubleClickPos.x(), y - m_doubleClickPos.y()) < DRAG_THRESHOLD) {
    m_doubleClickTimer.invalidate();
    refreshGeometry();
    auto cp = toContainerCoords(x, y);
    int const idx = entryAt(cp.x(), cp.y());
    if (idx >= 0) {
      clearSelection();
      setPressed(true);
      int const len = m_entries[idx].selection->length();
      m_anchor = anchorFor(idx, 0);
      m_current = anchorFor(idx, len);
      applyCurrentSelection();
    }
    return;
  }

  clearSelection();
  setPressed(true);
  refreshGeometry();

  m_pressPos = {x, y};
  m_dragging = false;
  m_mouseX = x;
  m_mouseY = y;

  auto cp = toContainerCoords(x, y);
  int const idx = entryAt(cp.x(), cp.y());
  if (idx < 0) return;

  auto *selection = m_entries[idx].selection;
  const int position = positionAt(selection, cp.x(), cp.y());
  m_pressLink = linkAt(selection, cp.x(), cp.y());

  m_anchor = anchorFor(idx, position);
  m_current = m_anchor;
}

void DocumentController::handleMove(qreal x, qreal y) {
  if (!m_pressed || !validAnchor()) return;
  refreshGeometry();

  auto dist = std::hypot(x - m_pressPos.x(), y - m_pressPos.y());
  if (!m_dragging) {
    if (dist < DRAG_THRESHOLD) return;
    m_dragging = true;
    m_pressLink.clear();
  }

  m_mouseX = x;
  m_mouseY = y;

  auto cp = toContainerCoords(x, y);
  const int idx = entryAt(cp.x(), cp.y());
  if (idx < 0) return;

  const int position = positionAt(m_entries[idx].selection, cp.x(), cp.y());

  m_current = anchorFor(idx, position);
  applyCurrentSelection();

  if (m_flickable) {
    qreal const flickH = m_flickable->height();
    if (y < m_topInset + AUTO_SCROLL_MARGIN || y > flickH - m_bottomInset - AUTO_SCROLL_MARGIN)
      startAutoScroll();
    else
      stopAutoScroll();
  }
}

void DocumentController::handleRelease(qreal x, qreal y) {
  if (m_dragging) handleMove(x, y);
  stopAutoScroll();
  setPressed(false);

  if (!m_dragging && !m_pressLink.isEmpty()) emit linkActivated(m_pressLink);

  m_pressLink.clear();
  m_dragging = false;
}

void DocumentController::handleDoubleClick(qreal x, qreal y) {
  clearSelection();
  setPressed(true);
  m_pressPos = {x, y};
  refreshGeometry();

  auto cp = toContainerCoords(x, y);
  int const idx = entryAt(cp.x(), cp.y());
  if (idx < 0) return;

  auto *selection = m_entries[idx].selection;
  const int pos = positionAt(selection, cp.x(), cp.y());
  const auto [start, end] = selection->wordAt(pos);
  m_anchor = anchorFor(idx, start);
  m_current = anchorFor(idx, end);
  applyCurrentSelection();

  m_doubleClickTimer.start();
  m_doubleClickPos = {x, y};
}

void DocumentController::restoreSelection() {
  if (!m_hasSelection || m_applyingSelection) return;
  refreshGeometry();
  applyCurrentSelection(true);
}

void DocumentController::selectAll() {
  if (m_model) {
    const int count = m_model->rowCount();
    int first = 0;
    while (first < count && m_model->documentParts(first).empty())
      ++first;
    int last = count - 1;
    while (last >= first && m_model->documentParts(last).empty())
      --last;
    if (first > last) return;
    const int part = static_cast<int>(m_model->documentParts(last).size()) - 1;
    m_anchor = {-1, 0, m_model->index(first), 0};
    m_current = {-1, m_model->partLength(last, part), m_model->index(last), part};
    applyCurrentSelection();
    return;
  }
  if (m_entries.empty()) return;
  refreshGeometry();

  m_anchor = {0, 0};
  const int lastIdx = static_cast<int>(m_entries.size()) - 1;
  m_current = {lastIdx, m_entries[lastIdx].selection->length()};
  applyCurrentSelection();
}

void DocumentController::clearSelection() {
  const QScopedValueRollback guard(m_applyingSelection, true);
  stopAutoScroll();
  setPressed(false);
  m_dragging = false;
  m_pressLink.clear();
  if (m_model) {
    for (auto &entry : m_entries)
      entry.selection->select(0, 0);
  } else {
    for (int i = m_selectedFrom; i >= 0 && i <= m_selectedTo; ++i)
      m_entries[i].selection->select(0, 0);
  }
  m_selectedFrom = m_selectedTo = -1;
  m_anchor = {-1, 0};
  m_current = {-1, 0};
  setHasSelection(false);
}

void DocumentController::copy() {
  if (!m_hasSelection) return;
  restoreSelection();
  QStringList parts;
  if (m_model && validAnchor() && m_current.index.isValid()) {
    auto from = m_anchor;
    auto to = m_current;
    const auto key = [](const auto &anchor) {
      return std::tuple(anchor.index.row(), anchor.part, anchor.position);
    };
    if (key(from) > key(to)) std::swap(from, to);
    for (int row = from.index.row(); row <= to.index.row(); ++row) {
      const int count = static_cast<int>(m_model->documentParts(row).size());
      const int first = row == from.index.row() ? from.part : 0;
      const int last = row == to.index.row() ? to.part : count - 1;
      for (int part = first; part <= last; ++part) {
        const int length = m_model->partLength(row, part);
        const int start =
            row == from.index.row() && part == from.part ? std::clamp(from.position, 0, length) : 0;
        const int end =
            row == to.index.row() && part == to.part ? std::clamp(to.position, 0, length) : length;
        const auto text = m_model->textForSelection(row, part, start, end);
        if (!text.isEmpty()) parts.append(text);
      }
    }
  } else
    for (const auto &entry : m_entries) {
      if (!entry.selection->hasSelection()) continue;
      const auto text = entry.selection->selectedText();
      if (!text.isEmpty()) parts.append(text);
    }
  const auto text = parts.join(QStringLiteral("\n\n"));
  if (!text.isEmpty()) QGuiApplication::clipboard()->setText(text);
}

void DocumentController::applySelection(int fromEntry, int fromPosition, int toEntry, int toPosition,
                                        bool force) {
  if (fromEntry < 0 || toEntry < 0) return;
  if (fromEntry > toEntry || (fromEntry == toEntry && fromPosition > toPosition)) {
    std::swap(fromEntry, toEntry);
    std::swap(fromPosition, toPosition);
  }
  bool anySelected = false;
  const int begin = m_selectedFrom < 0 ? fromEntry : std::min(fromEntry, m_selectedFrom);
  const int end = std::max(toEntry, m_selectedTo);
  for (int i = begin; i <= end; ++i) {
    auto *selection = m_entries[i].selection;
    if (i < fromEntry || i > toEntry) {
      selection->select(0, 0);
      continue;
    }
    selection->select(i == fromEntry ? fromPosition : 0, i == toEntry ? toPosition : selection->length(),
                      force);
    anySelected |= selection->hasSelection();
  }
  m_selectedFrom = anySelected ? fromEntry : -1;
  m_selectedTo = anySelected ? toEntry : -1;
  setHasSelection(anySelected);
}

void DocumentController::setHasSelection(bool has) {
  if (m_hasSelection != has) {
    m_hasSelection = has;
    emit hasSelectionChanged();
  }
}

void DocumentController::setPressed(bool pressed) {
  if (m_pressed == pressed) return;
  m_pressed = pressed;
  emit selectingChanged();
}

void DocumentController::startAutoScroll() {
  if (!m_autoScrollTimer.isActive()) m_autoScrollTimer.start();
}

void DocumentController::stopAutoScroll() { m_autoScrollTimer.stop(); }

void DocumentController::autoScrollTick() {
  if (!m_flickable || !m_pressed || !validAnchor()) return;

  qreal const flickH = m_flickable->height();
  qreal const contentY = m_flickable->property("contentY").toReal();
  qreal const contentH = m_flickable->property("contentHeight").toReal();
  qreal const originY = m_flickable->property("originY").toReal();
  qreal const minY = originY - m_flickable->property("topMargin").toReal();
  qreal const maxY =
      std::max(minY, originY + contentH - flickH + m_flickable->property("bottomMargin").toReal());

  qreal delta = 0.0;
  qreal const top = m_topInset + AUTO_SCROLL_MARGIN;
  qreal const bottom = flickH - m_bottomInset - AUTO_SCROLL_MARGIN;
  if (m_mouseY < top) {
    qreal const t = std::min((top - m_mouseY) / AUTO_SCROLL_MARGIN, qreal(1));
    delta = -AUTO_SCROLL_MAX_SPEED * t * t;
  } else if (m_mouseY > bottom) {
    qreal const t = std::min((m_mouseY - bottom) / AUTO_SCROLL_MARGIN, qreal(1));
    delta = AUTO_SCROLL_MAX_SPEED * t * t;
  } else {
    stopAutoScroll();
    return;
  }

  qreal const newContentY = std::clamp(contentY + delta, minY, maxY);
  if (newContentY == contentY) return;

  m_flickable->setProperty("contentY", newContentY);
  handleMove(m_mouseX, m_mouseY);
}

void DocumentController::setModel(DocumentModel *model) {
  if (m_model == model) return;
  clearSelection();
  if (m_model) disconnect(m_model, nullptr, this, nullptr);
  m_model = model;
  m_search.setModel(model);
  if (model) {
    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &DocumentController::clearSelection);
    connect(model, &QAbstractItemModel::rowsRemoved, this, [this] {
      if (!validAnchor() || !m_current.index.isValid())
        clearSelection();
      else
        scheduleRestore();
    });
    connect(model, &QAbstractItemModel::dataChanged, this, &DocumentController::scheduleRestore);
    connect(model, &QAbstractItemModel::rowsInserted, this, &DocumentController::scheduleRestore);
    connect(model, &QAbstractItemModel::rowsMoved, this, &DocumentController::scheduleRestore);
    connect(model, &QObject::destroyed, this, [this] {
      m_anchor = {};
      m_current = {};
      for (auto &entry : m_entries)
        entry.selection->select(0, 0);
      m_selectedFrom = m_selectedTo = -1;
      clearSelection();
      emit modelChanged();
    });
  }
  emit modelChanged();
}

bool DocumentController::validAnchor() const {
  return m_model ? m_anchor.index.isValid() : m_anchor.entryIndex >= 0;
}

DocumentController::SelectionAnchor DocumentController::anchorFor(int entry, int position) const {
  if (!m_model) return {entry, position};
  const auto *selection = m_entries[entry].selection;
  const int row = selection->row();
  if (row < 0 || row >= m_model->rowCount() || selection->part() < 0 ||
      std::cmp_greater_equal(selection->part(), m_model->documentParts(row).size()))
    return {};
  return {-1, position, m_model->index(row), selection->part()};
}

void DocumentController::scheduleRestore() {
  if (m_restorePending) return;
  m_restorePending = true;
  QTimer::singleShot(0, this, [this] {
    m_restorePending = false;
    restoreSelection();
  });
}

void DocumentController::applyCurrentSelection(bool force) {
  // Selecting newly initialized rich text can itself emit textChanged.
  if (m_applyingSelection) return;
  const QScopedValueRollback guard(m_applyingSelection, true);
  if (!m_model) {
    applySelection(m_anchor.entryIndex, m_anchor.position, m_current.entryIndex, m_current.position, force);
    return;
  }
  if (!validAnchor() || !m_current.index.isValid()) return;
  const auto clamp = [this](SelectionAnchor &anchor) {
    const int count = static_cast<int>(m_model->documentParts(anchor.index.row()).size());
    if (count == 0) return false;
    anchor.part = std::clamp(anchor.part, 0, count - 1);
    anchor.position = std::clamp(anchor.position, 0, m_model->partLength(anchor.index.row(), anchor.part));
    return true;
  };
  if (!clamp(m_anchor) || !clamp(m_current)) {
    clearSelection();
    return;
  }
  auto from = m_anchor;
  auto to = m_current;
  const auto key = [](const auto &anchor) {
    return std::tuple(anchor.index.row(), anchor.part, anchor.position);
  };
  if (key(from) > key(to)) std::swap(from, to);
  for (auto &entry : m_entries) {
    auto *selection = entry.selection;
    const auto part = std::pair(selection->row(), selection->part());
    const auto first = std::pair(from.index.row(), from.part);
    const auto last = std::pair(to.index.row(), to.part);
    if (part < first || part > last)
      selection->select(0, 0);
    else
      selection->select(part == first ? from.position : 0, part == last ? to.position : selection->length(),
                        force);
  }
  setHasSelection(key(from) != key(to));
}

} // namespace vicinae::document

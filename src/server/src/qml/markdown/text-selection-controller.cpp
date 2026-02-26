#include "text-selection-controller.hpp"
#include <QClipboard>
#include <QGuiApplication>
#include <QMetaObject>
#include <QMouseEvent>
#include <algorithm>
#include <cmath>
#include <utility>

static constexpr qreal DRAG_THRESHOLD = 5.0;
static constexpr int AUTO_SCROLL_INTERVAL_MS = 16;
static constexpr qreal AUTO_SCROLL_MARGIN = 60.0;
static constexpr qreal AUTO_SCROLL_MAX_SPEED = 12.0;

TextSelectionController::TextSelectionController(QObject *parent) : QObject(parent) {
  m_autoScrollTimer.setInterval(AUTO_SCROLL_INTERVAL_MS);
  connect(&m_autoScrollTimer, &QTimer::timeout, this, &TextSelectionController::autoScrollTick);
}

void TextSelectionController::setFlickable(QQuickItem *item) {
  if (m_flickable == item) return;
  if (m_flickable) m_flickable->removeEventFilter(this);
  m_flickable = item;
  if (m_flickable) m_flickable->installEventFilter(this);
}

bool TextSelectionController::eventFilter(QObject *obj, QEvent *event) {
  if (obj != m_flickable) return false;
  switch (event->type()) {
  // NOLINTBEGIN(cppcoreguidelines-pro-type-static-cast-downcast)
  case QEvent::MouseButtonPress: {
    auto *me = static_cast<QMouseEvent *>(event);
    if (me->button() != Qt::LeftButton) return false;
    handlePress(me->position().x(), me->position().y());
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
  // NOLINTEND(cppcoreguidelines-pro-type-static-cast-downcast)
  default:
    return false;
  }
}

void TextSelectionController::registerSelectable(QQuickItem *item, int order, bool isTextEdit) {
  if (!item) return;
  if (isTextEdit) item->setAcceptedMouseButtons(Qt::NoButton);
  auto it = std::ranges::lower_bound(m_entries, order, {}, &SelectableEntry::order);
  m_entries.insert(it, {item, isTextEdit, order, 0.0, 0.0, 0.0, 0.0});
}

void TextSelectionController::unregisterSelectable(QQuickItem *item) {
  std::erase_if(m_entries, [item](const auto &e) { return e.item == item; });
}

QPointF TextSelectionController::toContainerCoords(qreal viewportX, qreal viewportY) const {
  if (m_container && m_flickable) return m_container->mapFromItem(m_flickable, QPointF(viewportX, viewportY));
  return {viewportX, viewportY};
}

void TextSelectionController::refreshGeometry() {
  if (!m_container) return;
  for (auto &entry : m_entries) {
    auto mapped = entry.item->mapToItem(m_container, QPointF(0, 0));
    entry.cachedY = mapped.y();
    entry.cachedHeight = entry.item->height();
    entry.cachedX = mapped.x();
    entry.cachedWidth = entry.item->width();
  }
  std::ranges::sort(m_entries, [](const auto &a, const auto &b) {
    if (a.cachedY != b.cachedY) return a.cachedY < b.cachedY;
    return a.order < b.order;
  });
}

int TextSelectionController::entryAt(qreal containerX, qreal containerY) const {
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

int TextSelectionController::positionAt(QQuickItem *textEdit, qreal containerX, qreal containerY) const {
  int result = 0;
  auto localPos = textEdit->mapFromItem(m_container, QPointF(containerX, containerY));
  QMetaObject::invokeMethod(textEdit, "positionAt", Q_RETURN_ARG(int, result), Q_ARG(double, localPos.x()),
                            Q_ARG(double, localPos.y()));
  return result;
}

QString TextSelectionController::linkAt(QQuickItem *textEdit, qreal containerX, qreal containerY) const {
  QString result;
  auto localPos = textEdit->mapFromItem(m_container, QPointF(containerX, containerY));
  QMetaObject::invokeMethod(textEdit, "linkAt", Q_RETURN_ARG(QString, result), Q_ARG(double, localPos.x()),
                            Q_ARG(double, localPos.y()));
  return result;
}

void TextSelectionController::handlePress(qreal x, qreal y) {
  // Triple-click: press shortly after double-click at same spot → select all
  if (m_doubleClickTimer.isValid() && m_doubleClickTimer.elapsed() < 400 &&
      std::hypot(x - m_doubleClickPos.x(), y - m_doubleClickPos.y()) < DRAG_THRESHOLD) {
    m_doubleClickTimer.invalidate();
    refreshGeometry();
    auto cp = toContainerCoords(x, y);
    int const idx = entryAt(cp.x(), cp.y());
    if (idx >= 0 && m_entries[idx].isTextEdit) {
      selectAllTextEdit(m_entries[idx].item);
      int const len = m_entries[idx].item->property("length").toInt();
      m_anchor = {idx, 0};
      m_current = {idx, len};
      setHasSelection(true);
    }
    return;
  }

  clearSelection();
  refreshGeometry();

  m_pressPos = {x, y};
  m_dragging = false;
  m_mouseX = x;
  m_mouseY = y;

  auto cp = toContainerCoords(x, y);
  int const idx = entryAt(cp.x(), cp.y());
  if (idx < 0) return;

  const auto &entry = m_entries[idx];
  int charPos = 0;
  if (entry.isTextEdit) {
    charPos = positionAt(entry.item, cp.x(), cp.y());
    m_pressLink = linkAt(entry.item, cp.x(), cp.y());
  } else {
    m_pressLink.clear();
  }

  m_anchor = {idx, charPos};
  m_current = m_anchor;
}

void TextSelectionController::handleMove(qreal x, qreal y) {
  if (m_anchor.entryIndex < 0) return;

  auto dist = std::hypot(x - m_pressPos.x(), y - m_pressPos.y());
  if (!m_dragging) {
    if (dist < DRAG_THRESHOLD) return;
    m_dragging = true;
    m_pressLink.clear();
  }

  m_mouseX = x;
  m_mouseY = y;

  auto cp = toContainerCoords(x, y);
  int idx = entryAt(cp.x(), cp.y());
  if (idx < 0) return;

  int charPos = 0;
  {
    const auto &entry = m_entries[idx];
    if (entry.isTextEdit) {
      if (cp.y() >= entry.cachedY && cp.y() <= entry.cachedY + entry.cachedHeight)
        charPos = positionAt(entry.item, cp.x(), cp.y());
      else if (cp.y() > entry.cachedY + entry.cachedHeight)
        charPos = entry.item->property("length").toInt();
    }
  }

  // Advance to adjacent entry when charPos is stuck at the edge AND the
  // mouse has physically left the entry's bounds. This handles the case
  // where entryAt still returns the same entry (e.g. mouse above the
  // first entry or below the last) but the user clearly wants cross-block
  // selection.
  if (idx == m_anchor.entryIndex && m_entries[idx].isTextEdit) {
    const auto &entry = m_entries[idx];
    int const len = entry.item->property("length").toInt();
    int const n = static_cast<int>(m_entries.size());
    if (charPos >= len && cp.y() > entry.cachedY + entry.cachedHeight && idx + 1 < n) {
      idx = idx + 1;
      charPos = 0;
      const auto &ne = m_entries[idx];
      if (ne.isTextEdit && cp.y() >= ne.cachedY && cp.y() <= ne.cachedY + ne.cachedHeight)
        charPos = positionAt(ne.item, cp.x(), cp.y());
    } else if (charPos <= 0 && cp.y() < entry.cachedY && idx > 0) {
      idx = idx - 1;
      charPos = 0;
      const auto &pe = m_entries[idx];
      if (pe.isTextEdit) {
        if (cp.y() >= pe.cachedY && cp.y() <= pe.cachedY + pe.cachedHeight)
          charPos = positionAt(pe.item, cp.x(), cp.y());
        else
          charPos = pe.item->property("length").toInt();
      }
    }
  }

  m_current = {idx, charPos};
  applySelection(m_anchor.entryIndex, m_anchor.charPos, m_current.entryIndex, m_current.charPos);

  if (m_flickable) {
    qreal const flickH = m_flickable->height();
    if (y < AUTO_SCROLL_MARGIN || y > flickH - AUTO_SCROLL_MARGIN)
      startAutoScroll();
    else
      stopAutoScroll();
  }
}

void TextSelectionController::handleRelease(qreal x, qreal y) {
  stopAutoScroll();

  if (!m_dragging && !m_pressLink.isEmpty() && m_mdModel) {
    QMetaObject::invokeMethod(m_mdModel, "openLink", Q_ARG(QString, m_pressLink));
  }

  m_pressLink.clear();
  m_dragging = false;
}

void TextSelectionController::handleDoubleClick(qreal x, qreal y) {
  refreshGeometry();

  auto cp = toContainerCoords(x, y);
  int const idx = entryAt(cp.x(), cp.y());
  if (idx < 0) return;

  const auto &entry = m_entries[idx];
  if (!entry.isTextEdit) return;

  int const pos = positionAt(entry.item, cp.x(), cp.y());
  entry.item->setProperty("cursorPosition", pos);
  QMetaObject::invokeMethod(entry.item, "selectWord");

  m_anchor = {idx, entry.item->property("selectionStart").toInt()};
  m_current = {idx, entry.item->property("selectionEnd").toInt()};
  setHasSelection(true);

  m_doubleClickTimer.start();
  m_doubleClickPos = {x, y};
}

void TextSelectionController::selectAll() {
  if (m_entries.empty()) return;

  for (auto &entry : m_entries) {
    if (entry.isTextEdit)
      selectAllTextEdit(entry.item);
    else
      setNonTextSelected(entry.item, true);
  }

  m_anchor = {0, 0};
  int const lastIdx = static_cast<int>(m_entries.size()) - 1;
  const auto &last = m_entries[lastIdx];
  int lastLen = 0;
  if (last.isTextEdit) lastLen = last.item->property("length").toInt();
  m_current = {lastIdx, lastLen};
  setHasSelection(true);
}

void TextSelectionController::clearSelection() {
  for (auto &entry : m_entries) {
    if (entry.isTextEdit)
      deselectTextEdit(entry.item);
    else
      setNonTextSelected(entry.item, false);
  }
  m_anchor = {-1, 0};
  m_current = {-1, 0};
  setHasSelection(false);
}

void TextSelectionController::copy() {
  if (!m_hasSelection) return;

  int fromIdx = m_anchor.entryIndex;
  int fromChar = m_anchor.charPos;
  int toIdx = m_current.entryIndex;
  int toChar = m_current.charPos;

  if (fromIdx > toIdx || (fromIdx == toIdx && fromChar > toChar)) {
    std::swap(fromIdx, toIdx);
    std::swap(fromChar, toChar);
  }

  QStringList parts;
  for (int i = fromIdx; i <= toIdx && std::cmp_less(i, m_entries.size()); ++i) {
    const auto &entry = m_entries[i];
    if (entry.isTextEdit) {
      auto text = entry.item->property("selectedText").toString();
      if (!text.isEmpty()) parts.append(text);
    } else {
      bool const selected = entry.item->property("selected").toBool();
      if (selected) {
        auto alt = entry.item->property("alt").toString();
        if (!alt.isEmpty())
          parts.append(alt);
        else
          parts.append(QStringLiteral("---"));
      }
    }
  }

  auto text = parts.join(QStringLiteral("\n\n"));
  if (!text.isEmpty()) QGuiApplication::clipboard()->setText(text);
}

void TextSelectionController::applySelection(int fromEntry, int fromChar, int toEntry, int toChar) {
  if (fromEntry < 0 || toEntry < 0) return;

  if (fromEntry > toEntry || (fromEntry == toEntry && fromChar > toChar)) {
    std::swap(fromEntry, toEntry);
    std::swap(fromChar, toChar);
  }

  bool anySelected = false;
  int const n = static_cast<int>(m_entries.size());

  for (int i = 0; i < n; ++i) {
    auto &entry = m_entries[i];

    if (i < fromEntry || i > toEntry) {
      if (entry.isTextEdit)
        deselectTextEdit(entry.item);
      else
        setNonTextSelected(entry.item, false);
    } else if (i == fromEntry && i == toEntry) {
      if (entry.isTextEdit) {
        selectTextEdit(entry.item, fromChar, toChar);
        anySelected = fromChar != toChar;
      } else {
        setNonTextSelected(entry.item, true);
        anySelected = true;
      }
    } else if (i == fromEntry) {
      if (entry.isTextEdit) {
        int const len = entry.item->property("length").toInt();
        if (fromChar >= len)
          selectAllTextEdit(entry.item);
        else
          selectTextEdit(entry.item, fromChar, len);
        anySelected = true;
      } else {
        setNonTextSelected(entry.item, true);
        anySelected = true;
      }
    } else if (i == toEntry) {
      if (entry.isTextEdit) {
        selectTextEdit(entry.item, 0, toChar);
        anySelected = true;
      } else {
        setNonTextSelected(entry.item, true);
        anySelected = true;
      }
    } else {
      if (entry.isTextEdit)
        selectAllTextEdit(entry.item);
      else
        setNonTextSelected(entry.item, true);
      anySelected = true;
    }
  }

  setHasSelection(anySelected);
}

void TextSelectionController::selectTextEdit(QQuickItem *item, int start, int end) {
  QMetaObject::invokeMethod(item, "select", Q_ARG(int, start), Q_ARG(int, end));
}

void TextSelectionController::selectAllTextEdit(QQuickItem *item) {
  QMetaObject::invokeMethod(item, "selectAll");
}

void TextSelectionController::deselectTextEdit(QQuickItem *item) {
  QMetaObject::invokeMethod(item, "deselect");
}

void TextSelectionController::setNonTextSelected(QQuickItem *item, bool selected) {
  item->setProperty("selected", selected);
}

void TextSelectionController::setHasSelection(bool has) {
  if (m_hasSelection != has) {
    m_hasSelection = has;
    emit hasSelectionChanged();
  }
}

void TextSelectionController::startAutoScroll() {
  if (!m_autoScrollTimer.isActive()) m_autoScrollTimer.start();
}

void TextSelectionController::stopAutoScroll() { m_autoScrollTimer.stop(); }

void TextSelectionController::autoScrollTick() {
  if (!m_flickable || m_anchor.entryIndex < 0) return;

  qreal const flickH = m_flickable->height();
  qreal const contentY = m_flickable->property("contentY").toReal();
  qreal const contentH = m_flickable->property("contentHeight").toReal();
  qreal const maxY = std::max(0.0, contentH - flickH);

  qreal delta = 0.0;
  if (m_mouseY < AUTO_SCROLL_MARGIN) {
    qreal const t = (AUTO_SCROLL_MARGIN - m_mouseY) / AUTO_SCROLL_MARGIN;
    delta = -AUTO_SCROLL_MAX_SPEED * t * t;
  } else if (m_mouseY > flickH - AUTO_SCROLL_MARGIN) {
    qreal const t = (m_mouseY - (flickH - AUTO_SCROLL_MARGIN)) / AUTO_SCROLL_MARGIN;
    delta = AUTO_SCROLL_MAX_SPEED * t * t;
  } else {
    stopAutoScroll();
    return;
  }

  qreal const newContentY = std::clamp(contentY + delta, 0.0, maxY);
  if (newContentY == contentY) return;

  m_flickable->setProperty("contentY", newContentY);
  refreshGeometry();

  auto cp = toContainerCoords(m_mouseX, m_mouseY);
  int idx = entryAt(cp.x(), cp.y());
  if (idx >= 0) {
    int charPos = 0;
    {
      const auto &entry = m_entries[idx];
      if (entry.isTextEdit) {
        if (cp.y() >= entry.cachedY && cp.y() <= entry.cachedY + entry.cachedHeight)
          charPos = positionAt(entry.item, cp.x(), cp.y());
        else if (cp.y() > entry.cachedY + entry.cachedHeight)
          charPos = entry.item->property("length").toInt();
      }
    }

    if (idx == m_anchor.entryIndex && m_entries[idx].isTextEdit) {
      const auto &entry = m_entries[idx];
      int const len = entry.item->property("length").toInt();
      int const n = static_cast<int>(m_entries.size());
      if (charPos >= len && cp.y() > entry.cachedY + entry.cachedHeight && idx + 1 < n) {
        idx = idx + 1;
        charPos = 0;
        const auto &ne = m_entries[idx];
        if (ne.isTextEdit && cp.y() >= ne.cachedY && cp.y() <= ne.cachedY + ne.cachedHeight)
          charPos = positionAt(ne.item, cp.x(), cp.y());
      } else if (charPos <= 0 && cp.y() < entry.cachedY && idx > 0) {
        idx = idx - 1;
        charPos = 0;
        const auto &pe = m_entries[idx];
        if (pe.isTextEdit) {
          if (cp.y() >= pe.cachedY && cp.y() <= pe.cachedY + pe.cachedHeight)
            charPos = positionAt(pe.item, cp.x(), cp.y());
          else
            charPos = pe.item->property("length").toInt();
        }
      }
    }

    m_current = {idx, charPos};
    applySelection(m_anchor.entryIndex, m_anchor.charPos, m_current.entryIndex, m_current.charPos);
  }
}

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include "editor-drop-area.hpp"

namespace Vicinae::Editor {

EditorDropArea::EditorDropArea(QQuickItem *parent) : QQuickItem(parent) { setFlag(ItemAcceptsDrops); }

bool EditorDropArea::accepts(const QDropEvent *event) const {
  return isEnabled() && m_document && event->possibleActions().testFlag(Qt::CopyAction) &&
         m_document->acceptsDrop(event->mimeData());
}

void EditorDropArea::updateDrag(const QDropEvent *event) {
  m_containsDrag = event;
  if (event) m_dragPosition = event->position();
  emit dragChanged();
}

void EditorDropArea::dragEnterEvent(QDragEnterEvent *event) { dragMoveEvent(event); }

void EditorDropArea::dragMoveEvent(QDragMoveEvent *event) {
  if (!accepts(event)) {
    event->ignore();
    updateDrag(nullptr);
    return;
  }
  updateDrag(event);
  event->setDropAction(Qt::CopyAction);
  event->accept();
}

void EditorDropArea::dragLeaveEvent(QDragLeaveEvent *) { updateDrag(nullptr); }

void EditorDropArea::dropEvent(QDropEvent *event) {
  if (accepts(event)) {
    updateDrag(event);
    const auto position = m_insertionPosition;
    emit dropping(position);
    if (m_document && m_document->drop(event->mimeData(), position)) {
      event->setDropAction(Qt::CopyAction);
      event->accept();
    } else {
      event->ignore();
    }
  } else {
    event->ignore();
  }
  updateDrag(nullptr);
}

} // namespace Vicinae::Editor

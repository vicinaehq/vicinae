#pragma once

#include <QPointer>
#include <QQuickItem>
#include "editor-document.hpp"

namespace Vicinae::Editor {

class EditorDropArea : public QQuickItem {
  Q_OBJECT
  QML_NAMED_ELEMENT(MarkdownDropArea)
  Q_PROPERTY(Vicinae::Editor::EditorDocument *document MEMBER m_document)
  Q_PROPERTY(int insertionPosition MEMBER m_insertionPosition NOTIFY insertionPositionChanged)
  Q_PROPERTY(bool containsDrag READ containsDrag NOTIFY dragChanged)
  Q_PROPERTY(QPointF dragPosition READ dragPosition NOTIFY dragChanged)

signals:
  void insertionPositionChanged();
  void dragChanged();
  void dropping(int position);

public:
  explicit EditorDropArea(QQuickItem *parent = nullptr);
  bool containsDrag() const { return m_containsDrag; }
  QPointF dragPosition() const { return m_dragPosition; }

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

private:
  bool accepts(const QDropEvent *event) const;
  void updateDrag(const QDropEvent *event);
  QPointer<EditorDocument> m_document;
  int m_insertionPosition = 0;
  bool m_containsDrag = false;
  QPointF m_dragPosition;
};

} // namespace Vicinae::Editor

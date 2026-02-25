#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <QQuickItem>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include <vector>

class TextSelectionController : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(TextSelectionController)

signals:
  void hasSelectionChanged();

public:
  Q_PROPERTY(QQuickItem *container MEMBER m_container)
  Q_PROPERTY(QQuickItem *flickable READ flickable WRITE setFlickable)
  Q_PROPERTY(QObject *mdModel MEMBER m_mdModel)
  Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)

  explicit TextSelectionController(QObject *parent = nullptr);

  bool hasSelection() const { return m_hasSelection; }
  QQuickItem *flickable() const { return m_flickable; }
  void setFlickable(QQuickItem *item);
  bool eventFilter(QObject *obj, QEvent *event) override;

  Q_INVOKABLE void registerSelectable(QQuickItem *item, int order,
                                      bool isTextEdit);
  Q_INVOKABLE void unregisterSelectable(QQuickItem *item);
  Q_INVOKABLE void handlePress(qreal x, qreal y);
  Q_INVOKABLE void handleMove(qreal x, qreal y);
  Q_INVOKABLE void handleRelease(qreal x, qreal y);
  Q_INVOKABLE void handleDoubleClick(qreal x, qreal y);
  Q_INVOKABLE void selectAll();
  Q_INVOKABLE void clearSelection();
  Q_INVOKABLE void copy();

private:
  struct SelectableEntry {
    QQuickItem *item;
    bool isTextEdit;
    int order;
    qreal cachedY;
    qreal cachedHeight;
    qreal cachedX;
    qreal cachedWidth;
  };

  struct SelectionAnchor {
    int entryIndex = -1;
    int charPos = 0;
  };

  QPointF toContainerCoords(qreal viewportX, qreal viewportY) const;
  void refreshGeometry();
  int entryAt(qreal containerX, qreal containerY) const;
  int positionAt(QQuickItem *textEdit, qreal containerX,
                 qreal containerY) const;
  QString linkAt(QQuickItem *textEdit, qreal containerX,
                 qreal containerY) const;
  void applySelection(int fromEntry, int fromChar, int toEntry, int toChar);
  void selectTextEdit(QQuickItem *item, int start, int end);
  void selectAllTextEdit(QQuickItem *item);
  void deselectTextEdit(QQuickItem *item);
  void setNonTextSelected(QQuickItem *item, bool selected);
  void setHasSelection(bool has);
  void startAutoScroll();
  void stopAutoScroll();
  void autoScrollTick();

  std::vector<SelectableEntry> m_entries;
  QQuickItem *m_container = nullptr;
  QQuickItem *m_flickable = nullptr;
  QObject *m_mdModel = nullptr;

  bool m_hasSelection = false;
  bool m_dragging = false;
  QPointF m_pressPos;
  SelectionAnchor m_anchor;
  SelectionAnchor m_current;

  QString m_pressLink;

  QTimer m_autoScrollTimer;
  qreal m_mouseX = 0;
  qreal m_mouseY = 0;

  QElapsedTimer m_doubleClickTimer;
  QPointF m_doubleClickPos;
};

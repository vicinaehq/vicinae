#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <QQuickItem>
#include <QPointer>
#include <QTimer>
#include <QtQml/qqmlregistration.h>
#include <vector>
#include <optional>
#include "document-model.hpp"

class DocumentSelection;

class DocumentController : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(DocumentController)

  Q_PROPERTY(DocumentModel *model READ model WRITE setModel NOTIFY modelChanged)
  Q_PROPERTY(QQuickItem *container MEMBER m_container)
  Q_PROPERTY(QQuickItem *flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
  Q_PROPERTY(qreal topInset MEMBER m_topInset)
  Q_PROPERTY(qreal bottomInset MEMBER m_bottomInset)
  Q_PROPERTY(QQuickItem *typingTarget READ typingTarget WRITE setTypingTarget NOTIFY typingTargetChanged)
  Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
  Q_PROPERTY(bool selecting READ selecting NOTIFY selectingChanged)

public:
  Q_INVOKABLE void selectAll();
  Q_INVOKABLE void restoreSelection();
  Q_INVOKABLE void clearSelection();
  Q_INVOKABLE void copy();

signals:
  void modelChanged();
  void hasSelectionChanged();
  void selectingChanged();
  void linkActivated(const QString &link);
  void typingTargetChanged();
  void flickableChanged();

public:
  explicit DocumentController(QObject *parent = nullptr);
  ~DocumentController() override;

  DocumentModel *model() const { return m_model; }
  void setModel(DocumentModel *model);
  bool selecting() const { return m_pressed; }
  bool hasSelection() const { return m_hasSelection; }
  QQuickItem *flickable() const { return m_flickable; }
  void setFlickable(QQuickItem *item);
  QQuickItem *typingTarget() const { return m_typingTarget; }
  void setTypingTarget(QQuickItem *item);
  bool eventFilter(QObject *obj, QEvent *event) override;

  struct ReadingAnchor {
    QPersistentModelIndex index;
    int part;
    int position;
    qreal viewportY;
  };
  std::optional<ReadingAnchor> readingAnchor(int row) const;
  std::optional<qreal> readingAnchorOffset(const ReadingAnchor &anchor) const;

private:
  void handlePress(qreal x, qreal y, bool extend = false);
  void handleMove(qreal x, qreal y);
  void handleRelease(qreal x, qreal y);
  void handleDoubleClick(qreal x, qreal y);
  friend class DocumentSelection;
  void registerSelection(DocumentSelection *selection);
  void unregisterSelection(DocumentSelection *selection);

  struct SelectableEntry {
    DocumentSelection *selection;
    qreal cachedY;
    qreal cachedHeight;
    qreal cachedX;
    qreal cachedWidth;
  };

  struct SelectionAnchor {
    int entryIndex = -1;
    int position = 0;
    QPersistentModelIndex index;
    int part = 0;
  };

  bool validAnchor() const;
  SelectionAnchor anchorFor(int entry, int position) const;
  void applyCurrentSelection(bool force = false);
  void scheduleRestore();
  QPointer<DocumentModel> m_model;
  bool m_restorePending = false;
  bool m_applyingSelection = false;
  void updateInputMethod();
  bool redirectInput(QEvent *event);
  QPointer<QQuickItem> m_typingTarget;
  QMetaObject::Connection m_typingTargetDestroyed;
  bool m_acceptedInputMethod = false;
  bool m_redirectingInput = false;

  QPointF toContainerCoords(qreal viewportX, qreal viewportY) const;
  void refreshGeometry();
  int entryAt(qreal containerX, qreal containerY) const;
  int positionAt(DocumentSelection *selection, qreal containerX, qreal containerY) const;
  QString linkAt(DocumentSelection *selection, qreal containerX, qreal containerY) const;
  void applySelection(int fromEntry, int fromPosition, int toEntry, int toPosition, bool force = false);
  void setHasSelection(bool has);
  void setPressed(bool pressed);
  void startAutoScroll();
  void stopAutoScroll();
  void autoScrollTick();

  std::vector<SelectableEntry> m_entries;
  bool m_orderDirty = false;
  int m_selectedFrom = -1;
  int m_selectedTo = -1;
  QPointer<QQuickItem> m_container;
  QPointer<QQuickItem> m_flickable;
  qreal m_topInset = 0;
  qreal m_bottomInset = 0;

  bool m_hasSelection = false;
  bool m_dragging = false;
  bool m_pressed = false;
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

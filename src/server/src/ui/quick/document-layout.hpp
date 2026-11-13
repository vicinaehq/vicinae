#pragma once

#include <QPointer>
#include <QAbstractItemModel>
#include <QElapsedTimer>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickItem>
#include <QTimer>
#include <optional>
#include <set>
#include <vector>
#include "document-controller.hpp"

class DocumentLayout : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QAbstractItemModel *model READ model WRITE setModel NOTIFY modelChanged)
  Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
  Q_PROPERTY(QQuickItem *viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
  Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
  Q_PROPERTY(qreal cacheBuffer READ cacheBuffer WRITE setCacheBuffer NOTIFY cacheBufferChanged)
  Q_PROPERTY(bool followEnd READ followEnd WRITE setFollowEnd NOTIFY followEndChanged)
  Q_PROPERTY(qreal contentHeight READ contentHeight NOTIFY contentHeightChanged)
  Q_PROPERTY(int count READ count NOTIFY countChanged)
  Q_PROPERTY(bool updating READ updating NOTIFY updatingChanged)
  Q_PROPERTY(bool embedded READ embedded WRITE setEmbedded NOTIFY embeddedChanged)
  Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
  Q_PROPERTY(QByteArray heightRole READ heightRole WRITE setHeightRole NOTIFY heightRoleChanged)
  Q_PROPERTY(QByteArray firstPartRole MEMBER m_firstPartRole NOTIFY firstPartRoleChanged)

public:
  Q_INVOKABLE void forceLayout();
  Q_INVOKABLE void invalidate();
  Q_INVOKABLE void positionAtEnd();
  Q_INVOKABLE void positionAt(int row, int part, int position, int length);

signals:
  void modelChanged();
  void delegateChanged();
  void viewportChanged();
  void spacingChanged();
  void cacheBufferChanged();
  void followEndChanged();
  void contentHeightChanged();
  void countChanged();
  void updatingChanged();
  void layoutUpdated();
  void embeddedChanged();
  void activeChanged();
  void heightRoleChanged();
  void firstPartRoleChanged();

public:
  explicit DocumentLayout(QQuickItem *parent = nullptr);
  QAbstractItemModel *model() const { return m_model; }
  void setModel(QAbstractItemModel *model);
  QQmlComponent *delegate() const { return m_delegate; }
  void setDelegate(QQmlComponent *delegate);
  QQuickItem *viewport() const { return m_viewport; }
  void setViewport(QQuickItem *viewport);
  qreal spacing() const { return m_spacing; }
  void setSpacing(qreal spacing);
  qreal cacheBuffer() const { return m_cacheBuffer; }
  void setCacheBuffer(qreal buffer);
  bool followEnd() const { return m_followEnd; }
  void setFollowEnd(bool follow);
  qreal contentHeight() const { return m_contentHeight; }
  int count() const { return static_cast<int>(m_rows.size()); }
  bool updating() const { return m_updating; }
  bool embedded() const { return m_embedded; }
  void setEmbedded(bool embedded);
  bool active() const { return m_active; }
  void setActive(bool active);
  QByteArray heightRole() const { return m_heightRole; }
  void setHeightRole(const QByteArray &role);

protected:
  void componentComplete() override;
  void updatePolish() override;

private slots:
  void scheduleLayout();
  void viewportScrolled();

private:
  struct Row {
    qreal height = 48;
    qreal y = 0;
    int index = -1;
    bool measured = false;
    QVariantMap values;
    QPointer<QQuickItem> item;
    QPointer<QQmlContext> context;
  };
  struct NestedAnchor {
    QPointer<DocumentLayout> layout;
    QPersistentModelIndex index;
    qreal viewportY;
  };
  struct Anchor {
    QPersistentModelIndex index;
    qreal offset;
    std::optional<DocumentController::ReadingAnchor> content;
    std::vector<NestedAnchor> nested;
  };

  void resetRows();
  void updateRows(int first, int last, const QList<int> &roles);
  void syncIndices();
  void rebuildGeometry();
  void layoutVisible(std::optional<Anchor> anchor = {});
  bool updateVisibleItems();
  void positionViewport(const std::optional<Anchor> &anchor);
  void measurePending();
  void readValues(int row);
  QQuickItem *createItem(int row, bool measuring);
  void releaseItem(Row &row);
  void updateProperties(int row);
  void measureItem(int row, QQuickItem *item);
  void readHeight(int row);
  void resolveHeightRole();
  void trackAncestors();
  std::optional<Anchor> readingAnchor() const;
  void restoreAnchor(const std::optional<Anchor> &anchor);
  int rowAt(qreal y) const;
  qreal minimumPosition() const;
  qreal bottomPosition() const;
  void setContentY(qreal y);
  static void polishTree(QQuickItem *item);
  DocumentController *document() const;
  void captureNestedAnchors(QQuickItem *item);
  void captureReadingAnchor(const std::optional<DocumentController::ReadingAnchor> &preferred = {});
  void notifyParentLayout();

  QPointer<QAbstractItemModel> m_model;
  QPointer<QQmlComponent> m_delegate;
  QPointer<QQuickItem> m_viewport;
  QQuickItem *m_measureContainer;
  std::vector<Row> m_rows;
  std::set<int> m_unmeasured;
  QTimer m_measureTimer;
  QElapsedTimer m_lastScroll;
  std::optional<Anchor> m_structuralAnchor;
  std::optional<DocumentController::ReadingAnchor> m_readingAnchor;
  std::vector<NestedAnchor> m_nestedAnchors;
  qreal m_spacing = 8;
  qreal m_cacheBuffer = 0;
  qreal m_contentHeight = 0;
  bool m_followEnd = false;
  bool m_positionAtEnd = false;
  bool m_updating = false;
  bool m_geometryReady = false;
  bool m_embedded = false;
  bool m_active = true;
  QByteArray m_heightRole;
  QByteArray m_firstPartRole;
  std::optional<int> m_heightRoleId;
  std::vector<QMetaObject::Connection> m_ancestorConnections;
};

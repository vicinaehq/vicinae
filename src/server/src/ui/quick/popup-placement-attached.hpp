#pragma once
#include <QObject>
#include <QPointer>
#include <QWindow>
#include <qqmlregistration.h>

class QQuickItem;
class QQuickWindow;

/**
 * Fixes native popup window (`popupType: Popup.Window`) placement on Wayland.
 *
 * QtWayland positions native popups exclusively through an xdg_positioner;
 * the popup's QML x/y are never part of it. Qt anchors the positioner to the
 * popup's parent item rect, but only its own ComboBox/Menu/ToolTip types set
 * the extended window type that selects below-the-control anchoring. A plain
 * `Popup` keeps the default top-right anchor and opens at the right edge of
 * its parent item (see QWaylandXdgSurface::createPositioner in Qt sources).
 *
 * This attached type opts a popup into the same compositor-side placement Qt
 * uses for ComboBox: anchored to the parent item, sliding to stay on screen
 * and flipping to the other side when there is no room. It uses the
 * "_q_waylandPopup*" override properties QtWayland reads when the popup
 * surface is created. On non-Wayland platforms the properties are ignored
 * and the regular QML x/y positioning applies.
 *
 * `alignment` combines a vertical flag (Qt.AlignBottom opens below the
 * parent item, Qt.AlignTop above; below is the default) with a horizontal
 * flag (Qt.AlignLeft, Qt.AlignHCenter, Qt.AlignRight) describing which
 * edges of popup and parent item line up.
 */
class PopupPlacementAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)

public:
  explicit PopupPlacementAttached(QObject *parent);

  Qt::Alignment alignment() const { return m_alignment; }
  void setAlignment(Qt::Alignment value);

signals:
  void alignmentChanged();

private slots:
  void attachToContentItem();

private:
  void onWindowChanged(QQuickWindow *window);
  void apply();

  QPointer<QQuickItem> m_contentItem;
  QPointer<QWindow> m_window;
  Qt::Alignment m_alignment = Qt::AlignLeft;
};

class PopupPlacement : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(PopupPlacement)
  QML_UNCREATABLE("")
  QML_ATTACHED(PopupPlacementAttached)

public:
  static PopupPlacementAttached *qmlAttachedProperties(QObject *object) {
    return new PopupPlacementAttached(object);
  }
};

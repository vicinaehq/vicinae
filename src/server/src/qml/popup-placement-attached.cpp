#include "popup-placement-attached.hpp"
#include <QQuickItem>
#include <QQuickWindow>
#include <QVariant>

namespace {
// xdg_positioner.constraint_adjustment bits from the xdg-shell protocol
constexpr uint32_t SLIDE_X = 1;
constexpr uint32_t SLIDE_Y = 2;
constexpr uint32_t FLIP_Y = 8;
} // namespace

PopupPlacementAttached::PopupPlacementAttached(QObject *parent) : QObject(parent) {
  // contentItem is a deferred property on QQuickPopup, so it may not exist
  // yet when the attached object is created.
  connect(parent, SIGNAL(contentItemChanged()), this, SLOT(attachToContentItem()));
  attachToContentItem();
}

void PopupPlacementAttached::attachToContentItem() {
  auto *contentItem = parent()->property("contentItem").value<QQuickItem *>();
  if (contentItem == m_contentItem) return;

  if (m_contentItem) disconnect(m_contentItem, nullptr, this, nullptr);
  m_contentItem = contentItem;
  if (!contentItem) return;

  // The popup's content item is reparented into the transient popup window
  // right before that window is shown, which is when the placement
  // properties must already be set.
  connect(contentItem, &QQuickItem::windowChanged, this, &PopupPlacementAttached::onWindowChanged);
  onWindowChanged(contentItem->window());
}

void PopupPlacementAttached::setAlignment(Qt::Alignment value) {
  if (m_alignment == value) return;
  m_alignment = value;
  emit alignmentChanged();
  apply();
}

void PopupPlacementAttached::onWindowChanged(QQuickWindow *window) {
  if (window && window->flags().testFlag(Qt::Popup)) {
    m_window = window;
    apply();
  } else {
    m_window = nullptr;
  }
}

void PopupPlacementAttached::apply() {
  if (!m_window) return;

  // The anchor is the point on the trigger item the popup attaches to, the
  // gravity the direction it grows in. A missing edge on an axis means centered
  // on that axis, so AlignCenter against the trigger rect centers the popup.
  Qt::Edges anchor;
  Qt::Edges gravity;
  if (!m_alignment.testFlag(Qt::AlignVCenter)) {
    const Qt::Edge side = m_alignment.testFlag(Qt::AlignTop) ? Qt::TopEdge : Qt::BottomEdge;
    anchor |= side;
    gravity |= side;
  }
  if (m_alignment.testFlag(Qt::AlignLeft)) {
    anchor |= Qt::LeftEdge;
    gravity |= Qt::RightEdge;
  } else if (m_alignment.testFlag(Qt::AlignRight)) {
    anchor |= Qt::RightEdge;
    gravity |= Qt::LeftEdge;
  }

  m_window->setProperty("_q_waylandPopupAnchor", QVariant::fromValue(anchor));
  m_window->setProperty("_q_waylandPopupGravity", QVariant::fromValue(gravity));
  m_window->setProperty("_q_waylandPopupConstraintAdjustment", SLIDE_X | SLIDE_Y | FLIP_Y);

  // Anchor to the trigger's real rect instead of letting QtWayland guess it
  // from the (possibly shifted/wider) popup window geometry.
  if (auto *trigger = parent()->property("parent").value<QQuickItem *>(); trigger && trigger->window()) {
    const QRect rect = trigger->mapRectToScene(trigger->boundingRect()).toRect();
    m_window->setProperty("_q_waylandPopupAnchorRect", rect);
  }
}

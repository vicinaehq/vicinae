#include <QElapsedTimer>
#include <QQmlEngine>
#include <algorithm>
#include <cmath>
#include <utility>
#include "document-layout.hpp"
#include "document-scope.hpp"

namespace vicinae::document {

namespace {
constexpr qint64 MEASUREMENT_BUDGET_MS = 4;
constexpr int MEASUREMENT_INTERVAL_MS = 16;
constexpr int SCROLL_IDLE_MS = 100;
constexpr int MAX_LAYOUT_PASSES = 8;
} // namespace

DocumentLayout::DocumentLayout(QQuickItem *parent)
    : QQuickItem(parent), m_measureContainer(new QQuickItem(this)) {
  m_nestedAnchors.reserve(4);
  m_measureContainer->setVisible(false);
  auto *scope =
      qobject_cast<DocumentScope *>(qmlAttachedPropertiesObject<DocumentScope>(m_measureContainer, true));
  scope->setDocument(nullptr);
  scope->setMeasuring(true);
  m_measureTimer.setSingleShot(true);
  connect(&m_measureTimer, &QTimer::timeout, this, &DocumentLayout::measurePending);
  connect(this, &QQuickItem::parentChanged, this, &DocumentLayout::trackAncestors);
}

void DocumentLayout::setModel(QAbstractItemModel *model) {
  if (m_model == model) return;
  if (m_model) disconnect(m_model, nullptr, this, nullptr);
  m_model = model;
  resolveHeightRole();
  if (model) {
    connect(model, &QAbstractItemModel::modelReset, this, &DocumentLayout::resetRows);
    connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this,
            [this] { m_structuralAnchor = readingAnchor(); });
    connect(model, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int first, int last) {
      if (m_rows.empty()) m_geometryReady = false;
      m_rows.insert(m_rows.begin() + first, last - first + 1, Row{});
      syncIndices();
      emit countChanged();
      layoutVisible(std::exchange(m_structuralAnchor, {}));
    });
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this,
            [this] { m_structuralAnchor = readingAnchor(); });
    connect(model, &QAbstractItemModel::rowsRemoved, this, [this](const QModelIndex &, int first, int last) {
      for (int row = first; row <= last; ++row)
        releaseItem(m_rows[row]);
      m_rows.erase(m_rows.begin() + first, m_rows.begin() + last + 1);
      syncIndices();
      emit countChanged();
      layoutVisible(std::exchange(m_structuralAnchor, {}));
    });
    connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this,
            [this] { m_structuralAnchor = readingAnchor(); });
    connect(model, &QAbstractItemModel::rowsMoved, this,
            [this](const QModelIndex &, int first, int last, const QModelIndex &, int destination) {
              if (destination > last)
                std::rotate(m_rows.begin() + first, m_rows.begin() + last + 1, m_rows.begin() + destination);
              else
                std::rotate(m_rows.begin() + destination, m_rows.begin() + first, m_rows.begin() + last + 1);
              syncIndices();
              layoutVisible(std::exchange(m_structuralAnchor, {}));
            });
    connect(model, &QAbstractItemModel::layoutChanged, this, &DocumentLayout::resetRows);
    connect(model, &QAbstractItemModel::dataChanged, this,
            [this](const QModelIndex &first, const QModelIndex &last, const QList<int> &roles) {
              updateRows(first.row(), last.row(), roles);
            });
    connect(model, &QObject::destroyed, this, &DocumentLayout::resetRows);
  }
  resetRows();
  emit modelChanged();
}

void DocumentLayout::setDelegate(QQmlComponent *delegate) {
  if (m_delegate == delegate) return;
  m_delegate = delegate;
  resetRows();
  emit delegateChanged();
}

void DocumentLayout::setViewport(QQuickItem *viewport) {
  if (m_viewport == viewport) return;
  if (m_viewport) disconnect(m_viewport, nullptr, this, nullptr);
  m_viewport = viewport;
  if (viewport) {
    connect(viewport, SIGNAL(contentYChanged()), this, SLOT(viewportScrolled()));
    connect(viewport, SIGNAL(contentHeightChanged()), this, SLOT(scheduleLayout()));
    connect(viewport, SIGNAL(topMarginChanged()), this, SLOT(scheduleLayout()));
    connect(viewport, SIGNAL(bottomMarginChanged()), this, SLOT(scheduleLayout()));
    connect(viewport, &QQuickItem::widthChanged, this, &DocumentLayout::invalidate);
    connect(viewport, &QQuickItem::heightChanged, this, &DocumentLayout::invalidate);
  }
  invalidate();
  emit viewportChanged();
}

void DocumentLayout::setSpacing(qreal spacing) {
  if (m_spacing == spacing) return;
  m_spacing = spacing;
  scheduleLayout();
  emit spacingChanged();
}

void DocumentLayout::setCacheBuffer(qreal buffer) {
  if (m_cacheBuffer == buffer) return;
  m_cacheBuffer = buffer;
  scheduleLayout();
  emit cacheBufferChanged();
}

void DocumentLayout::setFollowEnd(bool follow) {
  if (m_followEnd == follow) return;
  m_followEnd = follow;
  forceLayout();
  emit followEndChanged();
}

void DocumentLayout::componentComplete() {
  QQuickItem::componentComplete();
  trackAncestors();
  scheduleLayout();
}

void DocumentLayout::setEmbedded(bool embedded) {
  if (m_embedded == embedded) return;
  m_embedded = embedded;
  trackAncestors();
  resetRows();
  emit embeddedChanged();
}

void DocumentLayout::trackAncestors() {
  for (const auto &connection : m_ancestorConnections)
    disconnect(connection);
  m_ancestorConnections.clear();
  if (!m_embedded) return;
  m_ancestorConnections.reserve(24);
  for (auto *ancestor = parentItem(); ancestor; ancestor = ancestor->parentItem()) {
    m_ancestorConnections.emplace_back(
        connect(ancestor, &QQuickItem::yChanged, this, &DocumentLayout::scheduleLayout));
    m_ancestorConnections.emplace_back(
        connect(ancestor, &QQuickItem::parentChanged, this, &DocumentLayout::trackAncestors));
  }
  scheduleLayout();
}

void DocumentLayout::setActive(bool active) {
  if (m_active == active) return;
  m_active = active;
  scheduleLayout();
  emit activeChanged();
}

void DocumentLayout::setHeightRole(const QByteArray &role) {
  if (m_heightRole == role) return;
  m_heightRole = role;
  resolveHeightRole();
  invalidate();
  emit heightRoleChanged();
}

void DocumentLayout::resolveHeightRole() {
  m_heightRoleId.reset();
  if (m_model) {
    const auto names = m_model->roleNames();
    for (auto it = names.begin(); it != names.end(); ++it)
      if (it.value() == m_heightRole) m_heightRoleId = it.key();
  }
}

void DocumentLayout::readHeight(int row) {
  if (!m_heightRoleId || !m_model) return;
  m_rows[row].height = std::max(qreal(0), m_model->data(m_model->index(row, 0), *m_heightRoleId).toReal());
  m_rows[row].measured = true;
  m_rows[row].index = row;
  m_unmeasured.erase(row);
}

void DocumentLayout::resetRows() {
  m_measureTimer.stop();
  for (auto &row : m_rows)
    releaseItem(row);
  m_rows.clear();
  m_geometryReady = false;
  m_rows.reserve(m_model ? m_model->rowCount() : 0);
  m_rows.resize(m_model ? m_model->rowCount() : 0);
  m_structuralAnchor.reset();
  m_readingAnchor.reset();
  m_nestedAnchors.clear();
  invalidate();
  emit countChanged();
}

void DocumentLayout::syncIndices() {
  m_unmeasured.clear();
  for (int row = 0; row < count(); ++row) {
    if (m_rows[row].index != row) m_rows[row].measured = false;
    readHeight(row);
    if (!m_rows[row].measured) m_unmeasured.insert(row);
    if (auto *item = m_rows[row].item.data()) {
      item->setProperty("index", row);
      if (m_rows[row].context) m_rows[row].context->setContextProperty("index", row);
    }
  }
}

void DocumentLayout::invalidate() {
  m_unmeasured.clear();
  for (int row = 0; row < count(); ++row) {
    m_rows[row].measured = false;
    m_unmeasured.insert(row);
    readHeight(row);
  }
  scheduleLayout();
}

void DocumentLayout::updateRows(int first, int last, const QList<int> &roles) {
  if (!m_model) return;
  const auto names = m_model->roleNames();
  for (int row = first; row <= last && row < count(); ++row) {
    auto &entry = m_rows[row];
    if (entry.values.isEmpty()) {
      m_unmeasured.insert(row);
      readHeight(row);
      continue;
    }
    bool changed = false;
    for (auto it = names.begin(); it != names.end(); ++it) {
      if (!roles.empty() && !roles.contains(it.key())) continue;
      const auto name = QString::fromUtf8(it.value());
      const auto value = m_model->data(m_model->index(row, 0), it.key());
      if (entry.values.value(name) == value) continue;
      entry.values[name] = value;
      changed = true;
    }
    if (changed) {
      entry.measured = false;
      m_unmeasured.insert(row);
      readHeight(row);
      if (entry.item) updateProperties(row);
    }
  }
  scheduleLayout();
}

void DocumentLayout::readValues(int row) {
  auto &values = m_rows[row].values;
  if (!values.isEmpty()) return;
  const auto names = m_model->roleNames();
  for (auto it = names.begin(); it != names.end(); ++it)
    values.insert(QString::fromUtf8(it.value()), m_model->data(m_model->index(row, 0), it.key()));
}

QQuickItem *DocumentLayout::createItem(int row, bool measuring) {
  readValues(row);
  auto values = m_rows[row].values;
  values.insert("index", row);
  values.insert("model", m_rows[row].values);
  auto *creationContext = m_delegate->creationContext() ? m_delegate->creationContext() : qmlContext(this);
  auto *context = m_delegate->isBound() ? nullptr : new QQmlContext(creationContext, this);
  if (context) {
    for (auto it = values.begin(); it != values.end(); ++it)
      context->setContextProperty(it.key(), it.value());
  }
  auto *object = m_delegate->beginCreate(context ? context : creationContext);
  auto *item = qobject_cast<QQuickItem *>(object);
  if (!item) {
    m_delegate->completeCreate();
    if (object) object->deleteLater();
    if (context) context->deleteLater();
    qWarning() << "Document delegate must create an Item" << m_delegate->errors();
    return nullptr;
  }
  item->setParent(this);
  if (context) context->setParent(item);
  item->setParentItem(measuring    ? m_measureContainer
                      : m_embedded ? this
                                   : m_viewport->property("contentItem").value<QQuickItem *>());
  QVariantMap initial;
  for (auto it = values.begin(); it != values.end(); ++it)
    if (item->metaObject()->indexOfProperty(it.key().toUtf8()) >= 0) initial.insert(it.key(), it.value());
  m_delegate->setInitialProperties(item, initial);
  m_delegate->completeCreate();
  if (!measuring) {
    m_rows[row].item = item;
    m_rows[row].context = context;
    connect(item, &QQuickItem::heightChanged, this, &DocumentLayout::scheduleLayout);
  }
  return item;
}

void DocumentLayout::updateProperties(int row) {
  auto &entry = m_rows[row];
  for (auto it = entry.values.begin(); it != entry.values.end(); ++it) {
    if (entry.context) entry.context->setContextProperty(it.key(), it.value());
    if (entry.item->metaObject()->indexOfProperty(it.key().toUtf8()) >= 0)
      entry.item->setProperty(it.key().toUtf8(), it.value());
  }
  if (entry.context) entry.context->setContextProperty("model", entry.values);
  if (entry.item->metaObject()->indexOfProperty("model") >= 0) entry.item->setProperty("model", entry.values);
}

void DocumentLayout::releaseItem(Row &row) {
  if (!row.item) return;
  disconnect(row.item, nullptr, this, nullptr);
  row.item->setParentItem(nullptr);
  row.item->deleteLater();
  row.item = nullptr;
  row.context = nullptr;
}

void DocumentLayout::polishTree(QQuickItem *item) {
  item->ensurePolished();
  for (auto *child : item->childItems())
    polishTree(child);
  item->ensurePolished();
}

void DocumentLayout::measureItem(int row, QQuickItem *item) {
  if (m_heightRoleId) return;
  polishTree(item);
  const auto height = item->height();
  if (std::isfinite(height)) m_rows[row].height = std::max(qreal(0), height);
  m_rows[row].measured = true;
  m_rows[row].index = row;
  m_unmeasured.erase(row);
}

void DocumentLayout::rebuildGeometry() {
  qreal y = 0;
  for (auto &row : m_rows) {
    row.y = y;
    y += row.height + m_spacing;
    if (row.item) row.item->setY(row.y);
  }
  const auto total = std::max(qreal(0), y - (m_rows.empty() ? 0 : m_spacing));
  if (total != m_contentHeight) {
    m_contentHeight = total;
    emit contentHeightChanged();
  }
}

int DocumentLayout::rowAt(qreal y) const {
  if (m_rows.empty()) return -1;
  auto it = std::ranges::upper_bound(m_rows, y, {}, &Row::y);
  return std::max(0, static_cast<int>(std::distance(m_rows.begin(), it)) - 1);
}

DocumentController *DocumentLayout::document() const {
  if (!m_viewport || m_embedded) return nullptr;
  auto *scope = qobject_cast<DocumentScope *>(qmlAttachedPropertiesObject<DocumentScope>(m_viewport, false));
  return scope ? scope->document() : nullptr;
}

std::optional<DocumentLayout::Anchor> DocumentLayout::readingAnchor() const {
  if (m_embedded || !m_geometryReady || !m_viewport || !m_model || m_rows.empty()) return {};
  const auto y = m_viewport->property("contentY").toReal();
  const int row = rowAt(y + m_viewport->property("topMargin").toReal());
  const auto index = m_model->index(row, 0);
  return Anchor{index, y - m_rows[row].y,
                m_readingAnchor && m_readingAnchor->index == index ? m_readingAnchor : std::nullopt,
                m_nestedAnchors};
}

void DocumentLayout::restoreAnchor(const std::optional<Anchor> &anchor) {
  auto y = m_viewport->property("contentY").toReal();
  if (anchor && anchor->index.isValid() && anchor->index.row() < count()) {
    y = m_rows[anchor->index.row()].y + anchor->offset;
    for (const auto &nested : anchor->nested) {
      auto *layout = nested.layout.data();
      if (!layout || !nested.index.isValid() || nested.index.row() >= layout->count()) continue;
      layout->rebuildGeometry();
      const auto top = layout->mapToItem(m_viewport, QPointF(0, layout->m_rows[nested.index.row()].y)).y();
      // Keep the anchor row mounted when reflow makes it shorter than its previous offset.
      const auto visibleY = std::max(nested.viewportY, 1 - layout->m_rows[nested.index.row()].height);
      y = m_viewport->property("contentY").toReal() + top - visibleY;
    }
    if (anchor->content) {
      if (!anchor->nested.empty()) {
        setContentY(y);
        for (const auto &nested : anchor->nested)
          if (nested.layout) nested.layout->forceLayout();
      }
      if (auto *controller = document()) {
        if (const auto offset = controller->readingAnchorOffset(*anchor->content))
          y = m_viewport->property("contentY").toReal() + *offset;
      }
    }
  }
  setContentY(y);
}

qreal DocumentLayout::minimumPosition() const { return -m_viewport->property("topMargin").toReal(); }

qreal DocumentLayout::bottomPosition() const {
  const auto minimum = minimumPosition();
  const auto height = m_viewport->property("contentHeight").toReal();
  return std::max(minimum, height - m_viewport->height() + m_viewport->property("bottomMargin").toReal());
}

void DocumentLayout::setContentY(qreal y) {
  m_viewport->setProperty("contentY", std::clamp(y, minimumPosition(), bottomPosition()));
}

void DocumentLayout::positionViewport(const std::optional<Anchor> &anchor) {
  if (m_embedded) return;
  if (m_followEnd || m_positionAtEnd)
    setContentY(bottomPosition());
  else
    restoreAnchor(anchor);
}

void DocumentLayout::captureNestedAnchors(QQuickItem *item) {
  if (auto *layout = qobject_cast<DocumentLayout *>(item);
      layout && layout->m_embedded && layout->m_active && layout->m_geometryReady &&
      layout->m_viewport == m_viewport && !layout->m_rows.empty()) {
    const auto localY = layout->mapFromItem(m_viewport, QPointF()).y();
    if (localY >= 0 && localY < layout->m_contentHeight) {
      const int row = layout->rowAt(localY);
      m_nestedAnchors.emplace_back(layout, layout->m_model->index(row, 0),
                                   layout->mapToItem(m_viewport, QPointF(0, layout->m_rows[row].y)).y());
    }
  }
  for (auto *child : item->childItems())
    captureNestedAnchors(child);
}

void DocumentLayout::captureReadingAnchor(const std::optional<DocumentController::ReadingAnchor> &preferred) {
  m_readingAnchor.reset();
  m_nestedAnchors.clear();
  if (m_embedded || m_viewport->property("contentY").toReal() <= minimumPosition() + 1) return;
  const auto y = m_viewport->property("contentY").toReal();
  const int row = rowAt(y + m_viewport->property("topMargin").toReal());
  if (auto *controller = document()) {
    if (preferred) {
      if (const auto offset = controller->readingAnchorOffset(*preferred); offset && std::abs(*offset) < 0.5)
        m_readingAnchor = preferred;
    }
    if (!m_readingAnchor) m_readingAnchor = controller->readingAnchor(row);
  }
  if (row >= 0 && m_rows[row].item) captureNestedAnchors(m_rows[row].item);
}

void DocumentLayout::viewportScrolled() {
  if (!m_updating) {
    m_lastScroll.start();
    m_readingAnchor.reset();
    m_nestedAnchors.clear();
  }
  scheduleLayout();
}

void DocumentLayout::scheduleLayout() {
  if (!m_updating) polish();
}

bool DocumentLayout::updateVisibleItems() {
  const auto y = m_embedded ? (m_viewport ? mapFromItem(m_viewport, QPointF()).y() : 0)
                            : m_viewport->property("contentY").toReal();
  const auto viewHeight = m_viewport ? m_viewport->height() : m_contentHeight;
  const bool outside = !m_active || y - m_cacheBuffer > m_contentHeight || y + viewHeight + m_cacheBuffer < 0;
  const int first = outside ? -1 : rowAt(y - m_cacheBuffer);
  const int last = outside ? -1 : rowAt(y + viewHeight + m_cacheBuffer);
  bool changed = false;
  for (int row = 0; row < count(); ++row) {
    auto &entry = m_rows[row];
    if (row < first || row > last) {
      releaseItem(entry);
      continue;
    }
    if (!entry.item) {
      createItem(row, false);
      entry.measured = m_heightRoleId.has_value();
    }
    if (entry.item && (!entry.measured || entry.item->height() != entry.height)) {
      const auto previous = entry.height;
      measureItem(row, entry.item);
      changed |= previous != entry.height;
    }
  }
  return changed;
}

void DocumentLayout::notifyParentLayout() {
  if (m_embedded) {
    // Embedded rows may finish polishing after the outer layout captured its anchor.
    for (auto *ancestor = parent(); ancestor; ancestor = ancestor->parent()) {
      auto *layout = qobject_cast<DocumentLayout *>(ancestor);
      if (layout && !layout->m_embedded && layout->m_viewport == m_viewport) {
        if (!layout->m_updating && !layout->m_readingAnchor) layout->captureReadingAnchor();
        break;
      }
    }
  }
}

void DocumentLayout::layoutVisible(std::optional<Anchor> anchor) {
  if (m_updating || !isComponentComplete() || (!m_viewport && !m_embedded) || !m_model || !m_delegate ||
      !m_delegate->isReady())
    return;
  m_updating = true;
  emit updatingChanged();
  if (!anchor) anchor = readingAnchor();
  m_measureContainer->setWidth(m_embedded ? width() : m_viewport->width());
  for (int pass = 0; pass < MAX_LAYOUT_PASSES; ++pass) {
    rebuildGeometry();
    positionViewport(anchor);
    if (!updateVisibleItems()) break;
  }
  rebuildGeometry();
  positionViewport(anchor);
  m_geometryReady = true;
  const bool preserveReading = anchor && !m_followEnd && !m_positionAtEnd;
  captureReadingAnchor(preserveReading ? anchor->content : std::nullopt);
  m_positionAtEnd = false;
  m_updating = false;
  emit updatingChanged();
  emit layoutUpdated();
  notifyParentLayout();
  if (!m_unmeasured.empty() && !m_measureTimer.isActive()) m_measureTimer.start(MEASUREMENT_INTERVAL_MS);
}

void DocumentLayout::measurePending() {
  if (!m_viewport || !m_model || !m_delegate || !m_delegate->isReady() || m_updating) return;
  if (m_lastScroll.isValid() && m_lastScroll.elapsed() < SCROLL_IDLE_MS) {
    m_measureTimer.start(SCROLL_IDLE_MS - m_lastScroll.elapsed());
    return;
  }
  const auto anchor = readingAnchor();
  QElapsedTimer budget;
  budget.start();
  while (!m_unmeasured.empty() && budget.elapsed() < MEASUREMENT_BUDGET_MS) {
    const int row = *m_unmeasured.begin();
    auto *item = m_rows[row].item.data();
    const bool temporary = !item;
    if (!item) item = createItem(row, true);
    if (item) {
      measureItem(row, item);
      if (temporary) {
        // Keep the measurement scope until deletion so images stay unloaded.
        item->deleteLater();
      }
    } else {
      m_unmeasured.erase(row);
    }
  }
  layoutVisible(anchor);
}

void DocumentLayout::updatePolish() { layoutVisible(); }
void DocumentLayout::forceLayout() { layoutVisible(); }

void DocumentLayout::positionAtEnd() {
  m_positionAtEnd = true;
  forceLayout();
}

void DocumentLayout::positionAt(int row, int part, int position, int length) {
  if (m_embedded || !m_viewport || !m_model || row < 0 || row >= count()) return;
  m_positionAtEnd = false;
  QMetaObject::invokeMethod(m_viewport, "cancelFlick");
  const auto inset = m_viewport->property("topInset").toReal() + 12;
  if (auto *controller = document()) {
    if (const auto rect = controller->positionRectangle(row, part, position)) {
      controller->revealHorizontalPosition(row, part, position, length);
      const auto bottom = m_viewport->height() - m_viewport->property("bottomInset").toReal() - 12;
      if (rect->top() < inset || rect->bottom() > bottom) {
        setContentY(m_viewport->property("contentY").toReal() + rect->top() - inset);
        forceLayout();
      }
      return;
    }
  }
  layoutVisible(Anchor{m_model->index(row, 0), -inset, {}, {}});
  auto *item = m_rows[row].item.data();
  if (!item) return;
  polishTree(item);
  for (auto *nested : item->findChildren<DocumentLayout *>()) {
    if (!nested->m_embedded || !nested->m_model || nested->m_firstPartRole.isEmpty()) continue;
    const int role = nested->m_model->roleNames().key(nested->m_firstPartRole, -1);
    if (role < 0) continue;
    const auto rows = std::views::iota(0, nested->count());
    const auto after = std::ranges::upper_bound(rows, part, {}, [nested, role](int index) {
      return nested->m_model->data(nested->m_model->index(index, 0), role).toInt();
    });
    const int nestedRow = int(std::ranges::distance(rows.begin(), after)) - 1;
    if (nestedRow < 0) continue;
    nested->rebuildGeometry();
    const auto top = nested->mapToItem(m_viewport, QPointF(0, nested->m_rows[nestedRow].y)).y();
    setContentY(m_viewport->property("contentY").toReal() + top - inset);
    nested->forceLayout();
    polishTree(item);
  }
  if (auto *controller = document()) {
    controller->revealHorizontalPosition(row, part, position, length);
    if (const auto rect = controller->positionRectangle(row, part, position))
      setContentY(m_viewport->property("contentY").toReal() + rect->top() - inset);
  }
  forceLayout();
}

} // namespace vicinae::document

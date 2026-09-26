#include "document-scope.hpp"

namespace vicinae::document {

DocumentScope::DocumentScope(QObject *parent) : QQuickAttachedPropertyPropagator(parent) {
  if (auto *item = qobject_cast<QQuickItem *>(parent)) {
    // Intermediate Items need a scope too, so moving a reusable subtree updates its descendants.
    if (item->parentItem()) qmlAttachedPropertiesObject<DocumentScope>(item->parentItem(), true);
    connect(item, &QQuickItem::parentChanged, this, [this](QQuickItem *parent) {
      if (parent) qmlAttachedPropertiesObject<DocumentScope>(parent, true);
      initialize();
    });
  }
  initialize();
}

void DocumentScope::setDocument(DocumentController *document) {
  m_explicitDocument = true;
  disconnect(m_destroyedConnection);
  if (document) {
    m_destroyedConnection = connect(document, &QObject::destroyed, this, [this] {
      emit documentChanged();
      propagateDocument(true);
    });
  }
  updateDocument(document);
}

void DocumentScope::resetDocument() {
  m_explicitDocument = false;
  disconnect(m_destroyedConnection);
  const auto *scope = qobject_cast<DocumentScope *>(attachedParent());
  inheritDocument(scope ? scope->document() : nullptr);
  inheritRow(scope ? scope->row() : -1);
}

void DocumentScope::attachedParentChange(QQuickAttachedPropertyPropagator *parent,
                                         QQuickAttachedPropertyPropagator *) {
  const auto *scope = qobject_cast<DocumentScope *>(parent);
  inheritDocument(scope ? scope->document() : nullptr);
  inheritRow(scope ? scope->row() : -1);
  setMeasuring(scope && scope->measuring());
  inheritStyle(scope ? scope->style() : nullptr);
}

void DocumentScope::setMeasuring(bool measuring) {
  if (m_measuring == measuring) return;
  m_measuring = measuring;
  emit measuringChanged();
  for (auto *child : attachedChildren()) {
    if (auto *scope = qobject_cast<DocumentScope *>(child)) scope->setMeasuring(measuring);
  }
}

void DocumentScope::inheritDocument(DocumentController *document, bool force) {
  if (!m_explicitDocument) updateDocument(document, force);
}

void DocumentScope::updateDocument(DocumentController *document, bool force) {
  if (m_document == document && !force) return;
  m_document = document;
  emit documentChanged();
  propagateDocument(force);
}

void DocumentScope::propagateDocument(bool force) {
  for (auto *child : attachedChildren()) {
    if (auto *scope = qobject_cast<DocumentScope *>(child)) scope->inheritDocument(m_document, force);
  }
}

void DocumentScope::setRow(int row) {
  m_explicitRow = true;
  updateRow(row);
}

void DocumentScope::resetRow() {
  m_explicitRow = false;
  const auto *scope = qobject_cast<DocumentScope *>(attachedParent());
  inheritRow(scope ? scope->row() : -1);
}

void DocumentScope::inheritRow(int row) {
  if (!m_explicitRow) updateRow(row);
}

void DocumentScope::updateRow(int row) {
  if (m_row == row) return;
  m_row = row;
  emit rowChanged();
  for (auto *child : attachedChildren()) {
    if (auto *scope = qobject_cast<DocumentScope *>(child)) scope->inheritRow(row);
  }
}

void DocumentScope::setStyle(DocumentStyle *style) {
  m_explicitStyle = true;
  updateStyle(style);
}

void DocumentScope::resetStyle() {
  m_explicitStyle = false;
  const auto *scope = qobject_cast<DocumentScope *>(attachedParent());
  inheritStyle(scope ? scope->style() : nullptr);
}

void DocumentScope::inheritStyle(DocumentStyle *style) {
  if (!m_explicitStyle) updateStyle(style);
}

void DocumentScope::updateStyle(DocumentStyle *style) {
  if (!style) style = DocumentStyle::defaults();
  if (m_style == style) return;
  disconnect(m_styleDestroyed);
  m_style = style;
  m_styleDestroyed = connect(style, &QObject::destroyed, this, [this] { resetStyle(); });
  emit styleChanged();
  for (auto *child : attachedChildren())
    if (auto *scope = qobject_cast<DocumentScope *>(child)) scope->inheritStyle(this->style());
}

} // namespace vicinae::document

#include <QMetaMethod>
#include <QMetaProperty>
#include <QQmlInfo>
#include <algorithm>
#include "document-selection.hpp"
#include "document-scope.hpp"

DocumentSelection::DocumentSelection(QObject *parent) : QObject(parent) {}

DocumentSelection::~DocumentSelection() { detach(); }

void DocumentSelection::classBegin() { m_complete = false; }

void DocumentSelection::componentComplete() {
  if (!m_explicitTarget) setTarget(qobject_cast<QQuickItem *>(parent()));
  m_complete = true;
  updateRegistration();
}

void DocumentSelection::setDocument(DocumentController *document) {
  m_explicitDocument = true;
  updateDocument(document);
}

void DocumentSelection::resetDocument() {
  m_explicitDocument = false;
  updateScope();
}

void DocumentSelection::updateDocument(DocumentController *document) {
  if (m_document == document) return;
  detach();
  if (m_document) disconnect(m_document, nullptr, this, nullptr);
  m_document = document;
  if (document) {
    connect(document, &QObject::destroyed, this, [this] {
      detach();
      m_document = nullptr;
      emit documentChanged();
    });
  }
  emit documentChanged();
  updateRegistration();
}

void DocumentSelection::setTarget(QQuickItem *target) {
  m_explicitTarget = true;
  if (m_target == target) return;
  detach();
  if (m_target) disconnect(m_target, nullptr, this, nullptr);
  m_target = target;
  updateScope();
  if (target) {
    connect(target, &QQuickItem::visibleChanged, this, &DocumentSelection::updateRegistration);
    connect(target, &QObject::destroyed, this, [this] {
      detach();
      m_target = nullptr;
      updateScope();
      emit targetChanged();
    });
  }
  emit targetChanged();
  updateRegistration();
}

void DocumentSelection::updateScope() {
  disconnect(m_scopeConnection);
  disconnect(m_rowConnection);
  m_scope = nullptr;
  if (m_target) {
    m_scope = qobject_cast<DocumentScope *>(qmlAttachedPropertiesObject<DocumentScope>(m_target, true));
    if (m_scope) {
      m_rowConnection = connect(m_scope, &DocumentScope::rowChanged, this, [this] {
        if (m_registeredDocument) m_registeredDocument->scheduleRestore();
      });
      m_scopeConnection = connect(m_scope, &DocumentScope::documentChanged, this, [this] {
        if (!m_explicitDocument) updateDocument(m_scope ? m_scope->document() : nullptr);
      });
    }
  }
  if (!m_explicitDocument) updateDocument(m_scope ? m_scope->document() : nullptr);
}

void DocumentSelection::setEnabled(bool enabled) {
  if (m_enabled == enabled) return;
  m_enabled = enabled;
  emit enabledChanged();
  updateRegistration();
}

void DocumentSelection::setLength(int length) {
  length = std::max(0, length);
  if (m_length == length) return;
  m_length = length;
  select(m_start, m_end);
  emit lengthChanged();
  if (m_registeredDocument) m_registeredDocument->restoreSelection();
}

void DocumentSelection::setSelectedText(const QString &text) {
  if (m_selectedText == text) return;
  m_selectedText = text;
  emit selectedTextChanged();
}

void DocumentSelection::updateRegistration() {
  if (!m_complete || !m_document || !m_target || !m_enabled || !m_target->isVisible()) {
    detach();
    return;
  }
  if (m_registeredDocument == m_document) return;
  activate();
  m_registeredDocument = m_document;
  m_registeredDocument->registerSelection(this);
}

void DocumentSelection::detach() {
  if (m_registeredDocument) m_registeredDocument->unregisterSelection(this);
  m_registeredDocument = nullptr;
  select(0, 0);
  deactivate();
}

int DocumentSelection::positionAt(const QPointF &position) const {
  if (m_hitTest.isCallable()) {
    const auto result = m_hitTest.call({position.x(), position.y()});
    if (result.isNumber()) return std::clamp(result.toInt(), 0, length());
    qmlWarning(this) << "hitTest must return a selection position:" << result.toString();
  }
  return target() && position.x() >= target()->width() / 2 ? length() : 0;
}

QRectF DocumentSelection::rectangleAt(int position) const {
  if (m_positionRectangle.isCallable()) {
    const auto result = m_positionRectangle.call({position});
    if (result.isObject() && result.property("x").isNumber() && result.property("y").isNumber() &&
        result.property("width").isNumber() && result.property("height").isNumber())
      return {result.property("x").toNumber(), result.property("y").toNumber(),
              result.property("width").toNumber(), result.property("height").toNumber()};
    qmlWarning(this) << "positionRectangle must return {x, y, width, height}:" << result.toString();
  }
  return target() ? target()->boundingRect() : QRectF{};
}

std::pair<int, int> DocumentSelection::wordAt(int position) const {
  if (m_wordRange.isCallable()) {
    const auto result = m_wordRange.call({position});
    if (result.isObject() && result.property("start").isNumber() && result.property("end").isNumber()) {
      const int start = std::clamp(result.property("start").toInt(), 0, length());
      const int end = std::clamp(result.property("end").toInt(), 0, length());
      return {std::min(start, end), std::max(start, end)};
    }
    qmlWarning(this) << "wordRange must return {start, end}:" << result.toString();
  }
  return {0, length()};
}

QString DocumentSelection::linkAtPosition(const QPointF &position) const {
  if (!m_linkAt.isCallable()) return {};
  const auto result = m_linkAt.call({position.x(), position.y()});
  return result.isString() ? result.toString() : QString{};
}

void DocumentSelection::select(int start, int end, bool force) {
  start = std::clamp(start, 0, length());
  end = std::clamp(end, 0, length());
  if (start > end) std::swap(start, end);
  const bool changed = m_start != start || m_end != end;
  if (!changed && !force) return;
  m_start = start;
  m_end = end;
  applySelection(start, end);
  if (changed) emit selectionChanged();
}

DocumentTextSelection::DocumentTextSelection(QObject *parent) : DocumentSelection(parent) {}

DocumentTextSelection::~DocumentTextSelection() {
  select(0, 0);
  deactivate();
}

int DocumentTextSelection::length() const { return target() ? target()->property("length").toInt() : 0; }

QString DocumentTextSelection::selectedText() const {
  return target() ? target()->property("selectedText").toString() : QString{};
}

int DocumentTextSelection::positionAt(const QPointF &position) const {
  int result = 0;
  if (target())
    QMetaObject::invokeMethod(target(), "positionAt", Q_RETURN_ARG(int, result), Q_ARG(double, position.x()),
                              Q_ARG(double, position.y()));
  return std::clamp(result, 0, length());
}

QRectF DocumentTextSelection::rectangleAt(int position) const {
  QRectF result;
  if (target())
    QMetaObject::invokeMethod(target(), "positionToRectangle", Q_RETURN_ARG(QRectF, result),
                              Q_ARG(int, position));
  return result;
}

std::pair<int, int> DocumentTextSelection::wordAt(int position) const {
  if (!target()) return {0, 0};
  target()->setProperty("cursorPosition", position);
  QMetaObject::invokeMethod(target(), "selectWord");
  return {target()->property("selectionStart").toInt(), target()->property("selectionEnd").toInt()};
}

QString DocumentTextSelection::linkAtPosition(const QPointF &position) const {
  QString result;
  if (target())
    QMetaObject::invokeMethod(target(), "linkAt", Q_RETURN_ARG(QString, result), Q_ARG(double, position.x()),
                              Q_ARG(double, position.y()));
  return result;
}

void DocumentTextSelection::applySelection(int start, int end) {
  if (!target()) return;
  if (target()->property("selectionStart").toInt() == start &&
      target()->property("selectionEnd").toInt() == end)
    return;
  QMetaObject::invokeMethod(target(), "select", Q_ARG(int, start), Q_ARG(int, end));
}

void DocumentTextSelection::activate() {
  if (!target()) return;
  m_activeTarget = target();
  m_mouseButtons = target()->acceptedMouseButtons();
  m_focusOnTab = target()->activeFocusOnTab();
  target()->setAcceptedMouseButtons(Qt::NoButton);
  target()->setActiveFocusOnTab(false);
  const auto forwardSignal = [this](const char *property, const char *signal) {
    const auto source = target()->metaObject()->property(target()->metaObject()->indexOfProperty(property));
    if (source.hasNotifySignal())
      connect(target(), source.notifySignal(), this,
              metaObject()->method(metaObject()->indexOfSignal(signal)), Qt::UniqueConnection);
  };
  forwardSignal("length", "lengthChanged()");
  forwardSignal("selectedText", "selectedTextChanged()");
  const auto text = target()->metaObject()->property(target()->metaObject()->indexOfProperty("text"));
  if (text.hasNotifySignal())
    connect(target(), text.notifySignal(), this,
            metaObject()->method(metaObject()->indexOfSlot("restoreTextSelection()")), Qt::UniqueConnection);
  emit lengthChanged();
  emit selectedTextChanged();
}

void DocumentTextSelection::deactivate() {
  if (!m_activeTarget) return;
  m_activeTarget->setAcceptedMouseButtons(m_mouseButtons);
  m_activeTarget->setActiveFocusOnTab(m_focusOnTab);
  m_activeTarget = nullptr;
}

int DocumentSelection::row() const { return m_scope ? m_scope->row() : -1; }

void DocumentSelection::setPart(int part) {
  if (m_part == part) return;
  m_part = part;
  emit partChanged();
  if (m_registeredDocument) m_registeredDocument->scheduleRestore();
}

void DocumentTextSelection::restoreTextSelection() {
  if (m_activeTarget && document() && document()->model()) document()->restoreSelection();
}

#pragma once

#include <QJSValue>
#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>
#include <QQuickItem>
#include <QtQml/qqmlregistration.h>
#include <utility>
#include "document-controller.hpp"

class DocumentScope;

class DocumentSelection : public QObject, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(
      DocumentController *document READ document WRITE setDocument RESET resetDocument NOTIFY documentChanged)
  Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
  Q_PROPERTY(QString selectedText READ selectedText WRITE setSelectedText NOTIFY selectedTextChanged)
  Q_PROPERTY(int selectionStart READ selectionStart NOTIFY selectionChanged)
  Q_PROPERTY(int selectionEnd READ selectionEnd NOTIFY selectionChanged)
  Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectionChanged)
  Q_PROPERTY(int part READ part WRITE setPart NOTIFY partChanged)
  Q_PROPERTY(QJSValue hitTest READ hitTest WRITE setHitTest NOTIFY callbacksChanged)
  Q_PROPERTY(
      QJSValue positionRectangle READ positionRectangle WRITE setPositionRectangle NOTIFY callbacksChanged)
  Q_PROPERTY(QJSValue wordRange READ wordRange WRITE setWordRange NOTIFY callbacksChanged)
  Q_PROPERTY(QJSValue linkAt READ linkAt WRITE setLinkAt NOTIFY callbacksChanged)

signals:
  void documentChanged();
  void targetChanged();
  void enabledChanged();
  void lengthChanged();
  void selectedTextChanged();
  void selectionChanged();
  void callbacksChanged();
  void partChanged();

public:
  explicit DocumentSelection(QObject *parent = nullptr);
  ~DocumentSelection() override;

  DocumentController *document() const { return m_document; }
  QQuickItem *target() const { return m_target; }
  int row() const;
  int part() const { return m_part; }
  void setPart(int part);
  bool enabled() const { return m_enabled; }
  virtual int length() const { return m_length; }
  virtual QString selectedText() const { return m_selectedText; }
  int selectionStart() const { return m_start; }
  int selectionEnd() const { return m_end; }
  bool hasSelection() const { return m_start != m_end; }
  QJSValue hitTest() const { return m_hitTest; }
  QJSValue positionRectangle() const { return m_positionRectangle; }
  void setPositionRectangle(QJSValue callback) {
    m_positionRectangle = std::move(callback);
    emit callbacksChanged();
  }
  QJSValue wordRange() const { return m_wordRange; }
  QJSValue linkAt() const { return m_linkAt; }

  void setHitTest(QJSValue callback) {
    m_hitTest = std::move(callback);
    emit callbacksChanged();
  }
  void setWordRange(QJSValue callback) {
    m_wordRange = std::move(callback);
    emit callbacksChanged();
  }
  void setLinkAt(QJSValue callback) {
    m_linkAt = std::move(callback);
    emit callbacksChanged();
  }

  void setDocument(DocumentController *document);
  void resetDocument();
  void setTarget(QQuickItem *target);
  void setEnabled(bool enabled);
  void setLength(int length);
  void setSelectedText(const QString &text);
  void classBegin() override;
  void componentComplete() override;

  virtual int positionAt(const QPointF &position) const;
  virtual QRectF rectangleAt(int position) const;
  virtual std::pair<int, int> wordAt(int position) const;
  virtual QString linkAtPosition(const QPointF &position) const;
  void select(int start, int end, bool force = false);

protected:
  virtual void applySelection(int start, int end) {}
  virtual void activate() {}
  virtual void deactivate() {}

private:
  void updateRegistration();
  void updateDocument(DocumentController *document);
  void updateScope();
  void detach();

  QPointer<DocumentController> m_document;
  QPointer<DocumentController> m_registeredDocument;
  QPointer<QQuickItem> m_target;
  QPointer<DocumentScope> m_scope;
  QMetaObject::Connection m_scopeConnection;
  QMetaObject::Connection m_rowConnection;
  int m_part = 0;
  QJSValue m_hitTest;
  QJSValue m_positionRectangle;
  QJSValue m_wordRange;
  QJSValue m_linkAt;
  QString m_selectedText;
  int m_length = 1;
  int m_start = 0;
  int m_end = 0;
  bool m_enabled = true;
  bool m_complete = true;
  bool m_explicitDocument = false;
  bool m_explicitTarget = false;
};

class DocumentTextSelection : public DocumentSelection {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit DocumentTextSelection(QObject *parent = nullptr);
  ~DocumentTextSelection() override;

  int length() const override;
  QString selectedText() const override;
  int positionAt(const QPointF &position) const override;
  QRectF rectangleAt(int position) const override;
  std::pair<int, int> wordAt(int position) const override;
  QString linkAtPosition(const QPointF &position) const override;

protected:
  void applySelection(int start, int end) override;
  void activate() override;
  void deactivate() override;

private slots:
  void restoreTextSelection();

private:
  QPointer<QQuickItem> m_activeTarget;
  Qt::MouseButtons m_mouseButtons;
  bool m_focusOnTab = false;
};

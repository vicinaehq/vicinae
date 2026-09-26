#pragma once

#include <QPointer>
#include <QQuickAttachedPropertyPropagator>
#include <QtQml/qqmlregistration.h>
#include "document-controller.hpp"
#include "document-style.hpp"

namespace vicinae::document {

class DocumentScope : public QQuickAttachedPropertyPropagator {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("DocumentScope is an attached property")
  QML_ATTACHED(DocumentScope)
  Q_PROPERTY(vicinae::document::DocumentController *document READ document WRITE setDocument RESET
                 resetDocument NOTIFY documentChanged)

  Q_PROPERTY(
      vicinae::document::DocumentStyle *style READ style WRITE setStyle RESET resetStyle NOTIFY styleChanged)
  Q_PROPERTY(int row READ row WRITE setRow RESET resetRow NOTIFY rowChanged)
  Q_PROPERTY(bool measuring READ measuring NOTIFY measuringChanged)

signals:
  void styleChanged();
  void rowChanged();
  void documentChanged();
  void measuringChanged();

public:
  explicit DocumentScope(QObject *parent = nullptr);
  static DocumentScope *qmlAttachedProperties(QObject *object) { return new DocumentScope(object); }

  DocumentStyle *style() const { return m_style ? m_style.data() : DocumentStyle::defaults(); }
  void setStyle(DocumentStyle *style);
  void resetStyle();

  DocumentController *document() const { return m_document; }
  void setDocument(DocumentController *document);
  void resetDocument();
  int row() const { return m_row; }
  void setRow(int row);
  void resetRow();
  bool measuring() const { return m_measuring; }
  void setMeasuring(bool measuring);

protected:
  void attachedParentChange(QQuickAttachedPropertyPropagator *parent,
                            QQuickAttachedPropertyPropagator *previous) override;

private:
  void inheritStyle(DocumentStyle *style);
  void updateStyle(DocumentStyle *style);
  void inheritDocument(DocumentController *document, bool force = false);
  void updateDocument(DocumentController *document, bool force = false);
  void propagateDocument(bool force = false);
  void inheritRow(int row);
  void updateRow(int row);

  QPointer<DocumentStyle> m_style;
  QMetaObject::Connection m_styleDestroyed;
  bool m_explicitStyle = false;
  QPointer<DocumentController> m_document;
  QMetaObject::Connection m_destroyedConnection;
  bool m_explicitDocument = false;
  bool m_explicitRow = false;
  bool m_measuring = false;
  int m_row = -1;
};

} // namespace vicinae::document

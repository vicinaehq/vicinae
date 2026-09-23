#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <span>

struct DocumentPart {
  QString text;
  bool atomic = false;
};

// Selection content lives in the model, independently of the lifetime of virtualized delegates.
class DocumentModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Use a document model supplied by a view host")

public:
  using QAbstractListModel::QAbstractListModel;
  virtual std::span<const DocumentPart> documentParts(int row) const = 0;
  virtual int partLength(int row, int part) const {
    const auto &value = documentParts(row)[part];
    return value.atomic ? 1 : value.text.size();
  }
  virtual QString textForSelection(int row, int part, int start, int end) const {
    const auto &value = documentParts(row)[part];
    if (start == end) return {};
    return value.atomic ? value.text : value.text.mid(start, end - start);
  }
};

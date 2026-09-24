#pragma once

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <span>
#include <vector>

struct DocumentPart {
  struct InlineObject {
    int position;
    QString text;
  };
  QString text;
  bool atomic = false;
  std::vector<InlineObject> inlineObjects;

  QString selectedText(int start, int end) const {
    if (start == end) return {};
    if (atomic) return text;
    auto result = text.mid(start, end - start);
    for (auto it = inlineObjects.rbegin(); it != inlineObjects.rend(); ++it)
      if (it->position >= start && it->position < end) result.replace(it->position - start, 1, it->text);
    return result;
  }
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
    return value.selectedText(start, end);
  }
};

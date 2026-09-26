#pragma once
#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <algorithm>

namespace vicinae::document {

// Unlike a Repeater's numeric model, resizing preserves existing delegates.
class IndexModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

signals:
  void countChanged();

public:
  using QAbstractListModel::QAbstractListModel;

  int count() const { return m_count; }

  void setCount(int count) {
    count = std::max(0, count);
    if (count == m_count) return;
    if (count > m_count) {
      beginInsertRows({}, m_count, count - 1);
      m_count = count;
      endInsertRows();
    } else {
      beginRemoveRows({}, count, m_count - 1);
      m_count = count;
      endRemoveRows();
    }
    emit countChanged();
  }

  int rowCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : m_count; }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_count || role != Qt::DisplayRole) return {};
    return index.row();
  }

private:
  int m_count = 0;
};

} // namespace vicinae::document

#pragma once

#include <QAbstractListModel>
#include <QFont>
#include <QPointer>
#include <QQmlParserStatus>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>
#include <vector>

namespace vicinae::document {

class DocumentImageCache;

class DocumentTableModel : public QAbstractListModel, public QQmlParserStatus {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QVariantList headers READ headers WRITE setHeaders NOTIFY headersChanged)
  Q_PROPERTY(QVariantList rows READ rows WRITE setRows NOTIFY rowsChanged)
  Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
  Q_PROPERTY(qreal cellWidth READ cellWidth WRITE setCellWidth NOTIFY cellWidthChanged)
  Q_PROPERTY(qreal cellPadding READ cellPadding WRITE setCellPadding NOTIFY cellPaddingChanged)
  Q_PROPERTY(qreal lineHeight READ lineHeight WRITE setLineHeight NOTIFY lineHeightChanged)

signals:
  void headersChanged();
  void rowsChanged();
  void fontChanged();
  void cellWidthChanged();
  void cellPaddingChanged();
  void lineHeightChanged();

public:
  using QAbstractListModel::QAbstractListModel;
  void classBegin() override { m_complete = false; }
  void componentComplete() override;
  enum Role { CellsRole = Qt::UserRole + 1, HeaderRole, HeightRole, SelectionOffsetRole };
  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  QVariantList headers() const { return m_headers; }
  void setHeaders(const QVariantList &headers);
  QVariantList rows() const { return m_rows; }
  void setRows(const QVariantList &rows);
  QFont font() const { return m_font; }
  void setFont(const QFont &font);
  qreal cellWidth() const { return m_cellWidth; }
  void setCellWidth(qreal width);
  qreal cellPadding() const { return m_cellPadding; }
  void setCellPadding(qreal padding);
  qreal lineHeight() const { return m_lineHeight; }
  void setLineHeight(qreal height);

private:
  struct Row {
    QVariantList cells;
    qreal height;
    int selectionOffset;
    bool header;
    bool operator==(const Row &) const = default;
  };
  void rebuild();
  QVariantList m_headers;
  QVariantList m_rows;
  QFont m_font;
  qreal m_cellWidth = 40;
  qreal m_cellPadding = 8;
  qreal m_lineHeight = 1;
  std::vector<Row> m_layout;
  bool m_complete = true;
  bool m_hasImages = false;
  QPointer<DocumentImageCache> m_imageCache;
};

} // namespace vicinae::document

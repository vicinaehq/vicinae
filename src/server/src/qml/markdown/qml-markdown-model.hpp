#pragma once
#include "theme.hpp"
#include <QAbstractListModel>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include <string>
#include <vector>

enum class MdBlockType : int {
  Heading,
  Paragraph,
  CodeBlock,
  BulletList,
  OrderedList,
  Table,
  Image,
  HorizontalRule,
  HtmlBlock,
  Blockquote,
  Callout,
};

class QmlMarkdownModel : public QAbstractListModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(MarkdownModel)

signals:
  void modelReset();

public:
  enum Role {
    BlockTypeRole = Qt::UserRole + 1,
    BlockDataRole,
  };

  explicit QmlMarkdownModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void setMarkdown(const QString &markdown);
  Q_INVOKABLE void clear();
  Q_INVOKABLE void openLink(const QString &url);
  Q_INVOKABLE QString copyCodeBlock(int blockIndex);

private:
  struct Block {
    MdBlockType type;
    QVariantMap data;
  };

  void rebuildInlineStyles();

  std::vector<Block> m_blocks;
  QString m_markdown;

  // Cached theme color hex strings for inline HTML
  QString m_inlineCodeFg;
  QString m_inlineCodeBg;
  QString m_linkColor;
  QString m_textColor;
};

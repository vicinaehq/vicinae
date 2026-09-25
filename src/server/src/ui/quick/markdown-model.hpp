#pragma once
#include <QAbstractListModel>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include <string>
#include <vector>
#include <optional>
#include "markdown-document.hpp"
#include "math-renderer.hpp"
#include "ui/quick/syntax-highlighter.hpp"

class MarkdownModel : public DocumentModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(MarkdownModel)

signals:
  void blocksAppended();
  void loadingChanged();

public:
  enum Role {
    BlockTypeRole = Qt::UserRole + 1,
    BlockDataRole,
  };

  explicit MarkdownModel(QObject *parent = nullptr);

  const QString &markdown() const { return m_markdown; }

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  std::span<const DocumentPart> documentParts(int row) const override;

  Q_INVOKABLE void setMarkdown(const QString &markdown);
  void setMarkdownAsync(QString markdown);
  bool loading() const { return m_loading; }
  Q_INVOKABLE void clear();
  Q_INVOKABLE void openLink(const QString &url);
  Q_INVOKABLE QString copyCodeBlock(int blockIndex);

private:
  struct Block {
    Markdown::BlockType type;
    QVariantMap data;
    mutable std::optional<std::vector<DocumentPart>> parts;
    std::shared_ptr<const math::Resources> resources;
  };

  struct Styles {
    QString inlineCodeFg;
    QString inlineCodeBg;
    QString linkColor;
    QString textColor;
    QString monoFamily;
    syntax::StyleMap syntax;
    bool dark = false;
  };

  void rebuildInlineStyles();
  void setLoading(bool loading);
  void startParse();
  void applyBlocks(const QString &markdown, std::vector<Block> blocks);
  static std::vector<Block> parseBlocks(const QString &markdown, const Styles &styles);

  std::vector<Block> m_blocks;
  QString m_markdown;
  QString m_renderedMarkdown;

  Styles m_styles;
  quint64 m_parseGeneration = 0;
  quint64 m_styleGeneration = 0;
  bool m_parseRunning = false;
  bool m_loading = false;
  bool m_async = false;
};

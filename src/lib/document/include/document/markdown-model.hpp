#pragma once
#include <QPointer>
#include <memory>
#include "document-model.hpp"
#include "document-style.hpp"

namespace vicinae::document {

namespace markdown {
struct Block;
} // namespace markdown

class MarkdownModel : public DocumentModel {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(vicinae::document::DocumentStyle *style READ style WRITE setStyle NOTIFY styleChanged)

  Q_PROPERTY(QString markdown READ markdown WRITE setMarkdown NOTIFY markdownChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
  Q_INVOKABLE void setMarkdown(const QString &markdown);
  Q_INVOKABLE void clear();
  Q_INVOKABLE QString copyCodeBlock(int blockIndex);

signals:
  void markdownChanged();
  void styleChanged();
  void blocksAppended();
  void loadingChanged();

public:
  enum Role {
    BlockTypeRole = Qt::UserRole + 1,
    BlockDataRole,
  };

  explicit MarkdownModel(QObject *parent = nullptr);
  ~MarkdownModel() override;
  DocumentStyle *style() const { return m_style; }
  void setStyle(DocumentStyle *style);

  const QString &markdown() const { return m_markdown; }

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  std::span<const DocumentPart> documentParts(int row) const override;
  TextSnapshot textSnapshot(int row) const override;

  void setMarkdownAsync(QString markdown);
  bool loading() const { return m_loading; }

private:
  void updateMarkdown(QString markdown, bool asynchronous);
  void reparse();
  void setLoading(bool loading);
  void startParse();
  void applyBlocks(const QString &markdown, std::vector<markdown::Block> blocks);

  std::vector<markdown::Block> m_blocks;
  QString m_markdown;
  QString m_renderedMarkdown;

  struct StyleBinding;
  std::unique_ptr<StyleBinding> m_styles;
  QPointer<DocumentStyle> m_style;
  quint64 m_parseGeneration = 0;
  quint64 m_styleGeneration = 0;
  bool m_parseRunning = false;
  bool m_loading = false;
  bool m_async = false;
};

} // namespace vicinae::document

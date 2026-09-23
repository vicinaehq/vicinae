#include <algorithm>
#include "quick-ai-document-model.hpp"

QuickAIDocumentModel::QuickAIDocumentModel(QuickAIConversationModel *conversation, QObject *parent)
    : DocumentModel(parent), m_conversation(conversation) {
  connect(conversation, &QAbstractItemModel::rowsInserted, this,
          [this](const QModelIndex &, int first, int last) {
            for (int row = first; row <= last; ++row)
              addExchange(row);
          });
  connect(conversation, &QAbstractItemModel::dataChanged, this,
          [this](const QModelIndex &first, const QModelIndex &last) {
            for (int row = first.row(); row <= last.row(); ++row)
              updateExchange(row);
          });
  for (int row = 0; row < conversation->rowCount(); ++row)
    addExchange(row);
}

int QuickAIDocumentModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid() || m_exchanges.empty()) return 0;
  const auto &last = m_exchanges.back();
  return last.offset + last.blocks + 2;
}

const QuickAIDocumentModel::Exchange &QuickAIDocumentModel::exchangeAt(int row) const {
  return *std::prev(std::ranges::upper_bound(m_exchanges, row, {}, &Exchange::offset));
}

QVariant QuickAIDocumentModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
  const auto &exchange = exchangeAt(index.row());
  const int local = index.row() - exchange.offset;
  const bool markdown = local > 0 && local <= exchange.blocks;
  switch (role) {
  case KindRole:
    return local == 0 ? "query" : markdown ? "markdown" : "tail";
  case TextRole:
    return local == 0               ? exchange.query.front().text
           : exchange.error.empty() ? QString{}
                                    : exchange.error.front().text;
  case BlockTypeRole:
    return markdown
               ? exchange.markdown->data(exchange.markdown->index(local - 1), MarkdownModel::BlockTypeRole)
               : QVariant::fromValue(Markdown::BlockType::Paragraph);
  case BlockDataRole:
    return markdown
               ? exchange.markdown->data(exchange.markdown->index(local - 1), MarkdownModel::BlockDataRole)
               : QVariant(QVariantMap{});
  case BlockIndexRole:
    return local - 1;
  case MarkdownModelRole:
    return QVariant::fromValue(exchange.markdown.data());
  case PendingRole:
    return exchange.pending && exchange.blocks == 0;
  case FailedRole:
    return !exchange.error.empty();
  case AttachmentsRole:
    return local == 0 ? exchange.attachments : QVariantList{};
  default:
    return {};
  }
}

QHash<int, QByteArray> QuickAIDocumentModel::roleNames() const {
  return {{KindRole, "kind"},
          {TextRole, "text"},
          {BlockTypeRole, "blockType"},
          {BlockDataRole, "blockData"},
          {BlockIndexRole, "blockIndex"},
          {MarkdownModelRole, "markdownModel"},
          {PendingRole, "pending"},
          {FailedRole, "failed"},
          {AttachmentsRole, "attachments"}};
}

std::span<const DocumentPart> QuickAIDocumentModel::documentParts(int row) const {
  const auto &exchange = exchangeAt(row);
  const int local = row - exchange.offset;
  if (local == 0) return exchange.query;
  if (local <= exchange.blocks) return exchange.markdown->documentParts(local - 1);
  return exchange.error;
}

void QuickAIDocumentModel::updateOffsets() {
  int offset = 0;
  for (auto &exchange : m_exchanges) {
    exchange.offset = offset;
    offset += exchange.blocks + 2;
  }
}

void QuickAIDocumentModel::addExchange(int row) {
  const int offset = rowCount();
  beginInsertRows({}, offset, offset + 1);
  if (m_exchanges.size() == m_exchanges.capacity())
    m_exchanges.reserve(std::max<std::size_t>(8, m_exchanges.size() * 2));
  auto *markdown = new MarkdownModel(this);
  m_exchanges.emplace_back(Exchange{
      .markdown = markdown,
      .query = {{m_conversation->data(m_conversation->index(row), QuickAIConversationModel::QueryRole)
                     .toString()}},
      .attachments =
          m_conversation->data(m_conversation->index(row), QuickAIConversationModel::AttachmentsRole)
              .toList(),
      .offset = offset});
  endInsertRows();
  connect(markdown, &QAbstractItemModel::rowsAboutToBeInserted, this,
          [this, row](const QModelIndex &, int first, int last) {
            const int base = m_exchanges[row].offset + 1;
            beginInsertRows({}, base + first, base + last);
          });
  connect(markdown, &QAbstractItemModel::rowsInserted, this, [this, row] {
    m_exchanges[row].blocks = m_exchanges[row].markdown->rowCount();
    updateOffsets();
    endInsertRows();
  });
  connect(markdown, &QAbstractItemModel::rowsAboutToBeRemoved, this,
          [this, row](const QModelIndex &, int first, int last) {
            const int base = m_exchanges[row].offset + 1;
            beginRemoveRows({}, base + first, base + last);
          });
  connect(markdown, &QAbstractItemModel::rowsRemoved, this, [this, row] {
    m_exchanges[row].blocks = m_exchanges[row].markdown->rowCount();
    updateOffsets();
    endRemoveRows();
  });
  connect(markdown, &QAbstractItemModel::dataChanged, this,
          [this, row](const QModelIndex &first, const QModelIndex &last) {
            const int base = m_exchanges[row].offset + 1;
            emit dataChanged(index(base + first.row()), index(base + last.row()), {BlockDataRole});
          });
  connect(markdown, &QAbstractItemModel::modelAboutToBeReset, this, [this, row] {
    const auto &exchange = m_exchanges[row];
    if (exchange.blocks > 0) beginRemoveRows({}, exchange.offset + 1, exchange.offset + exchange.blocks);
  });
  connect(markdown, &QAbstractItemModel::modelReset, this, [this, row] {
    auto &exchange = m_exchanges[row];
    if (exchange.blocks > 0) {
      exchange.blocks = 0;
      updateOffsets();
      endRemoveRows();
    }
    const int count = exchange.markdown->rowCount();
    if (count > 0) {
      beginInsertRows({}, exchange.offset + 1, exchange.offset + count);
      exchange.blocks = count;
      updateOffsets();
      endInsertRows();
    }
  });
  updateExchange(row);
}

void QuickAIDocumentModel::updateExchange(int row) {
  auto &exchange = m_exchanges[row];
  const auto source = m_conversation->index(row);
  exchange.pending = m_conversation->data(source, QuickAIConversationModel::PendingRole).toBool();
  const auto error = m_conversation->data(source, QuickAIConversationModel::ErrorRole).toString();
  exchange.error.clear();
  if (!error.isEmpty()) {
    exchange.error.reserve(1);
    exchange.error.emplace_back(DocumentPart{error});
  }
  const auto response = m_conversation->data(source, QuickAIConversationModel::ResponseRole).toString();
  if (exchange.markdown->markdown() != response) exchange.markdown->setMarkdown(response);
  emit dataChanged(index(exchange.offset), index(exchange.offset + exchange.blocks + 1),
                   {PendingRole, FailedRole, TextRole});
}

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
          [this](const QModelIndex &first, const QModelIndex &last, const QList<int> &roles) {
            if (roles == QList<int>{QuickAIConversationModel::ResponseRole}) return;
            for (int row = first.row(); row <= last.row(); ++row)
              updateExchange(row);
          });
  connect(conversation, &QuickAIConversationModel::contentAdded, this, &QuickAIDocumentModel::addContent);
  connect(conversation, &QuickAIConversationModel::contentChanged, this,
          &QuickAIDocumentModel::updateContent);
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

const QuickAIDocumentModel::Content *QuickAIDocumentModel::contentAt(const Exchange &exchange,
                                                                     int row) const {
  if (row == exchange.offset || row == exchange.offset + exchange.blocks + 1) return nullptr;
  return &*std::prev(std::ranges::upper_bound(exchange.contents, row, {}, &Content::offset));
}

QVariant QuickAIDocumentModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
  const auto &exchange = exchangeAt(index.row());
  const auto *content = contentAt(exchange, index.row());
  const int local = content ? index.row() - content->offset : 0;
  const bool query = index.row() == exchange.offset;
  const bool markdown = content && content->markdown;
  switch (role) {
  case KindRole:
    return query                                                                    ? "query"
           : !content                                                               ? "tail"
           : markdown                                                               ? "markdown"
           : local == 0 && content->tool.value(QStringLiteral("count")).toInt() > 1 ? "toolGroup"
                                                                                    : "tool";
  case TextRole:
    if (query) return exchange.query.front().text;
    if (content) return QString{};
    return exchange.error.empty() ? QString{} : exchange.error.front().text;
  case BlockTypeRole:
    return markdown ? content->markdown->data(content->markdown->index(local), MarkdownModel::BlockTypeRole)
                    : QVariant::fromValue(Markdown::BlockType::Paragraph);
  case BlockDataRole:
    return markdown ? content->markdown->data(content->markdown->index(local), MarkdownModel::BlockDataRole)
                    : QVariant(QVariantMap{});
  case BlockIndexRole:
    return local;
  case MarkdownModelRole:
    return QVariant::fromValue(markdown ? content->markdown.data() : nullptr);
  case PendingRole:
    return exchange.pending &&
           (exchange.blocks == 0 ||
            (!exchange.contents.back().markdown &&
             exchange.contents.back().tool.value(QStringLiteral("status")) != QStringLiteral("running")));
  case FailedRole:
    return !exchange.error.empty();
  case AttachmentsRole:
    return query ? exchange.attachments : QVariantList{};
  case ToolRole: {
    if (!content || markdown) return QVariantMap{};
    const auto &group = std::get<QuickAIConversationModel::ToolGroup>(
        m_conversation->contents(exchange.sourceRow)[content->sourcePart]);
    const bool grouped = group.calls.size() > 1;
    if (grouped && local == 0) return content->tool;
    const auto &tool = group.calls[grouped ? local - 1 : 0];
    return QVariantMap{
        {QStringLiteral("id"), tool.id},
        {QStringLiteral("name"), tool.name},
        {QStringLiteral("iconSource"), tool.iconSource},
        {QStringLiteral("summary"), tool.summary.value_or(QString{})},
        {QStringLiteral("status"), tool.status},
        {QStringLiteral("statusText"), tool.statusText ? QVariant(*tool.statusText) : QVariant{}},
        {QStringLiteral("expanded"), tool.expanded},
        {QStringLiteral("grouped"), grouped},
        {QStringLiteral("durationMs"), tool.durationMs ? QVariant::fromValue(*tool.durationMs) : QVariant{}},
        {QStringLiteral("arguments"), tool.arguments},
        {QStringLiteral("hasOutput"), tool.output.has_value()},
        {QStringLiteral("output"), tool.output.value_or(QString{})}};
  }
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
          {AttachmentsRole, "attachments"},
          {ToolRole, "tool"}};
}

std::span<const DocumentPart> QuickAIDocumentModel::documentParts(int row) const {
  const auto &exchange = exchangeAt(row);
  if (row == exchange.offset) return exchange.query;
  const auto *content = contentAt(exchange, row);
  if (!content) return exchange.error;
  const auto local = row - content->offset;
  if (content->markdown) return content->markdown->documentParts(local);
  return {};
}

void QuickAIDocumentModel::updateOffsets() {
  int offset = 0;
  for (auto &exchange : m_exchanges) {
    exchange.offset = offset++;
    for (auto &content : exchange.contents) {
      content.offset = offset;
      offset += content.rows;
    }
    exchange.blocks = offset - exchange.offset - 1;
    ++offset;
  }
}

void QuickAIDocumentModel::addExchange(int row) {
  const int offset = rowCount();
  beginInsertRows({}, offset, offset + 1);
  if (m_exchanges.size() == m_exchanges.capacity())
    m_exchanges.reserve(std::max<std::size_t>(8, m_exchanges.size() * 2));
  m_exchanges.emplace_back(Exchange{
      .query = {{m_conversation->data(m_conversation->index(row), QuickAIConversationModel::QueryRole)
                     .toString()}},
      .attachments =
          m_conversation->data(m_conversation->index(row), QuickAIConversationModel::AttachmentsRole)
              .toList(),
      .sourceRow = row,
      .offset = offset});
  endInsertRows();
  const auto count = static_cast<int>(m_conversation->contents(row).size());
  for (int part = 0; part < count; ++part)
    addContent(row, part);
  updateExchange(row);
}

void QuickAIDocumentModel::addContent(int row, int part) {
  auto &exchange = m_exchanges[row];
  const bool response =
      std::holds_alternative<QuickAIConversationModel::Response>(m_conversation->contents(row)[part]);
  const int offset = exchange.offset + exchange.blocks + 1;
  if (!response) beginInsertRows({}, offset, offset);
  exchange.contents.reserve(exchange.contents.size() + 1);
  auto *markdown = response ? new MarkdownModel(this) : nullptr;
  exchange.contents.emplace_back(
      Content{.markdown = markdown, .sourcePart = part, .offset = offset, .rows = response ? 0 : 1});
  updateOffsets();
  if (!response) endInsertRows();
  if (markdown) {
    connect(markdown, &QAbstractItemModel::rowsAboutToBeInserted, this,
            [this, row, part](const QModelIndex &, int first, int last) {
              const auto base = m_exchanges[row].contents[part].offset;
              beginInsertRows({}, base + first, base + last);
            });
    connect(markdown, &QAbstractItemModel::rowsInserted, this, [this, row, part] {
      auto &content = m_exchanges[row].contents[part];
      content.rows = content.markdown->rowCount();
      updateOffsets();
      endInsertRows();
    });
    connect(markdown, &QAbstractItemModel::rowsAboutToBeRemoved, this,
            [this, row, part](const QModelIndex &, int first, int last) {
              const auto base = m_exchanges[row].contents[part].offset;
              beginRemoveRows({}, base + first, base + last);
            });
    connect(markdown, &QAbstractItemModel::rowsRemoved, this, [this, row, part] {
      auto &content = m_exchanges[row].contents[part];
      content.rows = content.markdown->rowCount();
      updateOffsets();
      endRemoveRows();
    });
    connect(markdown, &QAbstractItemModel::dataChanged, this,
            [this, row, part](const QModelIndex &first, const QModelIndex &last) {
              const auto base = m_exchanges[row].contents[part].offset;
              emit dataChanged(index(base + first.row()), index(base + last.row()),
                               {BlockTypeRole, BlockDataRole});
            });
    connect(markdown, &QAbstractItemModel::modelAboutToBeReset, this, [this, row, part] {
      const auto &content = m_exchanges[row].contents[part];
      if (content.rows > 0) beginRemoveRows({}, content.offset, content.offset + content.rows - 1);
    });
    connect(markdown, &QAbstractItemModel::modelReset, this, [this, row, part] {
      auto &content = m_exchanges[row].contents[part];
      if (content.rows > 0) {
        content.rows = 0;
        updateOffsets();
        endRemoveRows();
      }
      const int count = content.markdown->rowCount();
      if (count > 0) {
        beginInsertRows({}, content.offset, content.offset + count - 1);
        content.rows = count;
        updateOffsets();
        endInsertRows();
      }
    });
  }
  updateContent(row, part);
}

void QuickAIDocumentModel::updateContent(int row, int part) {
  auto &content = m_exchanges[row].contents[part];
  const auto &source = m_conversation->contents(row)[part];
  if (const auto *response = std::get_if<QuickAIConversationModel::Response>(&source)) {
    content.markdown->setMarkdown(QString::fromUtf8(response->text.data(), response->visibleBytes));
  } else {
    const auto &group = std::get<QuickAIConversationModel::ToolGroup>(source);
    const auto count = static_cast<int>(group.calls.size());
    const int rows = count > 1 && group.expanded ? count + 1 : 1;
    if (count > 1 && group.expanded && content.tool.value(QStringLiteral("count")).toInt() == 1) {
      // Keep the open call's delegate and selection when inserting its new group header.
      beginInsertRows({}, content.offset, content.offset);
      ++content.rows;
      updateOffsets();
      endInsertRows();
    }
    if (rows < content.rows) {
      beginRemoveRows({}, content.offset + rows, content.offset + content.rows - 1);
      content.rows = rows;
      updateOffsets();
      endRemoveRows();
    } else if (rows > content.rows) {
      beginInsertRows({}, content.offset + content.rows, content.offset + rows - 1);
      content.rows = rows;
      updateOffsets();
      endInsertRows();
    }
    bool cancelled = false;
    bool queued = false;
    const QuickAIConversationModel::Tool *running = nullptr;
    std::optional<qint64> duration;
    for (const auto &tool : group.calls) {
      if (tool.durationMs) duration = duration.value_or(0) + *tool.durationMs;
      if (tool.status == QLatin1String("running"))
        running = &tool;
      else if (tool.status == QLatin1String("queued"))
        queued = true;
      else if (tool.status == QLatin1String("cancelled"))
        cancelled = true;
    }
    const auto &current = running ? *running : group.calls.back();
    const QString status = running     ? QStringLiteral("running")
                           : queued    ? QStringLiteral("queued")
                           : cancelled ? QStringLiteral("cancelled")
                                       : QStringLiteral("completed");
    content.tool = {{QStringLiteral("id"), group.calls.front().id},
                    {QStringLiteral("count"), count},
                    {QStringLiteral("expanded"), group.expanded},
                    {QStringLiteral("status"), status},
                    {QStringLiteral("name"), current.name},
                    {QStringLiteral("iconSource"), current.iconSource},
                    {QStringLiteral("summary"), current.summary.value_or(QString{})},
                    {QStringLiteral("durationMs"),
                     duration && !running && !queued ? QVariant::fromValue(*duration) : QVariant{}}};
    emit dataChanged(index(content.offset), index(content.offset + content.rows - 1), {KindRole, ToolRole});
  }
  const auto &exchange = m_exchanges[row];
  const auto tail = index(exchange.offset + exchange.blocks + 1);
  emit dataChanged(tail, tail, {PendingRole});
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
  emit dataChanged(index(exchange.offset), index(exchange.offset + exchange.blocks + 1),
                   {PendingRole, FailedRole, TextRole});
}

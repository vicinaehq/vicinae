#include <QBuffer>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QPromise>
#include <QSaveFile>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <unordered_set>
#include "common/enumerate.hpp"
#include "conversation-store.hpp"
#include "db/database.hpp"
#include "utils/migration-manager/migration-manager.hpp"

namespace AI {
namespace {

std::optional<ModelRef> readModel(const db::Statement &statement, int column) {
  if (statement.isNull(column)) return {};
  auto model = ModelRef::fromString(statement.columnText(column));
  return model ? std::optional(std::move(*model)) : std::nullopt;
}

std::optional<std::string> modelName(const std::optional<ModelRef> &model) {
  return model ? std::optional(model->toString()) : std::nullopt;
}

std::optional<std::string> readOptional(const db::Statement &statement, int column) {
  return statement.isNull(column) ? std::nullopt : std::optional(statement.columnText(column));
}

std::string_view toolStatus(Agent::ToolState state) {
  switch (state) {
  case Agent::ToolState::Queued:
    return "queued";
  case Agent::ToolState::Running:
    return "running";
  case Agent::ToolState::Succeeded:
    return "succeeded";
  case Agent::ToolState::Failed:
    return "failed";
  case Agent::ToolState::Cancelled:
    return "cancelled";
  }
  std::unreachable();
}

Agent::ToolState readToolStatus(std::string_view state) {
  if (state == "queued") return Agent::ToolState::Queued;
  if (state == "running") return Agent::ToolState::Running;
  if (state == "succeeded") return Agent::ToolState::Succeeded;
  if (state == "failed") return Agent::ToolState::Failed;
  return Agent::ToolState::Cancelled;
}

ChatRole readRole(std::string_view role) {
  if (role == "user") return ChatRole::User;
  if (role == "assistant") return ChatRole::Assistant;
  if (role == "tool") return ChatRole::Tool;
  throw std::runtime_error("Invalid conversation message role");
}

void writeFile(const std::filesystem::path &path, const QByteArray &bytes) {
  if (std::filesystem::exists(path)) return;
  std::filesystem::create_directories(path.parent_path());

  QSaveFile file(QString::fromStdString(path.string()));
  if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
    throw std::runtime_error(file.errorString().toStdString());
}

QByteArray readFile(const std::filesystem::path &path) {
  QFile file(QString::fromStdString(path.string()));
  if (!file.open(QIODevice::ReadOnly)) throw std::runtime_error(file.errorString().toStdString());
  auto data = file.readAll();
  if (file.error() != QFileDevice::NoError) throw std::runtime_error(file.errorString().toStdString());
  return data;
}

template <typename F> auto submit(QObject *worker, F operation) {
  using T = std::invoke_result_t<F>;
  auto promise = std::make_shared<QPromise<Result<T>>>();
  promise->start();
  auto future = promise->future();

  QMetaObject::invokeMethod(
      worker,
      [promise, operation = std::move(operation)]() mutable {
        try {
          if constexpr (std::is_void_v<T>) {
            operation();
            promise->addResult(Result<void>{});
          } else {
            promise->addResult(Result<T>{operation()});
          }
        } catch (const std::exception &error) {
          promise->addResult(Result<T>{std::unexpected(std::string(error.what()))});
        }
        promise->finish();
      },
      Qt::QueuedConnection);

  return future;
}

} // namespace

class ConversationStore::Worker : public QObject {
public:
  explicit Worker(std::filesystem::path directory) : m_directory(std::move(directory)) {}

  void open() {
    if (m_ready) return;
    std::filesystem::create_directories(m_directory / "attachments");
    auto database = db::Database::open(m_directory / "conversations.db");
    if (!database) throw std::runtime_error(database.error());
    m_db = std::move(*database);

    if (!m_db.exec("PRAGMA journal_mode=WAL; PRAGMA foreign_keys=ON;"))
      throw std::runtime_error(m_db.lastError());
    MigrationManager migrations(m_db, "conversations");
    migrations.runMigrations();

    auto interrupted =
        prepare("UPDATE turn SET status='interrupted', finished_at=:now WHERE status='running'");
    interrupted.bind(":now", std::int64_t(QDateTime::currentMSecsSinceEpoch()));
    execute(interrupted);
    if (!m_db.exec("UPDATE tool_call SET status='interrupted' WHERE status IN ('running', 'queued')"))
      throw std::runtime_error(m_db.lastError());

    removeOrphanedFiles();
    m_ready = true;
  }

  void close() { m_db = {}; }

  std::vector<ConversationInfo> list() {
    open();
    auto query =
        prepare("SELECT id,title,model,created_at,updated_at FROM conversation ORDER BY updated_at DESC,id");
    std::vector<ConversationInfo> conversations;
    conversations.reserve(64);

    while (next(query))
      conversations.emplace_back(ConversationInfo{query.columnText(0), query.columnText(1),
                                                  readModel(query, 2), query.columnInt64(3),
                                                  query.columnInt64(4)});
    return conversations;
  }

  void save(const ConversationInfo &info, const ConversationTurn &turn) {
    open();
    saveFiles(info.id, turn.attachments);
    auto tx = m_db.transaction();

    auto conversation = prepare(R"(
      INSERT INTO conversation(id,title,model,created_at,updated_at) VALUES(:id,:title,:model,:created,:updated)
      ON CONFLICT(id) DO UPDATE SET model=excluded.model, updated_at=excluded.updated_at
    )");
    conversation.bind(":id", info.id);
    conversation.bind(":title", info.title);
    conversation.bind(":model", modelName(info.model));
    conversation.bind(":created", info.createdAt);
    conversation.bind(":updated", info.updatedAt);
    execute(conversation);

    auto record = prepare(R"(
      INSERT INTO turn(id,conversation_id,position,query,status,model,model_label,thinking,max_steps,started_at,finished_at,error)
      VALUES(:id,:conversation,:position,:query,:status,:model,:label,:thinking,:steps,:started,:finished,:error)
      ON CONFLICT(id) DO UPDATE SET status=excluded.status,model_label=excluded.model_label,
        finished_at=excluded.finished_at,error=excluded.error
    )");
    record.bind(":id", turn.id);
    record.bind(":conversation", info.id);
    record.bind(":position", turn.position);
    record.bind(":query", turn.query);
    record.bind(":status", turn.status);
    record.bind(":model", modelName(turn.options.model));
    record.bind(":label", turn.modelLabel);
    record.bind(":thinking", static_cast<int>(turn.options.thinking));
    record.bind(":steps", turn.options.maxSteps);
    record.bind(":started", turn.startedAt);
    record.bind(":finished", turn.finishedAt);
    record.bind(":error", turn.error);
    execute(record);

    auto enabled = prepare("INSERT OR IGNORE INTO turn_tool(turn_id,name) VALUES(:turn,:name)");
    for (const auto &name : turn.options.tools) {
      enabled.bind(":turn", turn.id);
      enabled.bind(":name", name);
      execute(enabled);
    }

    saveAttachments(info.id, turn);
    auto remove = prepare("DELETE FROM message WHERE turn_id=:turn");
    remove.bind(":turn", turn.id);
    execute(remove);

    std::map<std::string, std::int64_t> calls;
    std::size_t imageIndex = 0;
    for (const auto &[position, message] : vicinae::enumerate(turn.messages)) {
      auto entry = prepare("INSERT INTO message(turn_id,position,role) VALUES(:turn,:position,:role)");
      entry.bind(":turn", turn.id);
      entry.bind(":position", std::int64_t(position));
      entry.bind(":role", roleName(message.content.role));
      execute(entry);
      const auto messageId = m_db.lastInsertRowId();

      for (const auto &[index, part] : vicinae::enumerate(message.content.parts)) {
        const auto kind = std::holds_alternative<TextPart>(part)       ? "text"
                          : std::holds_alternative<ImagePart>(part)    ? "image"
                          : std::holds_alternative<ToolCallPart>(part) ? "tool_call"
                                                                       : "tool_result";
        auto item =
            prepare("INSERT INTO message_part(message_id,position,kind) VALUES(:message,:position,:kind)");
        item.bind(":message", messageId);
        item.bind(":position", std::int64_t(index));
        item.bind(":kind", kind);
        execute(item);
        const auto partId = m_db.lastInsertRowId();

        if (const auto *text = std::get_if<TextPart>(&part)) {
          auto value = prepare("INSERT INTO text_part(part_id,text) VALUES(:id,:text)");
          value.bind(":id", partId);
          value.bind(":text", text->text);
          execute(value);
        } else if (std::holds_alternative<ImagePart>(part)) {
          while (imageIndex < turn.attachments.size() && !turn.attachments[imageIndex].data.isImage())
            ++imageIndex;
          if (imageIndex == turn.attachments.size()) throw std::runtime_error("Missing image attachment");
          auto value = prepare("INSERT INTO image_part(part_id,attachment_id) VALUES(:id,:attachment)");
          value.bind(":id", partId);
          value.bind(":attachment", turn.attachments[imageIndex++].id);
          execute(value);
        } else if (const auto *call = std::get_if<ToolCallPart>(&part)) {
          const auto source =
              std::ranges::find(turn.calls, call->id, [](const auto &tool) { return tool.call.id; });
          if (source == turn.calls.end()) throw std::runtime_error("Missing tool invocation");
          auto value = prepare(R"(
            INSERT INTO tool_call(part_id,provider_id,name,arguments,summary,status,duration_ms)
            VALUES(:id,:provider,:name,:arguments,:summary,:status,:duration)
          )");
          value.bind(":id", partId);
          value.bind(":provider", call->id);
          value.bind(":name", call->name);
          value.bind(":arguments", call->arguments);
          value.bind(":summary", source->summary);
          value.bind(":status", toolStatus(source->state));
          value.bind(":duration", source->duration ? std::optional<std::int64_t>(source->duration->count())
                                                   : std::nullopt);
          execute(value);
          calls.emplace(call->id, partId);
        } else if (const auto *result = std::get_if<ToolResultPart>(&part)) {
          const auto source =
              std::ranges::find(turn.calls, result->callId, [](const auto &tool) { return tool.call.id; });
          if (source == turn.calls.end() || !source->output) throw std::runtime_error("Missing tool result");
          auto value = prepare(R"(
            INSERT INTO tool_result(part_id,call_part_id,content,display_text,status_text,failed)
            VALUES(:id,:call,:content,:display,:status,:failed)
          )");
          value.bind(":id", partId);
          value.bind(":call", calls.at(result->callId));
          value.bind(":content", result->content);
          value.bind(":display", source->output->displayText);
          value.bind(":status", source->output->statusText);
          value.bind(":failed", int(source->output->failed));
          execute(value);
        }
      }
    }

    if (!tx.commit()) {
      const auto error = m_db.lastError();
      tx.rollback();
      throw std::runtime_error(error);
    }
  }

  Conversation load(const std::string &id) {
    open();
    Conversation result;
    auto info = prepare("SELECT id,title,model,created_at,updated_at FROM conversation WHERE id=:id");
    info.bind(":id", id);
    if (!next(info)) throw std::runtime_error("Conversation no longer exists");
    result.info = {info.columnText(0), info.columnText(1), readModel(info, 2), info.columnInt64(3),
                   info.columnInt64(4)};
    result.turns.reserve(32);

    auto turns = prepare(R"(
      SELECT id,position,query,status,model,model_label,thinking,max_steps,started_at,finished_at,error
      FROM turn WHERE conversation_id=:conversation ORDER BY position
    )");
    turns.bind(":conversation", id);
    Agent::Id nextId = 1;
    while (next(turns)) {
      ConversationTurn turn;
      turn.id = turns.columnText(0);
      turn.position = turns.columnInt(1);
      turn.query = turns.columnText(2);
      turn.status = turns.columnText(3);
      turn.options.model = readModel(turns, 4);
      turn.modelLabel = turns.columnText(5);
      turn.options.thinking = static_cast<ThinkingMode>(turns.columnInt(6));
      turn.options.maxSteps = turns.columnInt(7);
      turn.startedAt = turns.columnInt64(8);
      if (!turns.isNull(9)) turn.finishedAt = turns.columnInt64(9);
      turn.error = readOptional(turns, 10);
      turn.messages.reserve(16);
      turn.calls.reserve(8);

      loadAttachments(turn);
      auto enabled = prepare("SELECT name FROM turn_tool WHERE turn_id=:turn");
      enabled.bind(":turn", turn.id);
      turn.options.tools.reserve(8);
      while (next(enabled))
        turn.options.tools.emplace_back(enabled.columnText(0));

      loadMessages(turn, nextId);
      for (auto &call : turn.calls) {
        if (call.output) continue;
        call.state = Agent::ToolState::Cancelled;
        call.output = ToolOutput{
            .content = QCoreApplication::translate("ConversationStore",
                                                   "Tool execution was interrupted; no result was recorded.")
                           .toStdString(),
            .failed = true};
        ChatMessage message{.role = ChatRole::Tool};
        message.parts.emplace_back(ToolResultPart{call.call.id, call.output->content});
        turn.messages.reserve(turn.messages.size() + 1);
        turn.messages.emplace_back(Agent::Message{nextId++, std::move(message)});
      }
      result.turns.emplace_back(std::move(turn));
    }
    return result;
  }

  void remove(const std::string &id) {
    open();
    auto remove = prepare("DELETE FROM conversation WHERE id=:id");
    remove.bind(":id", id);
    execute(remove);
    std::filesystem::remove_all(m_directory / "attachments" / id);
  }

  void setModel(const std::string &id, const std::optional<ModelRef> &model) {
    open();
    auto update = prepare("UPDATE conversation SET model=:model WHERE id=:id");
    update.bind(":id", id);
    update.bind(":model", modelName(model));
    execute(update);
  }

  bool setTitle(const std::string &id, const std::string &title) {
    open();
    auto update = prepare("UPDATE conversation SET title=:title WHERE id=:id");
    update.bind(":id", id);
    update.bind(":title", title);
    execute(update);
    return m_db.changes() > 0;
  }

private:
  db::Statement prepare(std::string_view sql) {
    auto statement = m_db.prepare(sql);
    if (!statement) throw std::runtime_error(m_db.lastError());
    return statement;
  }

  bool next(db::Statement &statement) {
    if (statement.step()) return true;
    const auto code = sqlite3_errcode(m_db.handle());
    if (code != SQLITE_OK && code != SQLITE_DONE) throw std::runtime_error(m_db.lastError());
    return false;
  }

  void execute(db::Statement &statement) {
    if (!statement.exec()) throw std::runtime_error(statement.lastError());
  }

  std::filesystem::path originalPath(std::string_view conversation,
                                     const ConversationAttachment &attachment) const {
    return std::filesystem::path("attachments") / conversation / attachment.id / "original" /
           QFileInfo(QString::fromStdString(attachment.data.name)).fileName().toStdString();
  }

  void removeOrphanedFiles() {
    auto query = prepare("SELECT id FROM attachment");
    std::unordered_set<std::string> attachments;
    attachments.reserve(64);
    while (next(query))
      attachments.emplace(query.columnText(0));

    for (const auto &conversation : std::filesystem::directory_iterator(m_directory / "attachments")) {
      if (!conversation.is_directory()) continue;
      for (const auto &attachment : std::filesystem::directory_iterator(conversation.path())) {
        if (!attachments.contains(attachment.path().filename().string()))
          std::filesystem::remove_all(attachment.path());
      }
      if (std::filesystem::is_empty(conversation.path())) std::filesystem::remove(conversation.path());
    }
  }

  void saveFiles(std::string_view conversation, const std::vector<ConversationAttachment> &attachments) {
    for (const auto &attachment : attachments) {
      writeFile(m_directory / originalPath(conversation, attachment), attachment.data.originalBytes);
      if (const auto *image = std::get_if<FileAttachment::Image>(&attachment.data.contents)) {
        const auto path = m_directory / "attachments" / conversation / attachment.id / "image.png";
        if (!std::filesystem::exists(path)) writeFile(path, QByteArray::fromBase64(image->base64));
      }
    }
  }

  void saveAttachments(std::string_view conversation, const ConversationTurn &turn) {
    auto insert = prepare(R"(
      INSERT OR IGNORE INTO attachment(id,turn_id,position,name,mime_type,original_path,image_path,image_mime_type,text)
      VALUES(:id,:turn,:position,:name,:mime,:original,:image,:image_mime,:text)
    )");
    for (const auto &[position, attachment] : vicinae::enumerate(turn.attachments)) {
      const auto *image = std::get_if<FileAttachment::Image>(&attachment.data.contents);
      insert.bind(":id", attachment.id);
      insert.bind(":turn", turn.id);
      insert.bind(":position", std::int64_t(position));
      insert.bind(":name", attachment.data.name);
      insert.bind(":mime", attachment.data.originalMimeType);
      insert.bind(":original", originalPath(conversation, attachment).string());
      insert.bind(":image", image ? std::optional((std::filesystem::path("attachments") / conversation /
                                                   attachment.id / "image.png")
                                                      .string())
                                  : std::nullopt);
      insert.bind(":image_mime", image ? std::optional(image->mimeType) : std::nullopt);
      insert.bind(":text",
                  image ? std::nullopt : std::optional(std::get<std::string>(attachment.data.contents)));
      execute(insert);
    }
  }

  void loadAttachments(ConversationTurn &turn) {
    auto query = prepare("SELECT id,name,mime_type,original_path,image_path,image_mime_type,text FROM "
                         "attachment WHERE turn_id=:turn ORDER BY position");
    query.bind(":turn", turn.id);
    turn.attachments.reserve(8);

    while (next(query)) {
      ConversationAttachment attachment;
      attachment.id = query.columnText(0);
      attachment.data.name = query.columnText(1);
      attachment.data.originalMimeType = query.columnText(2);
      attachment.path = (m_directory / query.columnText(3)).string();
      if (!query.isNull(4)) {
        const auto bytes = readFile(m_directory / query.columnText(4));
        attachment.data.contents =
            FileAttachment::Image{query.columnText(5), bytes.toBase64(), std::size_t(bytes.size())};
        auto image = QImage::fromData(bytes).scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QByteArray thumbnail;
        QBuffer buffer(&thumbnail);
        image.save(&buffer, "PNG");
        attachment.data.preview = ImageUrl(ImageURL::rawData(thumbnail, QStringLiteral("image/png")));
      } else {
        attachment.data.contents = query.columnText(6);
      }
      turn.attachments.emplace_back(std::move(attachment));
    }
  }

  void loadMessages(ConversationTurn &turn, Agent::Id &nextId) {
    auto query = prepare("SELECT id,role FROM message WHERE turn_id=:turn ORDER BY position");
    query.bind(":turn", turn.id);

    while (next(query)) {
      Agent::Message message{nextId++, ChatMessage{.role = readRole(query.columnText(1))}};
      message.content.parts.reserve(4);
      auto parts = prepare(R"(
        SELECT p.kind,t.text,i.attachment_id,
          c.provider_id,c.name,c.arguments,c.summary,c.status,c.duration_ms,
          r.content,r.display_text,r.status_text,r.failed,rc.provider_id
        FROM message_part p
        LEFT JOIN text_part t ON t.part_id=p.id
        LEFT JOIN image_part i ON i.part_id=p.id
        LEFT JOIN tool_call c ON c.part_id=p.id
        LEFT JOIN tool_result r ON r.part_id=p.id
        LEFT JOIN tool_call rc ON rc.part_id=r.call_part_id
        WHERE p.message_id=:message ORDER BY p.position
      )");
      parts.bind(":message", query.columnInt64(0));
      while (next(parts)) {
        const auto kind = parts.columnText(0);
        if (kind == "text") {
          message.content.parts.emplace_back(TextPart{parts.columnText(1)});
        } else if (kind == "image") {
          const auto attachment =
              std::ranges::find(turn.attachments, parts.columnText(2), &ConversationAttachment::id);
          if (attachment == turn.attachments.end()) throw std::runtime_error("Missing image attachment");
          const auto &image = std::get<FileAttachment::Image>(attachment->data.contents);
          message.content.parts.emplace_back(ImagePart{image.mimeType, image.base64});
        } else if (kind == "tool_call") {
          Agent::ToolCall call{.id = nextId++,
                               .messageId = message.id,
                               .call = {parts.columnText(3), parts.columnText(4), parts.columnText(5)},
                               .summary = readOptional(parts, 6),
                               .state = readToolStatus(parts.columnText(7))};
          if (!parts.isNull(8)) call.duration = std::chrono::milliseconds(parts.columnInt64(8));
          message.content.parts.emplace_back(call.call);
          turn.calls.emplace_back(std::move(call));
        } else if (kind == "tool_result") {
          const auto callId = parts.columnText(13);
          const auto call =
              std::ranges::find(turn.calls, callId, [](const auto &tool) { return tool.call.id; });
          if (call == turn.calls.end()) throw std::runtime_error("Missing tool invocation");
          call->output = ToolOutput{parts.columnText(9), readOptional(parts, 10), readOptional(parts, 11),
                                    parts.columnInt(12) != 0};
          message.content.parts.emplace_back(ToolResultPart{callId, call->output->content});
        }
      }
      turn.messages.emplace_back(std::move(message));
    }
  }

  std::filesystem::path m_directory;
  db::Database m_db;
  bool m_ready = false;
};

ConversationStore::ConversationStore(std::filesystem::path directory)
    : m_directory(std::move(directory)), m_worker(new Worker(m_directory)) {
  m_worker->moveToThread(&m_thread);
  connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
  m_thread.setObjectName(QStringLiteral("ConversationStore"));
  m_thread.start();
}

ConversationStore::~ConversationStore() {
  QMetaObject::invokeMethod(
      m_worker,
      [worker = m_worker] {
        worker->close();
        QThread::currentThread()->quit();
      },
      Qt::QueuedConnection);
  m_thread.wait();
}

QFuture<Result<std::vector<ConversationInfo>>> ConversationStore::list() {
  return submit(m_worker, [worker = m_worker] { return worker->list(); });
}

QFuture<Result<Conversation>> ConversationStore::load(std::string id) {
  return submit(m_worker, [worker = m_worker, id = std::move(id)] { return worker->load(id); });
}

void ConversationStore::save(ConversationInfo info, ConversationTurn turn) {
  submit(m_worker, [worker = m_worker, info = std::move(info), turn = std::move(turn)] {
    worker->save(info, turn);
  }).then(this, [this](const Result<void> &result) {
    if (result)
      emit changed();
    else
      emit errorOccurred(QString::fromStdString(result.error()));
  });
}

void ConversationStore::setModel(std::string id, std::optional<ModelRef> model) {
  submit(m_worker, [worker = m_worker, id = std::move(id), model = std::move(model)] {
    worker->setModel(id, model);
  }).then(this, [this](const Result<void> &result) {
    if (!result) emit errorOccurred(QString::fromStdString(result.error()));
  });
}

void ConversationStore::setTitle(std::string id, std::string title) {
  submit(m_worker, [worker = m_worker, id, title] {
    return worker->setTitle(id, title);
  }).then(this, [this, id = std::move(id), title = std::move(title)](const Result<bool> &result) {
    if (!result) {
      qWarning() << "Conversation title:" << result.error();
    } else if (*result) {
      emit titleChanged(QString::fromStdString(id), QString::fromStdString(title));
      emit changed();
    }
  });
}

QFuture<Result<void>> ConversationStore::remove(std::string id) {
  emit aboutToRemove(QString::fromStdString(id));
  return submit(m_worker, [worker = m_worker, id] { worker->remove(id); })
      .then(this, [this, id = std::move(id)](Result<void> result) {
        if (result) {
          emit removed(QString::fromStdString(id));
          emit changed();
        }
        return result;
      });
}

std::string ConversationStore::attachmentPath(std::string_view conversation, std::string_view attachment,
                                              std::string_view name) const {
  return (m_directory / "attachments" / conversation / attachment / "original" /
          QFileInfo(QString::fromStdString(std::string(name))).fileName().toStdString())
      .string();
}

} // namespace AI

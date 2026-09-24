#pragma once

#include <QFuture>
#include <QObject>
#include <QThread>
#include <filesystem>
#include "agentic-loop.hpp"
#include "common/file-attachment.hpp"

namespace AI {

struct ConversationInfo {
  std::string id;
  std::string title;
  std::optional<ModelRef> model;
  std::int64_t createdAt = 0;
  std::int64_t updatedAt = 0;
};

struct ConversationAttachment {
  std::string id;
  FileAttachment data;
  std::string path;
};

struct ConversationTurn {
  std::string id;
  int position = 0;
  std::string query;
  std::string status = "running";
  Agent::Options options;
  std::string modelLabel;
  std::int64_t startedAt = 0;
  std::optional<std::int64_t> finishedAt;
  std::optional<std::string> error;
  std::vector<Agent::Message> messages;
  std::vector<Agent::ToolCall> calls;
  std::vector<ConversationAttachment> attachments;
};

struct Conversation {
  ConversationInfo info;
  std::vector<ConversationTurn> turns;
};

class ConversationStore : public QObject {
  Q_OBJECT

signals:
  void changed();
  void aboutToRemove(const QString &id);
  void removed(const QString &id);
  void titleChanged(const QString &id, const QString &title);
  void errorOccurred(const QString &error);

public:
  explicit ConversationStore(std::filesystem::path directory);
  ~ConversationStore() override;

  QFuture<Result<std::vector<ConversationInfo>>> list();
  QFuture<Result<Conversation>> load(std::string id);
  void save(ConversationInfo info, ConversationTurn turn);
  void setTitle(std::string id, std::string title);
  void setModel(std::string id, std::optional<ModelRef> model);
  QFuture<Result<void>> remove(std::string id);
  std::string attachmentPath(std::string_view conversation, std::string_view attachment,
                             std::string_view name) const;

private:
  class Worker;
  std::filesystem::path m_directory;
  QThread m_thread;
  Worker *m_worker;
};

} // namespace AI

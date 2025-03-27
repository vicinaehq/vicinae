#pragma once
#include "omni-icon.hpp"
#include <qobject.h>
#include <qstring.h>
#include <qstringview.h>
#include <qtmetamacros.h>

struct AiModel {
  QString id;
  QString displayName;
  std::optional<OmniIconUrl> iconUrl;
};

class ChatMessage {
public:
  enum Role { InvalidChatMessageRole, ChatMessageSystemRole, ChatMessageUserRole, ChatMessageAssistantRole };

private:
  QString _content;
  Role _role;

public:
  Role role() const { return _role; }
  QStringView content() const { return _content; }

  void setRole(Role role) { _role = role; }
  void setContent(QStringView content) { _content = content.toString(); }

  ChatMessage() : _role(ChatMessageUserRole) {}
  ChatMessage(QStringView content, Role role = ChatMessageUserRole) : _content(content), _role(role) {}
};

struct CompletionPayload {
  QString modelId;
  std::vector<ChatMessage> messages;
};

struct ChatCompletionToken {
  QString modelId;
  ChatMessage message;
};

class StreamedChatCompletion : public QObject {
  Q_OBJECT

public:
signals:
  void errorOccured(const QString &error);
  void tokenReady(const ChatCompletionToken &token);
  void finished();
};

class AbstractAiProvider : public QObject {
public:
  virtual QString name() const = 0;
  virtual bool isAlive() const = 0;

  virtual QFuture<std::vector<AiModel>> models() const = 0;
  virtual ChatCompletionToken createCompletion(const CompletionPayload &payload) const = 0;
  virtual StreamedChatCompletion *createStreamedCompletion(const CompletionPayload &payload) const = 0;
};

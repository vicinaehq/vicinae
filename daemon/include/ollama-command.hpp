#pragma once

#include "app.hpp"
#include "chat-widget.hpp"
#include "ollama-service.hpp"
#include "view.hpp"
#include <qlabel.h>
#include <qlineedit.h>
#include <qtextedit.h>

class AskOllamaView : public View {
  OllamaService *client = new OllamaService;
  ChatWidget *chat;

  void submitMessage(const QString &message) {
    auto reply = client->generate("deepseek-r1:32b", message);
    auto self = new ChatMessageWidget;

    self->setMarkdown(message);

    chat->addMessage(self);

    auto chatMessage = new ChatMessageWidget;

    connect(reply, &CompletionResponse::chunkReady, this,
            [this, chatMessage, reply](const CompletionChunk &chunk) {
              if (chatMessage->markdown().isEmpty() && !chunk.response.isEmpty())
                chat->addMessage(chatMessage);

              chatMessage->setMarkdown(chatMessage->markdown() + chunk.response);

              if (chunk.done) { reply->deleteLater(); }
            });
  }

public:
  AskOllamaView(AppWindow &app) : View(app), chat(new ChatWidget) {

    connect(app.topBar->input, &QLineEdit::returnPressed, this,
            [this, &app]() { submitMessage(app.topBar->input->text()); });

    auto welcome = new ChatMessageWidget();

    welcome->setMarkdown("Hello! what can I do for you?");

    chat->addMessage(welcome);

    widget = chat;
  }

  void onMount() override {}
};

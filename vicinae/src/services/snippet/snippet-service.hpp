#pragma once
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include <qtmetamacros.h>

class SnippetService : public QObject {
  Q_OBJECT

signals:
  void snippetAdded();
  void snippetUpdated();
  void snippetRemoved();
  void snippetsChanged(); // add/updated/remove

public:
  SnippetService(std::filesystem::path path) : m_db(path) {
    connect(&m_server, &SnippetServer::keywordTriggered, this, &SnippetService::handleKeywordTrigger);
  }

  bool start() {
    m_server.start();

    for (const auto &snippet : m_db.snippets()) {
      if (const auto e = snippet.expansion) { m_server.registerSnippet({.trigger = e->keyword}); }
    }

    return true;
  }

  auto createSnippet(SnippetDatabase::SnippetPayload payload) {
    auto res = m_db.addSnippet(payload);
    if (!res) return res;

    if (payload.expansion) { m_server.registerSnippet({.trigger = payload.expansion->keyword}); }
    emit snippetAdded();
    emit snippetsChanged();

    return res;
  }

  auto updateSnippet(std::string_view id, SnippetDatabase::SnippetPayload payload) {
    auto res = m_db.updateSnippet(id, payload);
    if (res) {
      if (payload.expansion) { m_server.registerSnippet({.trigger = payload.expansion->keyword}); }
      emit snippetUpdated();
      emit snippetsChanged();
    }
    return res;
  }

  auto removeSnippet(std::string_view id) {
    auto res = m_db.removeSnippet(id);

    if (!res) return res;
    if (res->expansion) { m_server.unregisterSnippet(res->expansion->keyword); }

    emit snippetRemoved();
    emit snippetsChanged();

    return res;
  }

  /**
   * The snippet server will not be running if it is explicitly disabled or
   * if it doesn't have the required permisions to do its job.
   */
  bool isServerRunning() { return m_server.isRunning(); }

  SnippetServer *server() { return &m_server; }
  SnippetDatabase *database() { return &m_db; }

private:
  void handleKeywordTrigger(std::string keyword) {
    const auto snippet = m_db.findByKeyword(keyword);
    if (!snippet || !snippet->expansion) return;

    if (const auto text = std::get_if<SnippetDatabase::TextSnippet>(&snippet->data)) {
      m_server.injectClipboardText(keyword, text->text);
    }
  }

  SnippetServer m_server;
  SnippetDatabase m_db;
};

#pragma once
#include "services/app-service/app-service.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "services/window-manager/window-manager.hpp"
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include "snippet/types.hpp"
#include <qtmetamacros.h>

class SnippetService : public QObject {
  Q_OBJECT

signals:
  void snippetAdded();
  void snippetUpdated();
  void snippetRemoved();
  void snippetsChanged(); // add/updated/remove

public:
  SnippetService(std::filesystem::path path, WindowManager &wm, const AppService &appDb)
      : m_db(path), m_wm(wm), m_appDb(appDb) {
    connect(&m_server, &SnippetServer::keywordTriggered, this, &SnippetService::handleKeywordTrigger);
  }

  bool start() {
    m_server.start();

    for (const auto &snippet : m_db.snippets()) {
      if (const auto e = snippet.expansion) {
        m_server.registerSnippet(
            {.trigger = e->keyword,
             .mode = e->word ? snippet::ipc::ExpansionMode::Word : snippet::ipc::ExpansionMode::Keydown});
      }
    }

    return true;
  }

  auto createSnippet(snippet::SnippetPayload payload) {
    auto res = m_db.addSnippet(payload);
    if (!res) return res;

    if (const auto e = payload.expansion) {
      m_server.registerSnippet(
          {.trigger = e->keyword,
           .mode = e->word ? snippet::ipc::ExpansionMode::Word : snippet::ipc::ExpansionMode::Keydown});
    }
    emit snippetAdded();
    emit snippetsChanged();

    return res;
  }

  auto updateSnippet(std::string_view id, snippet::SnippetPayload payload) {
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

    bool terminal = false;

    if (const auto focusedWindow = m_wm.getFocusedWindow()) {
      if (const auto app = m_appDb.findByClass(focusedWindow->wmClass())) {
        qDebug() << "snippet service detected that current app is a terminal";
        terminal = app->isTerminalEmulator() || app->isTerminalApp();
      }
    }

    if (const auto text = std::get_if<snippet::TextSnippet>(&snippet->data)) {
      SnippetExpander expander;
      const auto expanded = expander.expandToString(text->text.c_str(), {});
      m_server.injectClipboardText(keyword, expanded);
    }
  }

  SnippetServer m_server;
  SnippetDatabase m_db;
  const AppService &m_appDb;
  WindowManager &m_wm;
};

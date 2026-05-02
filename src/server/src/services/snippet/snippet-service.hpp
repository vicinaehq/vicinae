#pragma once
#include <linux/input-event-codes.h>
#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
#include <qtmetamacros.h>
#include "linuxutils/keyboard.hpp"
#include "services/app-service/app-service.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "services/window-manager/window-manager.hpp"
#include "snippet-server.hpp"
#include "snippet-db.hpp"

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
    // m_server.start();

    for (const auto &snippet : m_db.snippets()) {
      if (const auto e = snippet.expansion) {
        m_server.registerSnippet(
            {.trigger = e->keyword,
             .mode = e->word ? snippet_gen::ExpansionMode::Word : snippet_gen::ExpansionMode::Keydown});
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
           .mode = e->word ? snippet_gen::ExpansionMode::Word : snippet_gen::ExpansionMode::Keydown});
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

    const auto text = std::get_if<snippet::TextSnippet>(&snippet->data);
    if (!text) return;

    SnippetExpander expander;
    const auto expanded = expander.expandToString(text->text.c_str(), {});

    auto *clip = QGuiApplication::clipboard();
    auto *expansionData = new QMimeData;
    expansionData->setData("text/plain;charset=utf-8", expanded.toUtf8());
    expansionData->setData("vicinae/concealed", "1");
    clip->setMimeData(expansionData);

    bool terminal = false;
    if (const auto focusedWindow = m_wm.getFocusedWindow()) {
      if (const auto app = m_appDb.findByClass(focusedWindow->wmClass())) {
        terminal = app->isTerminalEmulator() || app->isTerminalApp();
      }
    }

    const bool wordMode = snippet->expansion->word;
    const int eraseCount = static_cast<int>(keyword.size()) + (wordMode ? 1 : 0);

    using Mod = linuxutils::UInputKeyboard::Modifier;
    auto mods = static_cast<int>(Mod::Ctrl);
    if (terminal) mods = static_cast<int>(Mod::Ctrl | Mod::Shift);

    m_keyboard.repeatKey(KEY_BACKSPACE, eraseCount);
    usleep(2000);
    m_keyboard.sendKey(KEY_V, mods);

    if (wordMode) {
      usleep(2000);
      m_keyboard.sendKey(KEY_SPACE, 0);
    }
  }

  linuxutils::UInputKeyboard m_keyboard;
  SnippetServer m_server;
  SnippetDatabase m_db;
  WindowManager &m_wm;
  const AppService &m_appDb;
};

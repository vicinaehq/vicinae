#pragma once
#include "config/config.hpp"
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/keyboard/abstract-keyboard-service.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "services/window-manager/window-manager.hpp"
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include <QTimer>
#include <qtmetamacros.h>

class SnippetService : public QObject {
  Q_OBJECT

signals:
  void snippetAdded();
  void snippetUpdated();
  void snippetRemoved();
  void snippetsChanged(); // add/updated/remove

public:
  SnippetService(std::filesystem::path path, WindowManager &wm, const AppService &appDb,
                 AbstractKeyboardService &keyboard, ClipboardService &clipboard)
      : m_db(path), m_wm(wm), m_appDb(appDb), m_keyboard(keyboard), m_clipboard(clipboard) {
    connect(&m_server, &SnippetServer::keywordTriggered, this, &SnippetService::handleKeywordTrigger);
    connect(&wm, &WindowManager::focusChanged, this, [this]() {
      if (m_server.isRunning()) { m_server.resetContext(); }
    });
  }

  bool start(const config::SnippetConfig &cfg) {
    if (!cfg.enabled) return true;

    m_prePasteDelay = cfg.prePasteDelay;

    m_server.start();

    if (!m_server.isRunning()) return false;

    if (!cfg.layout.empty()) { m_server.setKeymap({.layout = cfg.layout}); }

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
  void handleKeywordTrigger(const std::string &keyword) {
    const auto snippet = m_db.findByKeyword(keyword);
    if (!snippet || !snippet->expansion) return;

    bool terminal = false;
    const auto focusedWindow = m_wm.getFocusedWindow();

    if (focusedWindow) {
      if (const auto app = m_appDb.findByClass(focusedWindow->wmClass())) {
        terminal = app->isTerminalEmulator() || app->isTerminalApp();
      }
    }

    qInfo().nospace() << "Snippet expansion: keyword=\"" << keyword
                      << "\" window=" << (focusedWindow ? focusedWindow->wmClass() : "<unknown>")
                      << " terminal=" << terminal;

    if (const auto text = std::get_if<snippet::TextSnippet>(&snippet->data)) {
      SnippetExpander expander;
      const auto result = expander.expand(QString::fromStdString(text->text), {});

      const auto expanded = result.parts | std::views::transform([](auto &&part) { return part.text; }) |
                            std::views::join | std::ranges::to<QString>();

      m_clipboard.copyText(expanded, {.concealed = true});

      const int charsToDelete = static_cast<int>(keyword.size()) + (snippet->expansion->word ? 1 : 0);
      const bool wordMode = snippet->expansion->word;

      auto inject = [=, this]() {
        m_keyboard.backspace(charsToDelete);
        m_keyboard.paste(terminal);

        if (wordMode) { m_keyboard.space(); }

        if (result.cursorPos) {
          const int totalLength = expanded.size();
          const int leftMoves = totalLength - *result.cursorPos;
          if (leftMoves > 0) { m_keyboard.moveCursorLeft(leftMoves); }
        }

        m_clipboard.scheduleClipboardRestore(200);
      };

      if (m_prePasteDelay > 0) {
        QTimer::singleShot(m_prePasteDelay, this, inject);
      } else {
        inject();
      }
    }
  }

  SnippetServer m_server;
  SnippetDatabase m_db;
  WindowManager &m_wm;
  const AppService &m_appDb;
  AbstractKeyboardService &m_keyboard;
  ClipboardService &m_clipboard;
  int m_prePasteDelay = 50;
};

#pragma once
#include "services/app-service/app-service.hpp"
#include "services/keyboard/abstract-keyboard-service.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "services/window-manager/window-manager.hpp"
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include <QClipboard>
#include <QGuiApplication>
#include <QMimeData>
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
                 AbstractKeyboardService &keyboard)
      : m_db(path), m_wm(wm), m_appDb(appDb), m_keyboard(keyboard) {
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
  void handleKeywordTrigger(const std::string &keyword) {
    const auto snippet = m_db.findByKeyword(keyword);
    if (!snippet || !snippet->expansion) return;

    bool terminal = false;

    if (const auto focusedWindow = m_wm.getFocusedWindow()) {
      if (const auto app = m_appDb.findByClass(focusedWindow->wmClass())) {
        terminal = app->isTerminalEmulator() || app->isTerminalApp();
      }
    }

    if (const auto text = std::get_if<snippet::TextSnippet>(&snippet->data)) {
      SnippetExpander expander;
      const auto result = expander.expand(QString::fromStdString(text->text), {});

      const auto expanded = result.parts | std::views::transform([](auto &&part) { return part.text; }) |
                            std::views::join | std::ranges::to<QString>();

      auto *clip = QGuiApplication::clipboard();
      auto *mimeData = new QMimeData;
      mimeData->setData("text/plain;charset=utf-8", expanded.toUtf8());
      mimeData->setData("vicinae/concealed", "1");
      clip->setMimeData(mimeData);

      const int charsToDelete = static_cast<int>(keyword.size()) + (snippet->expansion->word ? 1 : 0);
      m_keyboard.backspace(charsToDelete);
      m_keyboard.paste(terminal);

      if (snippet->expansion->word) { m_keyboard.space(); }

      if (result.cursorPos) {
        const int totalLength = expanded.size();
        const int leftMoves = totalLength - *result.cursorPos;
        if (leftMoves > 0) { m_keyboard.moveCursorLeft(leftMoves); }
      }
    }
  }

  SnippetServer m_server;
  SnippetDatabase m_db;
  WindowManager &m_wm;
  const AppService &m_appDb;
  AbstractKeyboardService &m_keyboard;
};

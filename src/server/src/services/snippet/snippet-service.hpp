#pragma once
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/keyboard/abstract-keyboard-service.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "services/window-manager/window-manager.hpp"
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include <QThreadPool>
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
  SnippetService(const std::filesystem::path &path, WindowManager &wm, const AppService &appDb,
                 AbstractKeyboardService &keyboard, ClipboardService &clipboard)
      : m_db(path), m_wm(wm), m_appDb(appDb), m_keyboard(keyboard), m_clipboard(clipboard) {
    connect(&m_server, &SnippetServer::keywordTriggered, this, &SnippetService::handleKeywordTrigger);
    connect(&m_server, &SnippetServer::undoTriggered, this, &SnippetService::handleUndo);
    connect(&wm, &WindowManager::focusChanged, this, [this]() {
      if (m_server.isRunning()) { m_server.resetContext(); }
    });
  }

  bool start() {
    m_server.start();

    if (!m_server.isRunning()) return false;

    for (const auto &snippet : m_db.snippets()) {
      if (const auto e = snippet.expansion) {
        m_server.registerSnippet(
            {.trigger = e->keyword,
             .mode = e->word ? snippet_gen::ExpansionMode::Word : snippet_gen::ExpansionMode::Keydown});
      }
    }

    return true;
  }

  auto createSnippet(const snippet::SnippetPayload &payload) {
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

  static constexpr int DEFAULT_PRE_PASTE_DELAY_MS = 100;

  /**
   * The snippet server will not be running if it is explicitly disabled or
   * if it doesn't have the required permisions to do its job.
   */
  bool isServerRunning() { return m_server.isRunning(); }

  void setEnabled(bool enabled) { m_enabled = enabled; }
  void setUndoEnabled(bool enabled) { m_undoEnabled = enabled; }
  void setPrePasteDelay(int ms) { m_prePasteDelay = ms; }
  void setLayout(const std::string &layout) {
    if (!layout.empty()) { m_server.setKeymap({.layout = layout}); }
  }

  SnippetServer *server() { return &m_server; }
  SnippetDatabase *database() { return &m_db; }

private:
  struct UndoRecord {
    std::string trigger;
    QString expandedText;
    bool wordMode = false;
  };

  void handleUndo(const std::string &trigger) {
    if (!m_undoEnabled || !m_undoRecord || m_undoRecord->trigger != trigger) return;

    // In non-word mode, the user's backspace already deleted one char of the expanded text.
    // In word mode, it deleted the trailing space, so the full expanded text remains.
    const int backspaceCount =
        static_cast<int>(m_undoRecord->expandedText.size()) - (m_undoRecord->wordMode ? 0 : 1);
    m_undoRecord.reset();

    QThreadPool::globalInstance()->start([this, backspaceCount, trigger]() {
      if (backspaceCount > 0) { m_keyboard.backspace(backspaceCount); }
      m_keyboard.typeText(trigger);
    });
  }

  void handleKeywordTrigger(const std::string &keyword) {
    if (!m_enabled) return;

    const auto snippet = m_db.findByKeyword(keyword);
    if (!snippet || !snippet->expansion) return;

    bool terminal = false;
    const auto focusedWindow = m_wm.getFocusedWindow();

    if (focusedWindow) {
      if (const auto app = m_appDb.findByClass(focusedWindow->wmClass())) {
        terminal = app->isTerminalEmulator() || app->isTerminalApp();
      }
    }

    const auto &apps = snippet->expansion->apps;
    if (!apps.empty()) {
      if (!focusedWindow) return;
      const auto app = m_appDb.findByClass(focusedWindow->wmClass());
      if (!app) return;
      const auto appId = app->id().toStdString();
      if (std::ranges::find(apps, appId) == apps.end()) return;
    }

    qInfo().nospace() << "Snippet expansion: keyword=\"" << keyword
                      << "\" window=" << (focusedWindow ? focusedWindow->wmClass() : "<unknown>")
                      << " terminal=" << terminal;

    const int charsToDelete = static_cast<int>(keyword.size()) + (snippet->expansion->word ? 1 : 0);
    const bool wordMode = snippet->expansion->word;

    const auto *text = std::get_if<snippet::TextSnippet>(&snippet->data);
    if (!text) return;

    SnippetExpander expander;
    const auto result = expander.expand(QString::fromStdString(text->text), {});

    const auto expanded = result.parts | std::views::transform([](auto &&part) { return part.text; }) |
                          std::views::join | std::ranges::to<QString>();

    m_clipboard.copyText(expanded, {.concealed = true});

    if (!result.cursorPos) {
      m_undoRecord = UndoRecord{.trigger = keyword, .expandedText = expanded, .wordMode = wordMode};
    } else {
      m_undoRecord.reset();
    }

    QTimer::singleShot(
        m_prePasteDelay, this,
        [this, charsToDelete, terminal, wordMode, cursorPos = result.cursorPos,
         expandedSize = expanded.size()]() {
          QThreadPool::globalInstance()->start([this, charsToDelete, terminal, wordMode, cursorPos, expandedSize]() {
            m_keyboard.backspace(charsToDelete);
            m_keyboard.paste(terminal);

            if (wordMode) { m_keyboard.space(); }

            if (cursorPos) {
              int leftMoves = static_cast<int>(expandedSize) - *cursorPos;
              if (wordMode) { ++leftMoves; }
              if (leftMoves > 0) { m_keyboard.moveCursorLeft(leftMoves); }
            }

            QMetaObject::invokeMethod(this, [this]() { m_clipboard.scheduleClipboardRestore(); });
          });
        });
  }

  SnippetServer m_server;
  SnippetDatabase m_db;
  WindowManager &m_wm;
  const AppService &m_appDb;
  AbstractKeyboardService &m_keyboard;
  ClipboardService &m_clipboard;
  bool m_enabled = true;
  bool m_undoEnabled = true;
  int m_prePasteDelay = DEFAULT_PRE_PASTE_DELAY_MS;
  std::optional<UndoRecord> m_undoRecord;
};

#pragma once
#include "services/app-service/app-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/snippet/snippet-expander.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"
#include "snippet-server.hpp"
#include "snippet-db.hpp"
#include <qguiapplication.h>
#include <qobject.h>
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
                 ClipboardService &clipboard)
      : m_db(path), m_wm(wm), m_appDb(appDb), m_clipboard(clipboard) {
    connect(&m_server, &SnippetServer::keywordTriggered, this, &SnippetService::handleKeywordTrigger);
    connect(&m_server, &SnippetServer::undoTriggered, this, &SnippetService::handleUndo);
    connect(&wm, &WindowManager::focusChanged, this, [this]() {
      m_undoRecord.reset();
      m_server.cancelInjection();
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
    std::optional<std::string> oldKeyword;
    if (const auto *existing = m_db.findById(id); existing && existing->expansion) {
      oldKeyword = existing->expansion->keyword;
    }

    auto res = m_db.updateSnippet(id, payload);
    if (res) {
      const auto newKeyword = payload.expansion ? std::optional{payload.expansion->keyword} : std::nullopt;

      if (oldKeyword && oldKeyword != newKeyword) { m_server.unregisterSnippet(*oldKeyword); }

      if (payload.expansion) {
        m_server.registerSnippet({.trigger = payload.expansion->keyword,
                                  .mode = payload.expansion->word ? snippet_gen::ExpansionMode::Word
                                                                  : snippet_gen::ExpansionMode::Keydown});
      }

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

  static constexpr int DEFAULT_PRE_PASTE_DELAY_MS = 0;
  static constexpr int DEFAULT_KEY_DELAY_US = 2000;

  /**
   * The snippet server will not be running if it is explicitly disabled or
   * if it doesn't have the required permisions to do its job.
   */
  bool isServerRunning() { return m_server.isRunning(); }

  void setEnabled(bool enabled) { m_enabled = enabled; }
  void setUndoEnabled(bool enabled) { m_undoEnabled = enabled; }
  void setPrePasteDelay(int ms) { m_prePasteDelay = ms; }
  void setKeyDelay(int us) { m_server.setKeyDelay(us); }
  void setLayout(const std::string &layout) {
    if (!layout.empty()) { m_server.setKeymap({.layout = layout}); }
  }

  SnippetServer *server() { return &m_server; }
  SnippetDatabase *database() { return &m_db; }

private:
  struct UndoRecord {
    std::string trigger;
    QString expandedText;
  };

  static constexpr int MAX_UNDO_LENGTH = 50;

  void handleUndo(const std::string &trigger) {
    if (!m_undoEnabled || !m_undoRecord || m_undoRecord->trigger != trigger) return;
    if (m_undoRecord->expandedText.size() > MAX_UNDO_LENGTH) {
      m_undoRecord.reset();
      return;
    }

    const int backspaceCount = static_cast<int>(m_undoRecord->expandedText.size()) - 1;
    m_undoRecord.reset();
    m_server.injectUndo(backspaceCount, trigger);
  }

  void handleKeywordTrigger(const std::string &keyword) {
    if (!m_enabled) return;

    const auto snippet = m_db.findByKeyword(keyword);
    if (!snippet || !snippet->expansion) return;

    bool terminal = false;

    // we have focus, and we are a layer so it means that the current window might be:
    // - the previously focused window, not invalidated by the layer grab (Hyprland does this)
    // - nullptr as there is technically no window owning focus (a layer is not a window)
    // this logic is here so that we don't mistakingly craft the shortcut from stale data when expanding in
    // vicinae itself (e.g using ctrl+shift+v because the last window is a terminal, but we are really
    // focusing the vicinae window)
    // FIXME: currently this logic will, if not using layer shell, null the window even if the currently
    // focused window is the vicinae one. The only problem with this is that app filtering won't work on the
    // vicinae window itself. We probably can fix this by just checking whether we are a layer here, but
    // requires pulling config...
    AbstractWindowManager::WindowPtr focusedWindow = nullptr;

    {
      if (QGuiApplication::focusWindow() && !m_wm.provider()->focusNullsOnLayerGrab()) {
        focusedWindow = nullptr;
      } else {
        focusedWindow = m_wm.getFocusedWindow();
      }
    }

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

    const auto *text = std::get_if<snippet::TextSnippet>(&snippet->data);
    if (!text) return;

    SnippetExpander expander;
    const auto result = expander.expand(QString::fromStdString(text->text), {});

    auto expanded = result.parts | std::views::transform([](auto &&part) { return part.text; }) |
                    std::views::join | std::ranges::to<QString>();

    if (snippet->expansion->word) { expanded.append(' '); }

    m_clipboard.copyText(expanded, {.concealed = true});

    if (!result.cursorPos) {
      m_undoRecord = UndoRecord{.trigger = keyword, .expandedText = expanded};
    } else {
      m_undoRecord.reset();
    }

    int cursorLeftMoves = 0;
    if (result.cursorPos) {
      int moves = static_cast<int>(expanded.size()) - *result.cursorPos;
      if (moves > 0) { cursorLeftMoves = moves; }
    }

    m_server.injectExpand(charsToDelete, m_prePasteDelay * 1000, terminal, cursorLeftMoves);
    QTimer::singleShot(0, this, [this]() { m_clipboard.scheduleClipboardRestore(); });
  }

  SnippetServer m_server;
  SnippetDatabase m_db;
  WindowManager &m_wm;
  const AppService &m_appDb;
  ClipboardService &m_clipboard;
  bool m_enabled = true;
  bool m_undoEnabled = true;
  int m_prePasteDelay = DEFAULT_PRE_PASTE_DELAY_MS;
  std::optional<UndoRecord> m_undoRecord;
};

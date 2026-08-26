#pragma once
#include "abstract-snippet-server.hpp"
#include <QTimer>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

class MacosSnippetServer : public AbstractSnippetServer {
  Q_OBJECT

public:
  MacosSnippetServer();
  ~MacosSnippetServer() override;

  void registerSnippet(snippet_gen::CreateSnippetRequest payload) override;
  void unregisterSnippet(std::string_view keyword) override;
  void setKeymap(snippet_gen::LayoutInfo info) override;
  void resetContext() override;

  void injectExpand(const std::string &text, unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                    unsigned cursorLeftMoves) override;
  void injectUndo(unsigned backspaceCount, const std::string &trigger) override;
  void setKeyDelay(int us) override;
  bool supportsKeyInjection() const override;
  bool usesClipboard() const override { return false; }

  bool isRunning() const override;

  void ensureTapRunning();

  // called from the CGEventTap thread
  void onKey(int keycode, const std::string &utf8, bool blockingMods);
  void reenableTap();

private:
  struct Snippet {
    std::string trigger;
    snippet_gen::ExpansionMode mode;
  };

  void startTapThread();
  void runTap();
  void emitExpansionLocked(const Snippet &snippet);

  std::vector<Snippet> m_snippets;
  std::string m_text;
  std::optional<std::string> m_undoTrigger;
  std::mutex m_mutex;

  QTimer m_permissionRetryTimer;
  std::thread m_thread;
  std::atomic<void *> m_runLoop{nullptr};
  std::atomic_bool m_tapThreadDone{false};
  void *m_tap = nullptr;
  void *m_source = nullptr;
  std::atomic_bool m_running{false};
  std::atomic_int m_keyDelayUs{5000};
};

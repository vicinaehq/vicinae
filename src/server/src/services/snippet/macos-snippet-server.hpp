#pragma once
#include "abstract-snippet-server.hpp"
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

  void injectExpand(unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                    unsigned cursorLeftMoves) override;
  void injectUndo(unsigned backspaceCount, const std::string &trigger) override;
  void setKeyDelay(int us) override;
  bool supportsKeyInjection() const override;

  bool isRunning() const override;

  // Invoked from the CGEventTap thread for every observed key/flags event.
  void onKey(bool flagsChanged, int keycode, const std::string &utf8, bool blockingMods);

private:
  struct Snippet {
    std::string trigger;
    snippet_gen::ExpansionMode mode;
  };

  void runTap();
  void emitExpansionLocked(const Snippet &snippet, bool blockingMods);
  void flushPendingLocked();

  std::vector<Snippet> m_snippets;
  std::string m_text;
  std::optional<Snippet> m_pendingExpansion;
  std::optional<std::string> m_undoTrigger;
  std::mutex m_mutex;

  std::thread m_thread;
  std::atomic<void *> m_runLoop{nullptr};
  void *m_tap = nullptr;
  void *m_source = nullptr;
  std::atomic_bool m_running{false};
  std::atomic_int m_keyDelayUs{2000};
};

#pragma once
#include "abstract-snippet-server.hpp"
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

class WindowsSnippetServer : public AbstractSnippetServer {
  Q_OBJECT

public:
  WindowsSnippetServer();
  ~WindowsSnippetServer() override;

  void registerSnippet(snippet_gen::CreateSnippetRequest payload) override;
  void unregisterSnippet(std::string_view keyword) override;
  void setKeymap(snippet_gen::LayoutInfo info) override;
  void resetContext() override;

  void injectExpand(const std::string &text, unsigned charsToDelete, unsigned prePasteDelayUs, bool terminal,
                    unsigned cursorLeftMoves, bool viaClipboard) override;
  void injectUndo(unsigned backspaceCount, const std::string &trigger) override;
  void setKeyDelay(int us) override;
  bool supportsKeyInjection() const override { return true; }
  bool usesClipboard(std::size_t expandedLength) const override;

  bool isRunning() const override;

  // called from the low-level keyboard hook thread
  void onKey(unsigned vk, const std::string &utf8, bool blockingMods);

private:
  struct Snippet {
    std::string trigger;
    snippet_gen::ExpansionMode mode;
  };

  void startHookThread();
  void emitExpansionLocked(const Snippet &snippet);

  std::vector<Snippet> m_snippets;
  std::string m_text;
  std::optional<std::string> m_undoTrigger;
  std::mutex m_mutex;

  std::thread m_thread;
  unsigned long m_hookThreadId = 0;
  std::atomic_bool m_running{false};
};

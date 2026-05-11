#pragma once
#include <atomic>
#include <optional>
#include <sys/epoll.h>
#include <libudev.h>
#include <unistd.h>
#include <xkbcommon/xkbcommon.h>
#include "linuxutils/keyboard.hpp"
#include "types.hpp"

class Frame;

namespace snippet {

class SnippetService : public snippet_gen::AbstractSnippet {
public:
  SnippetService(snippet_gen::RpcTransport &transport);

  std::expected<void, std::string> setKeymap(snippet_gen::LayoutInfo info) override;
  std::expected<snippet_gen::CreateSnippetResponse, std::string>
  createSnippet(snippet_gen::CreateSnippetRequest req) override;
  std::expected<snippet_gen::RemoveSnippetResponse, std::string>
  removeSnippet(snippet_gen::RemoveSnippetRequest req) override;
  std::expected<void, std::string> resetContext() override;
  std::expected<void, std::string> injectExpand(snippet_gen::InjectExpandRequest req) override;
  std::expected<void, std::string> injectUndo(snippet_gen::InjectUndoRequest req) override;
  std::expected<void, std::string> injectPaste(snippet_gen::InjectPasteRequest req) override;
  std::expected<void, std::string> cancelInjection() override;
  std::expected<void, std::string> setKeyDelay(int delayUs) override;
  std::expected<snippet_gen::KeyboardCapabilities, std::string> getCapabilities() override;

  void listen(snippet_gen::Server &server);
  void setIpcFrame(Frame *frame) { m_ipcFrame = frame; }

  struct Snippet {
    std::string trigger;
    snippet_gen::ExpansionMode mode;
  };

private:
  void setLayout(const snippet_gen::LayoutInfo &info);
  std::vector<std::string> enumerateKeyboards();
  void emitExpansion(const Snippet &snippet);
  bool hasActiveModifiers() const;
  void flushPendingExpansion();
  void drainStdin();

  std::string m_text;
  std::optional<std::string> m_undoTrigger;
  std::optional<Snippet> m_pendingExpansion;
  udev *m_udev = nullptr;
  xkb_context *m_xkb = nullptr;
  xkb_keymap *m_keymap = nullptr;
  xkb_state *m_kbState = nullptr;
  std::vector<Snippet> m_snippets;
  linuxutils::UInputKeyboard m_keyboard;
  std::atomic<bool> m_cancelInjection{false};
  Frame *m_ipcFrame = nullptr;
};

}; // namespace snippet

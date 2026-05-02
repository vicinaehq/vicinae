#pragma once
#include "types.hpp"
#include <sys/epoll.h>
#include <libudev.h>
#include <unistd.h>
#include <unordered_map>
#include <xkbcommon/xkbcommon.h>

namespace snippet {

class SnippetService : public snippet_gen::AbstractSnippet {
public:
  SnippetService(snippet_gen::RpcTransport &transport);

  std::expected<void, std::string> setKeymap(snippet_gen::LayoutInfo info) override;
  std::expected<snippet_gen::CreateSnippetResponse, std::string>
  createSnippet(snippet_gen::CreateSnippetRequest req) override;
  std::expected<snippet_gen::RemoveSnippetResponse, std::string>
  removeSnippet(snippet_gen::RemoveSnippetRequest req) override;

  void listen(snippet_gen::Server &server);

  struct Snippet {
    std::string trigger;
    snippet_gen::ExpansionMode mode;
  };

private:
  void setLayout(const snippet_gen::LayoutInfo &info);
  std::vector<std::string> enumerateKeyboards();
  void emitExpansion(const Snippet &snippet);

  std::string m_text;
  udev *m_udev = nullptr;
  xkb_context *m_xkb = nullptr;
  xkb_keymap *m_keymap = nullptr;
  xkb_state *m_kbState = nullptr;
  std::unordered_map<std::string, Snippet> m_snippetMap;
};

}; // namespace snippet

#pragma once
#include "types.hpp"
#include "keyboard.hpp"
#include <sys/epoll.h>
#include <libudev.h>
#include <unistd.h>
#include <unordered_map>
#include <xkbcommon/xkbcommon.h>

namespace snippet {

class Server {
public:
  Server();
  ~Server();

  void listen();

protected:
  std::vector<std::string> enumerateKeyboards();
  void setupIPC();
  void setLayout(const LayoutInfo &info);

  template <typename T> void notify(typename T::Request req) {
    const auto json = m_client.notify<T>(req);
    uint32_t size = json.size();
    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(json.data(), json.size());
    std::cout.flush();
  };

  struct Snippet {
    std::string trigger;
    ipc::ExpansionMode mode;
  };

  void emitExpansion(const Snippet &snipet);

private:
  UInputKeyboard m_kb;
  std::string m_text;
  udev *m_udev = nullptr;
  xkb_context *m_xkb = nullptr;
  xkb_keymap *m_keymap = nullptr;
  xkb_state *m_kbState = nullptr;
  std::unordered_map<std::string, Snippet> m_snippetMap;

  ::ipc::RpcServer<snippet::ipc::ClientSchema> m_server; // vicinae -> snippet requests
  ::ipc::RpcClient<snippet::ipc::ServerSchema> m_client; // snippet -> vicinae requests/notifications
};

}; // namespace snippet

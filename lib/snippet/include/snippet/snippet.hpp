#pragma once
#include "vicinae-ipc/ipc.hpp"
#include <libudev.h>
#include <unordered_map>
#include <xkbcommon/xkbcommon.h>

namespace snippet {

namespace ipc {
struct SetKeymap {
  static constexpr const auto key = "keymap/set";
  struct Request {
    std::string layout;
    std::optional<std::string> rules;
    std::optional<std::string> model;
    std::optional<std::string> variant;
    std::optional<std::string> options;
  };
  struct Response {};
};

struct CreateSnippet {
  static constexpr const auto key = "snippet/create";
  struct Request {
    std::string trigger;
  };
  struct Response {
    bool ok;
  };
};

struct RemoveSnippet {
  static constexpr const auto key = "snippet/remove";
  struct Request {
    std::string trigger;
  };
  struct Response {
    bool removed;
  };
};

struct TriggerSnippet {
  static constexpr const auto key = "snippet/trigger";
  struct Request {
    std::string trigger;
  };
  struct Response {};
};

using ClientSchema = ::ipc::RpcSchema<SetKeymap, CreateSnippet, RemoveSnippet>;
using ServerSchema = ::ipc::RpcSchema<TriggerSnippet>;

}; // namespace ipc

class Server {
public:
  Server();
  ~Server();

  void listen();

protected:
  std::vector<std::string> enumerateKeyboards();
  void setupIPC();

private:
  struct Snippet {
    std::string trigger;
  };

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

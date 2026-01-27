#pragma once
#include "vicinae-ipc/ipc.hpp"

namespace snippet {

namespace ipc {
struct SetKeymap {
  static constexpr const auto key = "keymap/set";
  struct Request {
    std::string layout;
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
  void listen();
};

}; // namespace snippet

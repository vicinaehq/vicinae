#pragma once
#include "vicinae-ipc/ipc.hpp"
#include "keyboard.hpp"
#include <cstdint>
#include <libudev.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <xkbcommon/xkbcommon.h>

namespace snippet {

struct LayoutInfo {
  std::string layout;
  std::optional<std::string> rules;
  std::optional<std::string> model;
  std::optional<std::string> variant;
  std::optional<std::string> options;
};

namespace ipc {
struct SetKeymap {
  static constexpr const auto key = "keymap/set";
  using Request = LayoutInfo;
  struct Response {};
};

enum class ExpansionMode : std::uint8_t {
  Keydown, // when the last key required to produce the trigger is pressed down
  Word,    // when space or enter is pressed after the trigger
};

struct CreateSnippet {
  static constexpr const auto key = "snippet/create";
  struct Request {
    std::string trigger;
    ExpansionMode mode = ExpansionMode::Word;
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

struct InjectClipboardExpansion {
  static constexpr const auto key = "snippet/inject";

  struct Request {
    std::string trigger;
    bool terminal = false;
  };

  struct Response {};
};

struct TriggerSnippet {
  static constexpr const auto key = "snippet/trigger";
  struct Request {
    std::string trigger;
  };
  struct Response {};
};

using ClientSchema = ::ipc::RpcSchema<SetKeymap, CreateSnippet, RemoveSnippet, InjectClipboardExpansion>;
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

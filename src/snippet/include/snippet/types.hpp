#pragma once
#include "vicinae-ipc/ipc.hpp"
#include <cstdint>
#include <optional>
#include <string>

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

}; // namespace snippet

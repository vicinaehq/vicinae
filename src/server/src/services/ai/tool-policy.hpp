#pragma once
#include <cstdint>
#include <glaze/core/common.hpp>

namespace AI {

enum class ToolPolicy : std::uint8_t { Disabled, Ask, Allow };

} // namespace AI

template <> struct glz::meta<AI::ToolPolicy> {
  using enum AI::ToolPolicy;
  static constexpr auto value = glz::enumerate("disabled", Disabled, "ask", Ask, "allow", Allow);
};

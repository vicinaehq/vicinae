#pragma once
#include <optional>
#include <cstdint>

template <typename T> struct EventCounted {
  T value;
  std::optional<uint32_t> eventCount;
};

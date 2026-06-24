#pragma once
#include <cstdint>

inline constexpr uint32_t EVENT_COUNT_UNTRACKED = UINT32_MAX;

template <typename T> struct EventCounted {
  T value;
  uint32_t eventCount;
};

#pragma once
#include "extend/node-props.hpp"
#include <cstdint>

inline constexpr uint32_t EVENT_COUNT_UNTRACKED = UINT32_MAX;

template <typename T> struct EventCounted {
  T value;
  uint32_t eventCount;
};

template <typename T> EventCounted<T> parseEventCounted(const glz::generic::object_t &obj, T value) {
  return {std::move(value), static_cast<uint32_t>(node_props::getInt(obj, "eventCount"))};
}

#pragma once
#include <cstdint>
#include <qjsonobject.h>
#include <qjsonvalue.h>

inline constexpr uint32_t EVENT_COUNT_UNTRACKED = UINT32_MAX;

template <typename T> struct EventCounted {
  T value;
  uint32_t eventCount;
};

template <typename T> EventCounted<T> parseEventCounted(const QJsonObject &obj, T value) {
  return {std::move(value), static_cast<uint32_t>(obj.value("eventCount").toInt())};
}

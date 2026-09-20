#pragma once
#include <string_view>
#include <QString>

namespace QuickAI {

constexpr auto REPOSITORY_ID = std::string_view("ai");
constexpr auto COMMAND_ID = std::string_view("quick");
constexpr auto MODEL_PREFERENCE = std::string_view("model");
constexpr auto NO_MODEL = std::string_view("none");

inline QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

} // namespace QuickAI

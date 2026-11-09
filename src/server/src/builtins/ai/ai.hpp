#pragma once
#include <string_view>
#include <QString>
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace QuickAI {

constexpr auto REPOSITORY_ID = std::string_view("ai");
constexpr auto COMMAND_ID = std::string_view("quick");
constexpr auto COLOR = SemanticColor::Accent;

inline const auto ICON = ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(COLOR);

inline QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

} // namespace QuickAI

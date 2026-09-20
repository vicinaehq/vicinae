#pragma once
#include <string_view>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <QString>
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace Dictation {
enum class DictationAction { PasteToActiveWindow, CopyToClipboard };
}

template <> struct glz::meta<Dictation::DictationAction> {
  using enum Dictation::DictationAction;
  static constexpr auto value = glz::enumerate("paste", PasteToActiveWindow, "copy", CopyToClipboard);
};

namespace Dictation {

constexpr auto REPOSITORY_ID = std::string_view("dictation");
constexpr auto NO_MODEL = std::string_view("none");
constexpr auto AUTO_LANGUAGE = std::string_view("auto");
constexpr auto COLOR = SemanticColor::Blue;

inline const auto ICON = ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(COLOR);
inline const auto VOCABULARY_ICON = ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(COLOR);

inline QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

} // namespace Dictation

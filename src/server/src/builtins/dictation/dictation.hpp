#pragma once
#include <string_view>
#include <utility>
#include <QString>
#include "services/builtin-icon/builtin-icon.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

namespace Dictation {
enum class DictationAction { PasteToActiveWindow, CopyToClipboard };

constexpr auto REPOSITORY_ID = std::string_view("dictation");
constexpr auto MODEL_PREFERENCE = std::string_view("model");
constexpr auto NO_MODEL = std::string_view("none");
constexpr auto LANGUAGE_PREFERENCE = std::string_view("language");
constexpr auto AUTO_LANGUAGE = std::string_view("auto");
constexpr auto HISTORY_PREFERENCE = std::string_view("history");
constexpr auto COLOR = SemanticColor::Blue;

constexpr auto ACTION_PASTE = "paste";
constexpr auto ACTION_COPY = "copy";

inline DictationAction dictationActionFromString(std::string_view id) {
  if (id == "copy") return DictationAction::CopyToClipboard;
  if (id == "paste") return DictationAction::PasteToActiveWindow;
  std::unreachable();
}

inline std::string_view dictationActionToString(DictationAction id) {
  switch (id) {
  case DictationAction::CopyToClipboard:
    return ACTION_COPY;
  case DictationAction::PasteToActiveWindow:
    return ACTION_PASTE;
  }
}

inline const auto ICON = ImageURL::builtin(BuiltinIcon::Microphone).setBackgroundTint(COLOR);
inline const auto VOCABULARY_ICON = ImageURL::builtin(BuiltinIcon::BookAntique).setBackgroundTint(COLOR);

inline QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

} // namespace Dictation

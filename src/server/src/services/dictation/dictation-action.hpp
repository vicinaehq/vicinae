#pragma once
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>

namespace Dictation {
enum class DictationAction { PasteToActiveWindow, CopyToClipboard };
}

template <> struct glz::meta<Dictation::DictationAction> {
  using enum Dictation::DictationAction;
  static constexpr auto value = glz::enumerate("paste", PasteToActiveWindow, "copy", CopyToClipboard);
};

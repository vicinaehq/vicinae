#pragma once

// clang-format off
static const std::unordered_set<std::string_view> flagMimes = {
	"x-kde-passwordManagerHint",
	"vicinae/concealed",
};

static const std::unordered_set<std::string_view> dataMimes = {
	"x-special/gnome-copied-files"
};

static const std::unordered_set<std::string_view> ignoredMimes = {
	"application/x-qt-image" // we only pick one image format, we don't need this hint
};
// clang-format on

namespace Wayland {
inline bool isDataMime(std::string_view mime) {
  if (ignoredMimes.contains(mime)) return false;
  return mime.starts_with("text/") || mime.starts_with("image/") || mime.starts_with("application/") ||
         dataMimes.contains(mime);
}

/**
 * Mime types that should be preserved as part of selections, but data shouldn't be requested for them.
 */
inline bool isFlagMime(std::string_view mime) { return flagMimes.contains(mime); }

}; // namespace Wayland

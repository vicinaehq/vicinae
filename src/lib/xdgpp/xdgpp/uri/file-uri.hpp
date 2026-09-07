#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace xdgpp {

/**
 * Percent-encoded `file://` URI for a local path, as produced by g_filename_to_uri.
 */
std::string toFileUri(const std::filesystem::path &path);

/**
 * Local path for a `file://` URI with no host (or `localhost`). Any other scheme or host yields nothing.
 */
std::optional<std::filesystem::path> fromFileUri(std::string_view uri);

} // namespace xdgpp

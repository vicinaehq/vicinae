#pragma once
#include <filesystem>
#include <span>
#include <string_view>

namespace xdgpp {

/**
 * Make `appId` the default application for every mime type in `mimes`, editing the given mimeapps.list in
 * place. The app is also added to the added associations, as `xdg-mime default` does.
 * Every other line of the file is preserved as is.
 */
bool setDefaultApplication(std::span<const std::string_view> mimes, std::string_view appId,
                           const std::filesystem::path &path);

/**
 * Same as above, targeting the user's mimeapps.list in the XDG config home.
 */
bool setDefaultApplication(std::span<const std::string_view> mimes, std::string_view appId);

}; // namespace xdgpp

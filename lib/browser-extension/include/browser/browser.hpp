#include <string_view>

namespace browser_extension {
static constexpr const auto nativeMessagingHost = NATIVE_MESSAGING_HOST;
bool chromeEntrypoint(std::string_view extensionId);
bool firefoxEntrypoint(std::string_view appId);
bool isNativeHostManifest(std::string_view path);

}; // namespace browser_extension

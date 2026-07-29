#pragma once

namespace Clipboard {

inline constexpr const char *CONCEALED_MIME_TYPE = "vicinae/concealed";
inline constexpr const char *PASSWORD_HINT_MIME_TYPE = "x-kde-passwordManagerHint";

// Qt wraps native Windows clipboard formats it has no mime mapping for
inline constexpr const char *WIN_NATIVE_MIME_PREFIX = "application/x-qt-windows-mime";
inline constexpr const char *WIN_EXCLUDE_FMT =
    R"(application/x-qt-windows-mime;value="ExcludeClipboardContentFromMonitorProcessing")";
inline constexpr const char *WIN_HISTORY_FMT =
    R"(application/x-qt-windows-mime;value="CanIncludeInClipboardHistory")";
inline constexpr const char *WIN_CLOUD_FMT =
    R"(application/x-qt-windows-mime;value="CanUploadToCloudClipboard")";
inline constexpr const char *WIN_LEGACY_IGNORE_FMT =
    R"(application/x-qt-windows-mime;value="Clipboard Viewer Ignore")";

} // namespace Clipboard

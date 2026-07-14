#pragma once
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>

namespace vicinae {

enum class FileCategory {
  Other,
  Directory,
  Image,
  Video,
  Audio,
  Document,
  Archive,
  Application,
};

inline bool extensionEquals(std::string_view ext, std::string_view candidate) {
  if (ext.size() != candidate.size()) return false;

  return std::ranges::equal(
      ext, candidate, [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
}

inline bool hasExtension(std::string_view ext, std::span<const std::string_view> extensions) {
  return std::ranges::any_of(extensions,
                             [&](std::string_view candidate) { return extensionEquals(ext, candidate); });
}

inline std::string normalizedExtension(const std::filesystem::path &path) {
  auto ext = path.extension().string();
  if (ext.starts_with('.')) { ext.erase(ext.begin()); }

  std::ranges::transform(ext, ext.begin(),
                         [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  return ext;
}

inline constexpr std::string_view IMAGE_EXTENSIONS[] = {"jpg", "jpeg", "png",  "gif", "webp", "avif",
                                                        "bmp", "tif",  "tiff", "svg", "heic", "ico"};
inline constexpr std::string_view VIDEO_EXTENSIONS[] = {"mp4", "m4v", "mkv",  "mov", "avi", "webm",
                                                        "wmv", "flv", "mpeg", "mpg", "3gp"};
inline constexpr std::string_view AUDIO_EXTENSIONS[] = {"mp3",  "flac", "wav",  "aac", "m4a", "ogg",
                                                        "opus", "wma",  "aiff", "mid", "midi"};
inline constexpr std::string_view DOCUMENT_EXTENSIONS[] = {
    "pdf", "txt",  "md",  "markdown", "doc", "docx", "odt", "rtf", "pages",
    "xls", "xlsx", "ods", "csv",      "ppt", "pptx", "odp", "epub"};
inline constexpr std::string_view ARCHIVE_EXTENSIONS[] = {"zip", "tar", "gz",  "tgz", "bz2", "xz",
                                                          "7z",  "rar", "zst", "lz4", "deb", "rpm"};
inline constexpr std::string_view APPLICATION_EXTENSIONS[] = {"desktop", "appimage", "exe",
                                                              "msi",     "app",      "dmg"};

inline constexpr std::span<const std::string_view> extensionsForCategory(FileCategory category) {
  switch (category) {
  case FileCategory::Image: return IMAGE_EXTENSIONS;
  case FileCategory::Video: return VIDEO_EXTENSIONS;
  case FileCategory::Audio: return AUDIO_EXTENSIONS;
  case FileCategory::Document: return DOCUMENT_EXTENSIONS;
  case FileCategory::Archive: return ARCHIVE_EXTENSIONS;
  case FileCategory::Application: return APPLICATION_EXTENSIONS;
  default: return {};
  }
}

inline FileCategory fileCategoryFor(const std::filesystem::path &path, bool isDirectory) {
  if (isDirectory) return FileCategory::Directory;

  auto ext = normalizedExtension(path);

  if (hasExtension(ext, IMAGE_EXTENSIONS)) return FileCategory::Image;
  if (hasExtension(ext, VIDEO_EXTENSIONS)) return FileCategory::Video;
  if (hasExtension(ext, AUDIO_EXTENSIONS)) return FileCategory::Audio;
  if (hasExtension(ext, DOCUMENT_EXTENSIONS)) return FileCategory::Document;
  if (hasExtension(ext, ARCHIVE_EXTENSIONS)) return FileCategory::Archive;
  if (hasExtension(ext, APPLICATION_EXTENSIONS)) return FileCategory::Application;

  return FileCategory::Other;
}

} // namespace vicinae

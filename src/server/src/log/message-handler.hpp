#pragma once
#include <iostream>
#include <qdatetime.h>
#include <qlogging.h>
#include <filesystem>

namespace vicinae::log {

inline constexpr std::string_view SERVER = "V";
inline constexpr std::string_view FILE_INDEXER = "F";
inline constexpr std::string_view EXTENSION = "E";
inline constexpr std::string_view INPUT_SERVER = "I";
inline constexpr std::string_view CLIPBOARD_SERVER = "C";

void installMessageHandler();

void openFile(const std::filesystem::path &path);

void subprocessLine(std::string_view source, QStringView level, QStringView message);

} // namespace vicinae::log

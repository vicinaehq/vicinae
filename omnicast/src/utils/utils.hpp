#pragma once
#include <filesystem>
#include <qdatetime.h>
#include <qstring.h>
#include <string_view>

/**
 * Attempts to compress the path as much as possible to make it better
 * fit in when not a lot of space is available.
 * Typically, if the path starts with $HOME, it is replaced by the '~' symbol.
 */
std::filesystem::path compressPath(const std::filesystem::path &);
std::filesystem::path homeDir();

QString getRelativeTimeString(const QDateTime &pastTime);

QString qStringFromStdView(std::string_view view);

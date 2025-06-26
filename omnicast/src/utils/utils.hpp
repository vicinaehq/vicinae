#pragma once
#include <filesystem>

/**
 * Attempts to compress the path as much as possible to make it better
 * fit in when not a lot of space is available.
 * Typically, if the path starts with $HOME, it is replaced by the '~' symbol.
 */
std::filesystem::path compressPath(const std::filesystem::path &);
std::filesystem::path homeDir();

#pragma once
#include <string>
#include <string_view>

std::string extractStaticCharsFromRegex(std::string_view regex, size_t minWordLength = 0);

#pragma once
#include <string_view>

constexpr char DEFAULT_CONFIG_DATA[] = {
#embed DEFAULT_CONFIG_PATH
};

constexpr std::string_view DEFAULT_CONFIG{DEFAULT_CONFIG_DATA, sizeof(DEFAULT_CONFIG_DATA)};

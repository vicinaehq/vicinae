#pragma once
#include "extend/metadata-model.hpp"
#include <optional>
#include <string>

struct DetailModel {
  std::optional<std::string> markdown;
  MetadataModel metadata;
};

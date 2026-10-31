#pragma once
#include "extension/model/metadata-model.hpp"
#include <optional>
#include <string>

struct DetailModel {
  std::optional<std::string> markdown;
  MetadataModel metadata;
};

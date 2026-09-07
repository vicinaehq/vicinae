#pragma once

#include "extension/model/action-model.hpp"
#include "extension/model/metadata-model.hpp"
#include <optional>
#include <string>

struct RootDetailModel {
  bool isLoading = false;
  std::string markdown;
  std::optional<MetadataModel> metadata;
  std::optional<ActionPannelModel> actions;
  std::optional<std::string> navigationTitle;
};

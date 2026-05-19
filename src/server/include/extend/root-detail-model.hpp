#pragma once

#include "extend/action-model.hpp"
#include "extend/metadata-model.hpp"
#include <optional>
#include <string>

struct RootDetailModel {
  bool isLoading = false;
  std::string markdown;
  std::optional<MetadataModel> metadata;
  std::optional<ActionPannelModel> actions;
  std::optional<std::string> navigationTitle;
};

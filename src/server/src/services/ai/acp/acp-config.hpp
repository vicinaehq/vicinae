#pragma once

#include <glaze/json.hpp>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <vector>
#include "services/ai/ai-reasoning.hpp"

namespace AI::acp {

struct ConfigChoice {
  std::string value;
  std::optional<std::string> name;
  std::optional<std::string> description;
};

struct ConfigEntry {
  std::optional<std::string> value;
  std::optional<std::string> name;
  std::optional<std::string> description;
  std::optional<std::vector<ConfigChoice>> options;
};

struct ConfigOption {
  std::string id;
  std::optional<std::string> category;
  std::optional<std::string> type;
  std::optional<glz::raw_json> currentValue;
  std::optional<std::vector<ConfigEntry>> options;
};

inline std::optional<ReasoningOptions> reasoningOptions(std::span<const ConfigOption> options) {
  const auto option = std::ranges::find_if(options, [](const auto &item) {
    return item.category == "thought_level" && item.type.value_or("select") == "select" && item.options;
  });
  if (option == options.end()) return std::nullopt;
  ReasoningOptions result{.id = option->id};
  result.choices.reserve(option->options->size());
  for (const auto &entry : *option->options) {
    if (entry.value) result.choices.emplace_back(*entry.value, entry.name.value_or(*entry.value));
    if (entry.options) {
      result.choices.reserve(result.choices.size() + entry.options->size());
      for (const auto &choice : *entry.options)
        result.choices.emplace_back(choice.value, choice.name.value_or(choice.value));
    }
  }
  if (result.choices.empty()) return std::nullopt;
  if (option->currentValue) {
    std::string current;
    if (!glz::read_json(current, option->currentValue->str) &&
        std::ranges::contains(result.choices, current, &ReasoningChoice::id))
      result.current = std::move(current);
  }
  return result;
}

} // namespace AI::acp

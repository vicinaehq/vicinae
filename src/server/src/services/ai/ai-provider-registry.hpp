#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include "ai-config.hpp"

namespace AI {

class AbstractProvider;

using ProviderFactory = std::function<std::shared_ptr<AbstractProvider>(const ConfigValue::ProviderConfig &)>;

class ProviderRegistry {
public:
  void add(std::string type, ProviderFactory factory) { m_factories[std::move(type)] = std::move(factory); }

  std::shared_ptr<AbstractProvider> create(const ConfigValue::ProviderConfig &config) const {
    auto type = std::visit([](const auto &cfg) { return cfg.type; }, config);
    auto it = m_factories.find(type);
    if (it == m_factories.end()) return nullptr;
    return it->second(config);
  }

private:
  std::unordered_map<std::string, ProviderFactory> m_factories;
};

ProviderRegistry makeBuiltinRegistry();

} // namespace AI

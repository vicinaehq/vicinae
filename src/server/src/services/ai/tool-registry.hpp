#pragma once

#include <vector>
#include <QObject>
#include "ai-tool.hpp"
#include "tool-contribution.hpp"

class RootItemManager;

namespace config {
class Manager;
}

namespace AI {

class ToolRegistry : public QObject {
  Q_OBJECT

signals:
  void changed();

public:
  struct Entry {
    std::string providerId;
    std::string name;
    ToolContribution contribution;
  };

  ToolRegistry(config::Manager &config, RootItemManager &rootItems);

  const std::vector<Entry> &entries() const { return m_entries; }
  const Entry *find(std::string_view name) const;
  const Entry *find(std::string_view providerId, std::string_view toolId) const;
  std::vector<std::unique_ptr<AbstractTool>> createTools() const;
  std::vector<std::string> enabledToolNames() const;

  bool providerEnabled(std::string_view providerId) const;
  ToolPolicy toolPolicy(std::string_view providerId, std::string_view toolId) const;
  ToolPolicy policy(std::string_view name) const;
  bool isEnabled(std::string_view name) const { return policy(name) != ToolPolicy::Disabled; }
  bool setToolPolicy(std::string_view providerId, std::string_view toolId, ToolPolicy policy);
  bool toolsEnabled() const;

private:
  void rebuild();

  config::Manager &m_config;
  RootItemManager &m_rootItems;
  std::vector<Entry> m_entries;
};

} // namespace AI

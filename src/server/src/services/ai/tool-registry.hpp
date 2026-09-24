#pragma once

#include <span>
#include <vector>
#include <QObject>
#include "ai-tool.hpp"
#include "tool-contribution.hpp"

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

  explicit ToolRegistry(config::Manager &config);

  void addProvider(std::string_view providerId, std::span<const ToolContribution> tools);
  const std::vector<Entry> &entries() const { return m_entries; }
  const Entry *find(std::string_view name) const;
  std::vector<std::unique_ptr<AbstractTool>> createTools() const;
  std::vector<std::string> enabledToolNames() const;

  bool providerEnabled(std::string_view providerId) const;
  bool toolEnabled(std::string_view providerId, std::string_view toolId) const;
  bool isEnabled(std::string_view name) const;
  bool setToolEnabled(std::string_view providerId, std::string_view toolId, bool enabled);
  bool toolsEnabled() const;

private:
  config::Manager &m_config;
  std::vector<Entry> m_entries;
};

} // namespace AI

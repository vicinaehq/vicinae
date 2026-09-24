#include <algorithm>
#include <QPointer>
#include "tool-registry.hpp"
#include "ai-preferences.hpp"
#include "config/config.hpp"

namespace AI {
namespace {

class RegisteredTool : public AbstractTool {
  Q_DECLARE_TR_FUNCTIONS(RegisteredTool)

public:
  RegisteredTool(const ToolRegistry &registry, std::string name, std::unique_ptr<AbstractTool> tool)
      : m_registry(&registry), m_name(std::move(name)), m_tool(std::move(tool)) {}

  std::string name() const override { return m_name; }
  std::string description() const override { return m_tool->description(); }
  std::string generateInputSchema() const override { return m_tool->generateInputSchema(); }
  std::optional<std::string> invocationSummary(std::string_view arguments) const override {
    return m_tool->invocationSummary(arguments);
  }
  bool isEnabled() const override {
    return m_registry && m_registry->isEnabled(m_name) && m_tool->isEnabled();
  }
  RawToolTask runRaw(std::string_view arguments) override {
    if (!isEnabled())
      return {QtFuture::makeReadyValueFuture<RawToolResult>(
                  std::unexpected(tr("This tool is not enabled.").toStdString())),
              {}};
    return m_tool->runRaw(arguments);
  }

private:
  QPointer<const ToolRegistry> m_registry;
  std::string m_name;
  std::unique_ptr<AbstractTool> m_tool;
};

bool validId(std::string_view id) {
  return !id.empty() && id.find("__") == std::string_view::npos && std::ranges::all_of(id, [](char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-';
  });
}

} // namespace

ToolRegistry::ToolRegistry(config::Manager &config) : m_config(config) {
  connect(&config, &config::Manager::configChanged, this, &ToolRegistry::changed);
}

void ToolRegistry::addProvider(std::string_view providerId, std::span<const ToolContribution> tools) {
  m_entries.reserve(m_entries.size() + tools.size());
  for (const auto &tool : tools) {
    auto name = std::format("{}__{}", providerId, tool.id);
    if (!validId(providerId) || !validId(tool.id) || name.size() > 64 || !tool.create ||
        std::ranges::find(m_entries, name, &Entry::name) != m_entries.end()) {
      qWarning() << "Invalid or duplicate tool contribution:" << name;
      continue;
    }
    m_entries.emplace_back(Entry{std::string(providerId), std::move(name), tool});
  }
  emit changed();
}

bool ToolRegistry::toolsEnabled() const {
  const auto &providers = m_config.value().providers;
  const auto it = providers.find(std::string(EXTENSION_ID));
  if (it == providers.end()) return true;
  if (const auto &prefs = it->second.preferences) {
    const auto *value = preferences::find(*prefs, "enableTools");
    if (value && value->is_boolean()) return value->get_boolean();
  }
  return AiPreferences{}.enableTools;
}

bool ToolRegistry::providerEnabled(std::string_view providerId) const {
  const auto &providers = m_config.value().providers;
  const auto it = providers.find(std::string(providerId));
  return it == providers.end() || it->second.enabled.value_or(true);
}

bool ToolRegistry::toolEnabled(std::string_view providerId, std::string_view toolId) const {
  const auto &providers = m_config.value().providers;
  const auto provider = providers.find(std::string(providerId));
  if (provider == providers.end()) return true;
  const auto &tools = provider->second.tools;
  const auto tool = tools.find(std::string(toolId));
  return tool == tools.end() || tool->second.enabled.value_or(true);
}

const ToolRegistry::Entry *ToolRegistry::find(std::string_view name) const {
  const auto entry = std::ranges::find(m_entries, name, &Entry::name);
  return entry != m_entries.end() ? &*entry : nullptr;
}

bool ToolRegistry::isEnabled(std::string_view name) const {
  const auto *entry = find(name);
  return entry && toolsEnabled() && providerEnabled(EXTENSION_ID) && providerEnabled(entry->providerId) &&
         toolEnabled(entry->providerId, entry->contribution.id);
}

bool ToolRegistry::setToolEnabled(std::string_view providerId, std::string_view toolId, bool enabled) {
  if (std::ranges::none_of(m_entries, [&](const Entry &entry) {
        return entry.providerId == providerId && entry.contribution.id == toolId;
      }))
    return false;
  return m_config.mergeProviderWithUser(providerId, {.tools = std::map<std::string, config::ProviderToolData>{
                                                         {std::string(toolId), {.enabled = enabled}}}});
}

std::vector<std::unique_ptr<AbstractTool>> ToolRegistry::createTools() const {
  std::vector<std::unique_ptr<AbstractTool>> tools;
  tools.reserve(m_entries.size());
  for (const auto &entry : m_entries) {
    if (auto tool = entry.contribution.create())
      tools.emplace_back(std::make_unique<RegisteredTool>(*this, entry.name, std::move(tool)));
  }
  return tools;
}

std::vector<std::string> ToolRegistry::enabledToolNames() const {
  std::vector<std::string> names;
  names.reserve(m_entries.size());
  for (const auto &entry : m_entries) {
    if (isEnabled(entry.name)) names.emplace_back(entry.name);
  }
  return names;
}

} // namespace AI

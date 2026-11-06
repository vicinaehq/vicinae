#include <algorithm>
#include <format>
#include <QPointer>

#include "tool-registry.hpp"
#include "ai-preferences.hpp"
#include "config/config.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

namespace AI {

namespace {

class RegisteredTool : public AbstractTool {
  Q_DECLARE_TR_FUNCTIONS(RegisteredTool)

public:
  RegisteredTool(const ToolRegistry &registry, std::string name, bool deferred,
                 std::unique_ptr<AbstractTool> tool)
      : m_registry(&registry), m_name(std::move(name)), m_deferred(deferred), m_tool(std::move(tool)) {}

  std::string name() const override { return m_name; }
  std::string description() const override { return m_tool->description(); }
  std::string generateInputSchema() const override { return m_tool->generateInputSchema(); }
  bool isDeferred() const override { return m_deferred; }

  std::optional<std::string> invocationSummary(std::string_view arguments) const override {
    return m_tool->invocationSummary(arguments);
  }

  bool isEnabled() const override {
    return m_registry && m_registry->isEnabled(m_name) && m_tool->isEnabled();
  }

  bool requiresApproval(std::string_view arguments) const override {
    return !m_registry || m_registry->policy(m_name) != ToolPolicy::Allow ||
           m_tool->requiresApproval(arguments);
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
  bool m_deferred;
  std::unique_ptr<AbstractTool> m_tool;
};

bool validId(std::string_view id) {
  return !id.empty() && id.find("__") == std::string_view::npos && std::ranges::all_of(id, [](char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-';
  });
}

} // namespace

ToolRegistry::ToolRegistry(config::Manager &config, RootItemManager &rootItems)
    : m_config(config), m_rootItems(rootItems) {
  connect(&config, &config::Manager::configChanged, this, &ToolRegistry::changed);
  connect(&rootItems, &RootItemManager::itemsChanged, this, &ToolRegistry::rebuild);
  rebuild();
}

void ToolRegistry::rebuild() {
  std::vector<Entry> entries;
  std::vector<std::string> invalid;

  for (const auto *provider : m_rootItems.providers()) {
    const auto providerId = provider->uniqueId().toStdString();
    auto tools = provider->tools();
    entries.reserve(entries.size() + tools.size());

    for (auto &tool : tools) {
      auto name = std::format("{}__{}", providerId, tool.id);

      if (!validId(providerId) || !validId(tool.id) || name.size() > 64 || !tool.create ||
          std::ranges::find(entries, name, &Entry::name) != entries.end()) {
        invalid.emplace_back(std::move(name));
        continue;
      }

      entries.emplace_back(Entry{providerId, std::move(name), std::move(tool)});
    }
  }

  const bool same = std::ranges::equal(entries, m_entries, {}, &Entry::name, &Entry::name);
  m_entries = std::move(entries);
  if (same) return;

  for (const auto &name : invalid)
    qWarning() << "Invalid or duplicate tool contribution:" << name;
  emit changed();
}

bool ToolRegistry::toolsEnabled() const {
  const auto *value = findPreference(m_config, "enableTools");
  return value && value->is_boolean() ? value->get_boolean() : AiPreferences{}.enableTools;
}

bool ToolRegistry::providerEnabled(std::string_view providerId) const {
  const auto &providers = m_config.value().providers;
  const auto it = providers.find(std::string(providerId));
  return it == providers.end() || it->second.enabled.value_or(true);
}

ToolPolicy ToolRegistry::toolPolicy(std::string_view providerId, std::string_view toolId) const {
  const auto *entry = find(providerId, toolId);
  if (!entry) return ToolPolicy::Disabled;

  const auto &providers = m_config.value().providers;
  const auto provider = providers.find(std::string(providerId));
  if (provider == providers.end()) return entry->contribution.defaultPolicy;

  const auto &tools = provider->second.tools;
  const auto tool = tools.find(std::string(toolId));
  if (tool == tools.end()) return entry->contribution.defaultPolicy;
  return tool->second.policy.value_or(entry->contribution.defaultPolicy);
}

const ToolRegistry::Entry *ToolRegistry::find(std::string_view name) const {
  const auto entry = std::ranges::find(m_entries, name, &Entry::name);
  return entry != m_entries.end() ? &*entry : nullptr;
}

const ToolRegistry::Entry *ToolRegistry::find(std::string_view providerId, std::string_view toolId) const {
  const auto entry = std::ranges::find_if(m_entries, [&](const Entry &entry) {
    return entry.providerId == providerId && entry.contribution.id == toolId;
  });
  return entry != m_entries.end() ? &*entry : nullptr;
}

ToolPolicy ToolRegistry::policy(std::string_view name) const {
  const auto *entry = find(name);
  if (!entry || !toolsEnabled() || !providerEnabled(EXTENSION_ID) || !providerEnabled(entry->providerId))
    return ToolPolicy::Disabled;
  return toolPolicy(entry->providerId, entry->contribution.id);
}

bool ToolRegistry::setToolPolicy(std::string_view providerId, std::string_view toolId, ToolPolicy policy) {
  if (!find(providerId, toolId)) return false;

  return m_config.mergeProviderWithUser(providerId, {.tools = std::map<std::string, config::ProviderToolData>{
                                                         {std::string(toolId), {.policy = policy}}}});
}

std::vector<std::unique_ptr<AbstractTool>> ToolRegistry::createTools() const {
  std::vector<std::unique_ptr<AbstractTool>> tools;
  tools.reserve(m_entries.size());

  for (const auto &entry : m_entries) {
    if (auto tool = entry.contribution.create())
      tools.emplace_back(
          std::make_unique<RegisteredTool>(*this, entry.name, entry.contribution.deferred, std::move(tool)));
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

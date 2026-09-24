#include <algorithm>
#include <map>
#include "tool-settings-model.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-preferences.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/views/view-utils.hpp"

ToolSettingsModel::ToolSettingsModel(QObject *parent)
    : QObject(parent), m_registry(*ServiceRegistry::instance()->tools()),
      m_rootItems(*ServiceRegistry::instance()->rootItemManager()) {
  connect(&m_registry, &AI::ToolRegistry::changed, this, [this] {
    rebuild();
    emit stateChanged();
  });
  connect(&m_rootItems, &RootItemManager::itemsChanged, this, &ToolSettingsModel::rebuild);
  rebuild();
}

bool ToolSettingsModel::enabled() const { return m_registry.toolsEnabled(); }
bool ToolSettingsModel::aiEnabled() const { return m_registry.providerEnabled(AI::EXTENSION_ID); }

void ToolSettingsModel::setToolEnabled(const QString &providerId, const QString &toolId, bool enabled) {
  m_registry.setToolEnabled(providerId.toStdString(), toolId.toStdString(), enabled);
}

void ToolSettingsModel::rebuild() {
  struct Group {
    QString name;
    QString iconSource;
    bool enabled;
    QVariantList tools;
  };
  std::vector<Group> groups;
  groups.reserve(m_registry.entries().size());
  std::map<std::string, std::size_t> indices;

  for (const auto &entry : m_registry.entries()) {
    auto *provider = m_rootItems.provider(entry.providerId);
    const auto name = provider ? provider->displayName() : QString::fromStdString(entry.providerId);
    auto [index, inserted] = indices.try_emplace(entry.providerId, groups.size());
    if (inserted) {
      groups.emplace_back(Group{.name = name,
                                .iconSource = provider ? qml::imageSourceFor(provider->icon()) : QString{},
                                .enabled = m_registry.providerEnabled(entry.providerId)});
    }
    auto &group = groups[index->second];
    const auto &tool = entry.contribution;
    group.tools.emplace_back(
        QVariantMap{{QStringLiteral("id"), QString::fromStdString(tool.id)},
                    {QStringLiteral("providerId"), QString::fromStdString(entry.providerId)},
                    {QStringLiteral("name"), tool.title},
                    {QStringLiteral("description"), tool.description},
                    {QStringLiteral("iconSource"), qml::imageSourceFor(tool.icon)},
                    {QStringLiteral("isEnabled"), m_registry.toolEnabled(entry.providerId, tool.id)},
                    {QStringLiteral("providerEnabled"), group.enabled}});
  }
  std::ranges::sort(
      groups, [](const Group &a, const Group &b) { return QString::localeAwareCompare(a.name, b.name) < 0; });
  QVariantList rows;
  rows.reserve(groups.size());
  for (auto &group : groups) {
    std::ranges::sort(group.tools, [](const QVariant &a, const QVariant &b) {
      return QString::localeAwareCompare(a.toMap().value(QStringLiteral("name")).toString(),
                                         b.toMap().value(QStringLiteral("name")).toString()) < 0;
    });
    rows.emplace_back(QVariantMap{{QStringLiteral("name"), group.name},
                                  {QStringLiteral("iconSource"), group.iconSource},
                                  {QStringLiteral("isEnabled"), group.enabled},
                                  {QStringLiteral("tools"), std::move(group.tools)}});
  }
  if (rows == m_groups) return;
  m_groups = std::move(rows);
  emit groupsChanged();
}

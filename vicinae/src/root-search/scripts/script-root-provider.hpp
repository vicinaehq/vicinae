#include "navigation-controller.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/script-command/script-command-service.hpp"
#include "utils.hpp"
#include <ranges>

class ScriptRootItem : public RootItem {

  QString displayName() const override { return m_file.data().title.c_str(); }

  QString subtitle() const override { return "Subtitle"; }

  QString typeDisplayName() const override { return "Script"; }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override {
    return nullptr;
  }

  AccessoryList accessories() const override { return {{.text = "Script"}}; }

  EntrypointId uniqueId() const override { return EntrypointId("scripts", m_file.path()); };

  ImageURL iconUrl() const override { return m_file.icon(); }

  std::vector<QString> keywords() const override { return Utils::toQStringVec(m_file.data().keywords); }

public:
  ScriptRootItem(ScriptCommandFile &&file) : m_file(std::move(file)) {}

private:
  ScriptCommandFile m_file;
};

class ScriptRootProvider : public RootProvider {
public:
  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_service.scanAll() | std::views::transform([](auto item) -> std::shared_ptr<RootItem> {
             return std::make_shared<ScriptRootItem>(std::move(item));
           }) |
           std::ranges::to<std::vector>();
  }

  Type type() const override { return Type::GroupProvider; }

  ImageURL icon() const override { return ImageURL::emoji("🤖"); }

  QString displayName() const override { return "Script Commands"; }

  QString uniqueId() const override { return "scripts"; }

  PreferenceList preferences() const override { return {}; }

  void preferencesChanged(const QJsonObject &preferences) override {}

  ScriptRootProvider() = default;

private:
  ScriptCommandService m_service;
};

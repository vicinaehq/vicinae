#pragma once
#include <QCoreApplication>
#include <QString>
#include "fuzzy/fuzzy-searchable.hpp"
#include "services/ai-commands/ai-command-store.hpp"
#include "ui/views/mono-list-view-host.hpp"

template <> struct fuzzy::FuzzySearchable<AiCommandStore::Command> {
  static fuzzy::Match score(const AiCommandStore::Command &command, const fuzzy::Query &query) {
    return fuzzy::scoreWeighted({{command.name, 1.0}}, query);
  }
};

class AiCommandsViewHost : public MonoListViewHost<AiCommandStore::Command> {
  Q_DECLARE_TR_FUNCTIONS(AiCommandsViewHost)

public:
  void onMount() override;

  QString sectionName() const override { return tr("AI Commands ({count})"); }
  QString displayId(const ItemType &command) const override { return QString::fromStdString(command.id); }
  QString displayTitle(const ItemType &command) const override {
    return QString::fromStdString(command.name);
  }
  QString displaySubtitle(const ItemType &) const override { return {}; }
  std::optional<ImageURL> displayIcon(const ItemType &command) const override;
  AccessoryList displayAccessories(const ItemType &command) const override;
  bool hasDetailPane() const override { return true; }
  std::optional<ListItemDetail> displayDetail(const ItemType &command) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &command) const override;

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  std::optional<QString> modelName(const ItemType &command) const;
  void reload();
};

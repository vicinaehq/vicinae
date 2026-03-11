#include "manage-models-model.hpp"
#include "view-utils.hpp"

QString ManageModelsModel::displayTitle(const AI::ProviderModel &item) const {
  return QString::fromStdString(item.name);
}

QString ManageModelsModel::displaySubtitle(const AI::ProviderModel &) const { return {}; }

QString ManageModelsModel::displayIconSource(const AI::ProviderModel &item) const {
  if (item.icon) { return item.icon->toSource(); }
  return {};
}

QVariantList ManageModelsModel::displayAccessory(const AI::ProviderModel &item) const {
  return qml::textAccessory(QString::fromStdString(item.ref.provider));
}

std::unique_ptr<ActionPanelState> ManageModelsModel::buildActionPanel(const AI::ProviderModel &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->setTitle(QString::fromStdString(item.name));
  return panel;
}

void ManageModelsModel::itemSelected(const AI::ProviderModel &item) { emit modelSelected(item); }

QString ManageModelsModel::sectionLabel() const { return QStringLiteral("Models ({count})"); }

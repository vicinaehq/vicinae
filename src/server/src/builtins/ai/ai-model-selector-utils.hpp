#pragma once
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <QVariantList>
#include <QVariantMap>
#include <map>

inline QVariantList buildGroupedModelList(AI::Service *service, std::optional<AI::Capabilities> caps) {
  auto models = service->listModels(caps);

  std::map<std::string, QVariantList> groups;

  for (const auto &model : models) {
    QVariantMap item;
    item[QStringLiteral("id")] = QString::fromStdString(model.ref.toString());
    item[QStringLiteral("displayName")] = QString::fromStdString(model.name);
    if (model.icon) { item[QStringLiteral("iconSource")] = model.icon->toSource(); }
    groups[model.ref.provider].append(item);
  }

  QVariantList result;
  result.reserve(groups.size());

  for (auto &[providerId, items] : groups) {
    QVariantMap section;
    auto title = QString::fromStdString(providerId);
    title[0] = title[0].toUpper();
    section[QStringLiteral("title")] = title;
    section[QStringLiteral("items")] = std::move(items);
    result.append(section);
  }

  return result;
}

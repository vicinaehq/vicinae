#pragma once
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include <QVariantList>
#include <QVariantMap>
#include <map>
#include <vector>
#include "command/preference.hpp"

inline std::optional<ImageUrl> modelIcon(AI::Service *service, const AI::ProviderModel &model) {
  if (model.icon) return model.icon;
  if (auto *provider = service->getProviderById(model.ref.provider)) return provider->icon();
  return std::nullopt;
}

inline QString providerDisplayName(AI::Service *service, std::string_view providerId) {
  if (auto *provider = service->getProviderById(providerId)) {
    return QString::fromStdString(provider->displayName());
  }
  auto title = QString::fromUtf8(providerId.data(), static_cast<qsizetype>(providerId.size()));
  if (!title.isEmpty()) title[0] = title[0].toUpper();
  return title;
}

inline QVariantList buildGroupedModelList(AI::Service *service, std::optional<AI::Capabilities> caps) {
  auto models = service->listModels(caps);

  std::map<std::string, QVariantList> groups;

  for (const auto &model : models) {
    QVariantMap item;
    item[QStringLiteral("id")] = QString::fromStdString(model.ref.toString());
    item[QStringLiteral("displayName")] = QString::fromStdString(model.name);
    if (auto icon = modelIcon(service, model)) {
      item[QStringLiteral("iconSource")] = icon->imageUrl().toString();
    }
    groups[model.ref.provider].append(item);
  }

  QVariantList result;
  result.reserve(groups.size());

  for (auto &[providerId, items] : groups) {
    QVariantMap section;
    section[QStringLiteral("title")] = providerDisplayName(service, providerId);
    section[QStringLiteral("items")] = std::move(items);
    result.append(section);
  }

  return result;
}

inline std::vector<Preference::DropdownData::Section>
buildModelDropdownSections(AI::Service *service, std::optional<AI::Capabilities> caps) {
  std::map<std::string, std::vector<Preference::DropdownData::Option>> groups;

  for (const auto &model : service->listModels(caps)) {
    groups[model.ref.provider].emplace_back(Preference::DropdownData::Option{
        .title = QString::fromStdString(model.name),
        .value = QString::fromStdString(model.ref.toString()),
        .icon = modelIcon(service, model).transform([](const ImageUrl &icon) { return icon.imageUrl(); }),
    });
  }

  std::vector<Preference::DropdownData::Section> sections;
  sections.reserve(groups.size());
  for (auto &[providerId, options] : groups) {
    sections.emplace_back(Preference::DropdownData::Section{
        .title = providerDisplayName(service, providerId),
        .options = std::move(options),
    });
  }

  return sections;
}

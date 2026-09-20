#include "ai-settings-model.hpp"
#include <algorithm>
#include <iterator>
#include <format>
#include <optional>
#include <ranges>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qcoreapplication.h>
#include <qlogging.h>
#include "command/preference.hpp"
#include "config/config.hpp"
#include "internal/glaze-qt.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-provider-types.hpp"
#include "services/ai/ai-provider.hpp"
#include "services/ai/ai-service.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "ui/image/image-url.hpp"
#include "ui/views/view-utils.hpp"
#include "utils/utils.hpp"

namespace {

QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

QStringList capabilityNames(AI::Capabilities caps) {
  QStringList names;
  for (const auto &name : AI::stringifyCapabilities(caps)) {
    auto text = QString::fromStdString(name);
    if (!text.isEmpty()) text[0] = text[0].toUpper();
    names << text;
  }
  return names;
}

QString joinMeta(const QStringList &parts) { return parts.join(QStringLiteral(" · ")); }

QVariant iconVariant(const std::optional<ImageUrl> &icon) {
  return icon ? QVariant::fromValue(*icon) : QVariant();
}

QVariantMap instanceEntry(const QString &id, const QString &name, const QString &status) {
  return {{QStringLiteral("id"), id}, {QStringLiteral("name"), name}, {QStringLiteral("statusText"), status}};
}

bool isToggle(const Preference &pref) {
  return std::holds_alternative<Preference::CheckboxData>(pref.data());
}

std::optional<Preference::DropdownData> dropdownOf(const Preference &pref) {
  const auto data = pref.data();
  if (const auto *dropdown = std::get_if<Preference::DropdownData>(&data)) return *dropdown;
  return std::nullopt;
}

QString kindName(const Preference &pref) {
  if (pref.isSecret()) return QStringLiteral("secret");
  if (isToggle(pref)) return QStringLiteral("toggle");
  if (dropdownOf(pref)) return QStringLiteral("select");
  return QStringLiteral("text");
}

glz::generic toStoredValue(const Preference &pref, const QVariant &value) {
  if (isToggle(pref)) return value.toBool();
  return value.toString().toStdString();
}

bool isBuiltin(const AI::ProviderTypeInfo &info) { return info.cardinality == AI::Cardinality::Builtin; }

QVariantMap optionEntry(const Preference::DropdownData::Option &option) {
  return qml::makeDropdownItem(option.value, option.title);
}

QVariantList optionSections(const Preference &pref) {
  const auto dropdown = dropdownOf(pref);
  if (!dropdown) return {};
  QVariantList items;
  for (const auto &option : dropdown->options) {
    items.append(optionEntry(option));
  }
  return {QVariantMap{{QStringLiteral("items"), items}}};
}

QVariant optionItem(const Preference &pref, const QString &value) {
  const auto dropdown = dropdownOf(pref);
  if (!dropdown) return {};
  auto it = std::ranges::find(dropdown->options, value, &Preference::DropdownData::Option::value);
  return it == dropdown->options.end() ? QVariant{} : QVariant(optionEntry(*it));
}

const Preference *findField(const std::vector<Preference> &fields, const QString &key) {
  auto it = std::ranges::find(fields, key, &Preference::name);
  return it == fields.end() ? nullptr : &*it;
}

} // namespace

// ── list models ──

QVariant AIProviderTypesModel::data(const QModelIndex &index, int role) const {
  const auto *row = rowAt(index);
  if (!row) return {};
  switch (role) {
  case TypeRole:
    return row->key;
  case LabelRole:
    return row->label;
  case DescriptionRole:
    return row->description;
  case IconRole:
    return row->icon;
  case BuiltinRole:
    return row->builtin;
  case AllowMultipleRole:
    return row->allowMultiple;
  case CanAddRole:
    return row->canAdd;
  case InstancesRole:
    return row->instances;
  default:
    return {};
  }
}

QHash<int, QByteArray> AIProviderTypesModel::roleNames() const {
  return {
      {TypeRole, "type"},     {LabelRole, "label"},         {DescriptionRole, "description"},
      {IconRole, "icon"},     {BuiltinRole, "builtin"},     {AllowMultipleRole, "allowMultiple"},
      {CanAddRole, "canAdd"}, {InstancesRole, "instances"},
  };
}

QVariant AIProviderFieldsModel::data(const QModelIndex &index, int role) const {
  const auto *row = rowAt(index);
  if (!row) return {};
  switch (role) {
  case KeyRole:
    return row->key;
  case LabelRole:
    return row->label;
  case DescriptionRole:
    return row->description;
  case PlaceholderRole:
    return row->placeholder;
  case KindRole:
    return row->kind;
  case OptionsRole:
    return row->options;
  case ValueRole:
    return row->value;
  case CurrentOptionRole:
    return row->currentOption;
  default:
    return {};
  }
}

QHash<int, QByteArray> AIProviderFieldsModel::roleNames() const {
  return {
      {KeyRole, "key"},
      {LabelRole, "label"},
      {DescriptionRole, "description"},
      {PlaceholderRole, "placeholder"},
      {KindRole, "kind"},
      {OptionsRole, "options"},
      {ValueRole, "value"},
      {CurrentOptionRole, "currentOption"},
  };
}

QVariant AIProviderModelsModel::data(const QModelIndex &index, int role) const {
  const auto *row = rowAt(index);
  if (!row) return {};
  switch (role) {
  case IdRole:
    return row->key;
  case NameRole:
    return row->name;
  case DescriptionRole:
    return row->description;
  case MetaRole:
    return row->meta;
  case IconRole:
    return row->icon;
  case StatusRole:
    return row->status;
  case ProgressRole:
    return row->progress;
  default:
    return {};
  }
}

QHash<int, QByteArray> AIProviderModelsModel::roleNames() const {
  return {
      {IdRole, "modelId"}, {NameRole, "name"},     {DescriptionRole, "description"}, {MetaRole, "meta"},
      {IconRole, "icon"},  {StatusRole, "status"}, {ProgressRole, "progress"},
  };
}

// ── provider page ──

AIProviderPage::AIProviderPage(AISettingsModel &owner) : QObject(&owner), m_owner(owner) {}

void AIProviderPage::setField(const QString &key, const QVariant &value) {
  if (m_valid) m_owner.setField(m_providerId.toStdString(), key, value);
}

void AIProviderPage::download(const QString &modelId) {
  auto *provider =
      m_owner.m_aiService ? m_owner.m_aiService->getProviderById(m_providerId.toStdString()) : nullptr;
  if (!provider) return;
  if (auto result = provider->downloadModel(modelId.toStdString()); !result) {
    qWarning() << "Could not start model download:" << result.error();
  }
}

void AIProviderPage::cancelDownload(const QString &modelId) {
  auto *provider =
      m_owner.m_aiService ? m_owner.m_aiService->getProviderById(m_providerId.toStdString()) : nullptr;
  if (provider) provider->cancelDownload(modelId.toStdString());
}

void AIProviderPage::removeModel(const QString &modelId) {
  auto *provider =
      m_owner.m_aiService ? m_owner.m_aiService->getProviderById(m_providerId.toStdString()) : nullptr;
  if (!provider) return;
  if (auto result = provider->removeModel(modelId.toStdString()); !result) {
    qWarning() << "Could not remove model:" << result.error();
  }
}

void AIProviderPage::remove() {
  if (m_valid && !m_builtin) m_owner.removeProvider(m_providerId.toStdString());
}

// ── settings model ──

AISettingsModel::AISettingsModel(QObject *parent) : QObject(parent) {
  auto *registry = ServiceRegistry::instance();
  m_aiService = registry->ai();
  m_rootItems = registry->rootItemManager();
  m_storage = registry->localStorage();

  if (auto *config = registry->config()) {
    connect(config, &config::Manager::configChanged, this, [this]() {
      rebuildTypes();
      rebuildPage();
    });
  }
  if (m_aiService) {
    connect(m_aiService, &AI::Service::modelsChanged, this, [this]() {
      rebuildTypes();
      rebuildPage();
    });
    connect(m_aiService, &AI::Service::managedModelsChanged, this, [this]() { rebuildPageModels(); });
  }

  rebuildTypes();
  rebuildPage();
}

void AISettingsModel::setSelectedProviderId(const QString &id) {
  if (m_selectedProviderId == id) return;
  m_selectedProviderId = id;
  rebuildPage();
  emit selectedProviderIdChanged();
}

std::map<std::string, AI::ProviderInstance> AISettingsModel::providers() const {
  if (!m_rootItems) return {};
  return readPreferences<AiPreferences>(m_rootItems->getProviderPreferenceValues(qs(AI::EXTENSION_ID)))
      .providers;
}

void AISettingsModel::saveProviders(const std::map<std::string, AI::ProviderInstance> &providers) {
  if (!m_rootItems) return;
  std::string json;
  glz::generic value;
  if (glz::write_json(providers, json) || glz::read_json(value, json)) {
    qWarning() << "Could not serialize AI providers";
    return;
  }
  auto values = m_rootItems->getProviderPreferenceValues(qs(AI::EXTENSION_ID));
  values[std::string(AI::PROVIDERS_PREFERENCE)] = std::move(value);
  m_rootItems->setProviderPreferenceValues(qs(AI::EXTENSION_ID), values);
}

const AI::ProviderTypeInfo *AISettingsModel::typeInfoFor(const std::string &id) const {
  const auto configured = providers();
  if (auto it = configured.find(id); it != configured.end()) return AI::findProviderType(it->second.type);
  const auto *info = AI::findProviderType(id);
  return info && isBuiltin(*info) ? info : nullptr;
}

bool AISettingsModel::canAddType(const AI::ProviderTypeInfo &info) const {
  switch (info.cardinality) {
  case AI::Cardinality::Builtin:
    return false;
  case AI::Cardinality::Multiple:
    return true;
  case AI::Cardinality::Single:
    break;
  }
  return std::ranges::none_of(providers(),
                              [type = info.type](const auto &entry) { return entry.second.type == type; });
}

QString AISettingsModel::statusText(AI::AbstractProvider &provider) const {
  if (provider.managesModels()) {
    const auto models = provider.managedModels();
    const auto installed =
        std::ranges::count(models, AI::ManagedModel::State::Installed, &AI::ManagedModel::state);
    if (installed == 0) return tr("No models installed");
    return tr("%n model(s) installed", nullptr, static_cast<int>(installed));
  }
  const auto count = provider.listModels().size();
  if (count == 0) return tr("No models found");
  return tr("%n model(s)", nullptr, static_cast<int>(count));
}

std::vector<AIProviderModelRow> AISettingsModel::modelRows(AI::AbstractProvider &provider) const {
  std::vector<AIProviderModelRow> rows;

  if (provider.managesModels()) {
    const auto models = provider.managedModels();
    rows.reserve(models.size());
    for (const auto &model : models) {
      QStringList meta = capabilityNames(model.caps);
      if (model.size > 0) meta << formatSize(model.size);
      if (!model.precision.empty()) meta << QString::fromStdString(model.precision);
      if (!model.languages.empty()) meta << QString::fromStdString(model.languages);

      QString status;
      switch (model.state) {
      case AI::ManagedModel::State::Absent:
        status = QStringLiteral("absent");
        break;
      case AI::ManagedModel::State::Downloading:
        status = QStringLiteral("downloading");
        break;
      case AI::ManagedModel::State::Installed:
        status = QStringLiteral("installed");
        break;
      }

      rows.emplace_back(AIProviderModelRow{
          .key = QString::fromStdString(model.id),
          .name = QString::fromStdString(model.name),
          .description = QString::fromStdString(model.description),
          .meta = joinMeta(meta),
          .icon = iconVariant(model.icon),
          .status = status,
          .progress = model.progress,
      });
    }
    return rows;
  }

  const auto providerIcon = provider.icon();
  auto models = provider.listModels();
  rows.reserve(models.size());
  for (auto &model : models) {
    rows.emplace_back(AIProviderModelRow{
        .key = QString::fromStdString(model.id),
        .name = QString::fromStdString(model.name),
        .description = model.description ? QString::fromStdString(*model.description) : QString(),
        .meta = joinMeta(capabilityNames(model.caps)),
        .icon = iconVariant(model.icon ? model.icon : providerIcon),
        .status = QStringLiteral("available"),
    });
  }
  return rows;
}

std::vector<AIProviderFieldRow>
AISettingsModel::fieldRows(const std::string &id, const AI::ProviderTypeInfo &info, bool withValues) const {
  std::vector<AIProviderFieldRow> rows;
  PreferenceValues stored;
  if (withValues) {
    const auto configured = providers();
    if (auto it = configured.find(id); it != configured.end()) stored = it->second.fields;
  }
  const auto scope = AI::Service::secretScope(id);
  const auto fields = info.fields();

  rows.reserve(fields.size());
  for (const auto &pref : fields) {
    AIProviderFieldRow row{
        .key = pref.name(),
        .label = pref.title(),
        .description = pref.description(),
        .placeholder = pref.placeholder(),
        .kind = kindName(pref),
        .options = optionSections(pref),
        .value = glazeToQVariant(pref.defaultOrNull()),
    };
    if (withValues) {
      if (pref.isSecret()) {
        if (m_storage) {
          const auto secret = m_storage->getItem(scope, pref.name());
          if (!secret.isUndefined() && !secret.isNull()) row.value = secret.toVariant();
        }
      } else if (const auto *value = preferences::find(stored, pref.name().toStdString())) {
        row.value = glazeToQVariant(*value);
      }
    }
    if (dropdownOf(pref)) row.currentOption = optionItem(pref, row.value.toString());
    rows.emplace_back(std::move(row));
  }
  return rows;
}

void AISettingsModel::rebuildTypes() {
  std::vector<AIProviderTypeRow> rows;
  rows.reserve(AI::PROVIDER_TYPES.size());
  const auto configured = providers();

  for (const auto &info : AI::PROVIDER_TYPES) {
    if (isBuiltin(info)) {
      auto *provider = m_aiService ? m_aiService->getProviderById(info.type) : nullptr;
      if (!provider) continue;
      const auto id = qs(info.type);
      const auto name = QString::fromStdString(provider->displayName());
      rows.emplace_back(AIProviderTypeRow{
          .key = id,
          .label = name,
          .description = qs(provider->description()),
          .icon = iconVariant(provider->icon()),
          .builtin = true,
          .instances = {instanceEntry(id, name, statusText(*provider))},
      });
      continue;
    }

    QVariantList instances;
    for (const auto &[id, instance] : configured) {
      if (instance.type != info.type) continue;
      auto *provider = m_aiService ? m_aiService->getProviderById(id) : nullptr;
      const auto qid = QString::fromStdString(id);
      instances.append(instanceEntry(qid, qid, provider ? statusText(*provider) : tr("Not connected")));
    }
    rows.emplace_back(AIProviderTypeRow{
        .key = qs(info.type),
        .label = qs(info.label),
        .description = qs(info.description),
        .icon = QVariant::fromValue(ImageUrl(ImageURL::builtin(info.icon))),
        .allowMultiple = info.cardinality == AI::Cardinality::Multiple,
        .canAdd = canAddType(info),
        .instances = std::move(instances),
    });
  }

  auto available =
      std::ranges::stable_partition(rows, [](const auto &row) { return !row.instances.isEmpty(); });
  std::vector<AIProviderTypeRow> availableRows(std::make_move_iterator(available.begin()),
                                               std::make_move_iterator(available.end()));
  rows.erase(available.begin(), available.end());

  m_configuredTypes.setRows(std::move(rows));
  m_availableTypes.setRows(std::move(availableRows));
}

void AISettingsModel::rebuildPage() {
  auto &page = m_page;
  const auto id = m_selectedProviderId.toStdString();
  auto *provider = m_aiService && !id.empty() ? m_aiService->getProviderById(id) : nullptr;
  const auto *info = id.empty() ? nullptr : typeInfoFor(id);

  page.m_valid = false;
  page.m_providerId = m_selectedProviderId;
  page.m_builtin = info && isBuiltin(*info);

  if (page.m_builtin && provider) {
    page.m_valid = true;
    page.m_name = QString::fromStdString(provider->displayName());
    page.m_typeLabel = page.m_name;
    page.m_description = qs(provider->description());
    page.m_icon = iconVariant(provider->icon());
    page.m_statusText = statusText(*provider);
    page.m_fields.setRows(fieldRows(id, *info, true));
  } else if (info && !page.m_builtin) {
    page.m_valid = true;
    page.m_name = m_selectedProviderId;
    page.m_typeLabel = qs(info->label);
    page.m_description = provider ? qs(provider->description()) : qs(info->description);
    page.m_icon = QVariant::fromValue(ImageUrl(ImageURL::builtin(info->icon)));
    page.m_statusText = provider ? statusText(*provider) : tr("Not connected");
    page.m_fields.setRows(fieldRows(id, *info, true));
  } else {
    page.m_name.clear();
    page.m_typeLabel.clear();
    page.m_description.clear();
    page.m_icon = QVariant();
    page.m_statusText.clear();
    page.m_fields.setRows({});
  }

  rebuildPageModels();
  emit page.changed();
}

void AISettingsModel::rebuildPageModels() {
  const auto id = m_selectedProviderId.toStdString();
  auto *provider = m_aiService && !id.empty() ? m_aiService->getProviderById(id) : nullptr;
  m_page.m_models.setRows(provider ? modelRows(*provider) : std::vector<AIProviderModelRow>{});
}

QStringList AISettingsModel::prepareSetup(const QString &type) {
  const auto *info = AI::findProviderType(type.toStdString());
  auto rows = info ? fieldRows({}, *info, false) : std::vector<AIProviderFieldRow>{};
  QStringList keys;
  for (const auto &row : rows) {
    keys << row.key;
  }
  m_setupFields.setRows(std::move(rows));
  return keys;
}

QString AISettingsModel::nextProviderId(const QString &type) const {
  auto typeStd = type.toStdString();
  const auto configured = providers();

  int suffix = 1;
  std::string id;
  do {
    id = suffix == 1 ? typeStd : std::format("{}-{}", typeStd, suffix);
    ++suffix;
  } while (configured.contains(id));

  return QString::fromStdString(id);
}

bool AISettingsModel::isProviderIdTaken(const QString &id) const {
  return providers().contains(id.toStdString());
}

void AISettingsModel::addProvider(const QString &type, const QVariantMap &fields) {
  auto typeStd = type.toStdString();
  const auto *typeInfo = AI::findProviderType(typeStd);
  if (!typeInfo || !canAddType(*typeInfo)) return;

  std::string id;
  if (fields.contains(QStringLiteral("id"))) { id = fields[QStringLiteral("id")].toString().toStdString(); }
  if (id.empty() || isProviderIdTaken(QString::fromStdString(id))) {
    id = nextProviderId(type).toStdString();
  }

  AI::ProviderInstance instance{.type = std::move(typeStd)};
  const auto scope = AI::Service::secretScope(id);

  for (const auto &pref : typeInfo->fields()) {
    const auto key = pref.name();
    if (!fields.contains(key)) continue;
    auto value = toStoredValue(pref, fields[key]);
    if (pref.isSecret()) {
      m_storage->setItem(scope, key, glazeToQJsonValue(value));
    } else {
      instance.fields[key.toStdString()] = std::move(value);
    }
  }

  auto configured = providers();
  configured[id] = std::move(instance);
  saveProviders(configured);
}

void AISettingsModel::removeProvider(const std::string &id) {
  m_storage->clearNamespace(AI::Service::secretScope(id));
  auto configured = providers();
  configured.erase(id);
  saveProviders(configured);
}

void AISettingsModel::setField(const std::string &id, const QString &key, const QVariant &value) {
  const auto *typeInfo = typeInfoFor(id);
  if (!typeInfo) return;

  const auto fields = typeInfo->fields();
  const auto *pref = findField(fields, key);
  if (!pref) return;

  auto stored = toStoredValue(*pref, value);

  if (pref->isSecret()) {
    m_storage->setItem(AI::Service::secretScope(id), key, glazeToQJsonValue(stored));
    m_aiService->reconfigure(id);
    return;
  }

  auto configured = providers();
  auto &instance = configured[id];
  instance.type = std::string(typeInfo->type);
  const auto keyStd = key.toStdString();
  std::string before;
  std::string after;
  if (const auto *current = preferences::find(instance.fields, keyStd)) {
    if (!glz::write_json(*current, before) && !glz::write_json(stored, after) && before == after) return;
  }
  instance.fields[keyStd] = std::move(stored);
  saveProviders(configured);
}

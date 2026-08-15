#include "preference-form-model.hpp"

#include "view-utils.hpp"
#include <QJSValue>
#include <utility>
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

using namespace std::chrono_literals;

PreferenceFormModel::PreferenceFormModel(QObject *parent) : QAbstractListModel(parent) {
  m_saveTimer.setInterval(1s);
  m_saveTimer.setSingleShot(true);
  connect(&m_saveTimer, &QTimer::timeout, this, &PreferenceFormModel::save);
}

PreferenceFormModel::~PreferenceFormModel() {
  if (m_saveTimer.isActive()) save();
}

int PreferenceFormModel::rowCount(const QModelIndex &) const { return static_cast<int>(m_fields.size()); }

QVariant PreferenceFormModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_fields.size())) return {};
  const auto &f = m_fields[index.row()];
  switch (role) {
  case TypeRole:
    return f.type;
  case FieldIdRole:
    return f.id;
  case LabelRole:
    return f.label;
  case CheckboxLabelRole:
    return f.checkboxLabel;
  case DescriptionRole:
    return f.description;
  case PlaceholderRole:
    return f.placeholder;
  case ValueRole:
    return f.value;
  case DropdownModelRole:
    return QVariant::fromValue(static_cast<QObject *>(f.dropdownModel));
  case CurrentDropdownItemRole:
    return currentDropdownItem(f);
  case ReadOnlyRole:
    return f.readOnly;
  case MultipleRole:
    return f.multiple;
  case CanChooseFilesRole:
    return f.canChooseFiles;
  case CanChooseDirectoriesRole:
    return f.canChooseDirectories;
  case LockedPathsRole:
    return f.lockedPaths;
  default:
    return {};
  }
}

QHash<int, QByteArray> PreferenceFormModel::roleNames() const {
  return {{TypeRole, "type"},
          {FieldIdRole, "fieldId"},
          {LabelRole, "label"},
          {CheckboxLabelRole, "checkboxLabel"},
          {DescriptionRole, "description"},
          {PlaceholderRole, "placeholder"},
          {ValueRole, "value"},
          {DropdownModelRole, "dropdownModel"},
          {CurrentDropdownItemRole, "currentDropdownItem"},
          {ReadOnlyRole, "readOnly"},
          {MultipleRole, "multiple"},
          {CanChooseFilesRole, "canChooseFiles"},
          {CanChooseDirectoriesRole, "canChooseDirectories"},
          {LockedPathsRole, "lockedPaths"}};
}

static QString preferenceType(const Preference &p) {
  return std::visit(
      [](const auto &d) -> QString {
        using T = std::decay_t<decltype(d)>;
        if constexpr (std::is_same_v<T, Preference::TextData>)
          return QStringLiteral("text");
        else if constexpr (std::is_same_v<T, Preference::PasswordData>)
          return QStringLiteral("password");
        else if constexpr (std::is_same_v<T, Preference::CheckboxData>)
          return QStringLiteral("checkbox");
        else if constexpr (std::is_same_v<T, Preference::DropdownData>)
          return QStringLiteral("dropdown");
        else if constexpr (std::is_same_v<T, Preference::FilePickerData>)
          return QStringLiteral("filepicker");
        else if constexpr (std::is_same_v<T, Preference::DirectoryPickerData>)
          return QStringLiteral("directorypicker");
        else
          return QStringLiteral("text");
      },
      p.data());
}

static QVariantList dropdownOptions(const Preference &p) {
  auto d = p.data();
  if (auto *dd = std::get_if<Preference::DropdownData>(&d)) {
    QVariantList items;
    for (const auto &opt : dd->options) {
      items.append(qml::makeDropdownItem(opt.value, opt.title));
    }
    return items;
  }
  return {};
}

static void applyPickerFlags(const Preference &p, bool &multiple, bool &canChooseFiles,
                             bool &canChooseDirectories, QStringList &lockedPaths) {
  auto d = p.data();
  if (auto *fp = std::get_if<Preference::FilePickerData>(&d)) {
    multiple = fp->multiple;
    canChooseFiles = true;
    canChooseDirectories = false;
    lockedPaths = QStringList(fp->lockedPaths.begin(), fp->lockedPaths.end());
  }
  if (auto *dp = std::get_if<Preference::DirectoryPickerData>(&d)) {
    multiple = dp->multiple;
    canChooseFiles = false;
    canChooseDirectories = true;
    lockedPaths = QStringList(dp->lockedPaths.begin(), dp->lockedPaths.end());
  }
}

static bool isFilePickerType(const Preference &p) {
  return std::holds_alternative<Preference::FilePickerData>(p.data()) ||
         std::holds_alternative<Preference::DirectoryPickerData>(p.data());
}

static QJsonValue normalizeFilePickerValue(const QJsonValue &v) {
  if (v.isArray()) return v;
  if (v.isString()) {
    auto s = v.toString();
    return s.isEmpty() ? QJsonArray{} : QJsonArray{s};
  }
  return QJsonArray{};
}

static QString checkboxLabel(const Preference &p) {
  auto d = p.data();
  if (auto *cb = std::get_if<Preference::CheckboxData>(&d)) return cb->label.value_or(QString());
  return {};
}

QVariant PreferenceFormModel::currentDropdownItem(const Field &f) {
  if (!f.dropdownModel) return {};
  auto option = f.dropdownModel->itemDataById(f.value.toString());
  return option.isEmpty() ? QVariant{} : QVariant(option);
}

void PreferenceFormModel::clearFields() {
  for (const auto &f : m_fields) {
    if (f.dropdownModel) f.dropdownModel->deleteLater();
  }
  m_fields.clear();
}

PreferenceFormModel::Field PreferenceFormModel::createField(const Preference &pref) {
  Field f;
  f.type = preferenceType(pref);
  f.id = pref.name();
  f.label = pref.title();
  f.checkboxLabel = checkboxLabel(pref);
  f.description = pref.description();
  f.placeholder = pref.placeholder();
  f.readOnly = pref.isReadOnly();

  if (auto options = dropdownOptions(pref); !options.isEmpty()) {
    f.dropdownModel = new CompletionModel(this);
    f.dropdownModel->setItems(options);
  }

  applyPickerFlags(pref, f.multiple, f.canChooseFiles, f.canChooseDirectories, f.lockedPaths);

  QJsonValue raw = m_values.contains(pref.name()) ? m_values.value(pref.name()) : pref.defaultValue();
  if (isFilePickerType(pref)) raw = normalizeFilePickerValue(raw);
  f.value = raw.toVariant();

  return f;
}

void PreferenceFormModel::load(const EntrypointId &id, const std::vector<Preference> &preferences) {
  if (m_saveTimer.isActive()) save();
  beginResetModel();
  m_itemId = id;
  m_isProvider = false;
  clearFields();

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  m_values = manager->getItemPreferenceValues(id);

  for (const auto &pref : preferences) {
    m_fields.push_back(createField(pref));
  }
  endResetModel();
}

void PreferenceFormModel::loadProvider(const QString &providerId,
                                       const std::vector<Preference> &preferences) {
  if (m_saveTimer.isActive()) save();
  beginResetModel();
  m_providerId = providerId;
  m_isProvider = true;
  clearFields();

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  m_values = manager->getProviderPreferenceValues(providerId);

  for (const auto &pref : preferences) {
    m_fields.push_back(createField(pref));
  }
  endResetModel();
}

void PreferenceFormModel::setFieldValue(int row, const QVariant &value) {
  if (row < 0 || std::cmp_greater_equal(row, m_fields.size())) return;
  auto resolved = value;
  if (resolved.canConvert<QJSValue>()) resolved = resolved.value<QJSValue>().toVariant();
  m_fields[row].value = resolved;
  m_values[m_fields[row].id] = QJsonValue::fromVariant(resolved);
  auto idx = index(row);
  emit dataChanged(idx, idx, {ValueRole, CurrentDropdownItemRole});
  m_saveTimer.start();
}

void PreferenceFormModel::save() {
  auto *manager = ServiceRegistry::instance()->rootItemManager();
  if (m_isProvider)
    manager->setProviderPreferenceValues(m_providerId, m_values);
  else
    manager->setItemPreferenceValues(m_itemId, m_values);
}

#include "preference-form-model.hpp"

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
  case DescriptionRole:
    return f.description;
  case PlaceholderRole:
    return f.placeholder;
  case ValueRole:
    return f.value;
  case OptionsRole:
    return f.options;
  case ReadOnlyRole:
    return f.readOnly;
  case MultipleRole:
    return f.multiple;
  case DirectoriesOnlyRole:
    return f.directoriesOnly;
  default:
    return {};
  }
}

QHash<int, QByteArray> PreferenceFormModel::roleNames() const {
  return {{TypeRole, "type"},
          {FieldIdRole, "fieldId"},
          {LabelRole, "label"},
          {DescriptionRole, "description"},
          {PlaceholderRole, "placeholder"},
          {ValueRole, "value"},
          {OptionsRole, "options"},
          {ReadOnlyRole, "readOnly"},
          {MultipleRole, "multiple"},
          {DirectoriesOnlyRole, "directoriesOnly"}};
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
      items.append(
          QVariantMap{{QStringLiteral("id"), opt.value}, {QStringLiteral("displayName"), opt.title}});
    }
    return {QVariantMap{{QStringLiteral("title"), QString()}, {QStringLiteral("items"), items}}};
  }
  return {};
}

static void applyPickerFlags(const Preference &p, bool &multiple, bool &directoriesOnly) {
  auto d = p.data();
  if (auto *fp = std::get_if<Preference::FilePickerData>(&d)) multiple = fp->multiple;
  if (auto *dp = std::get_if<Preference::DirectoryPickerData>(&d)) {
    multiple = dp->multiple;
    directoriesOnly = true;
  }
}

static QString resolveLabel(const Preference &p) {
  auto d = p.data();
  if (auto *cb = std::get_if<Preference::CheckboxData>(&d)) return cb->label.value_or(p.title());
  return p.title();
}

void PreferenceFormModel::load(const EntrypointId &id, const std::vector<Preference> &preferences) {
  beginResetModel();
  m_itemId = id;
  m_isProvider = false;
  m_fields.clear();

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  m_values = manager->getItemPreferenceValues(id);

  for (const auto &pref : preferences) {
    Field f;
    f.type = preferenceType(pref);
    f.id = pref.name();
    f.label = resolveLabel(pref);
    f.description = pref.description();
    f.placeholder = pref.placeholder();
    f.readOnly = pref.isReadOnly();
    f.options = dropdownOptions(pref);

    applyPickerFlags(pref, f.multiple, f.directoriesOnly);

    if (m_values.contains(pref.name()))
      f.value = m_values.value(pref.name()).toVariant();
    else
      f.value = pref.defaultValue().toVariant();

    m_fields.push_back(std::move(f));
  }
  endResetModel();
}

void PreferenceFormModel::loadProvider(const QString &providerId,
                                       const std::vector<Preference> &preferences) {
  beginResetModel();
  m_providerId = providerId;
  m_isProvider = true;
  m_fields.clear();

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  m_values = manager->getProviderPreferenceValues(providerId);

  for (const auto &pref : preferences) {
    Field f;
    f.type = preferenceType(pref);
    f.id = pref.name();
    f.label = resolveLabel(pref);
    f.description = pref.description();
    f.placeholder = pref.placeholder();
    f.readOnly = pref.isReadOnly();
    f.options = dropdownOptions(pref);

    applyPickerFlags(pref, f.multiple, f.directoriesOnly);

    if (m_values.contains(pref.name()))
      f.value = m_values.value(pref.name()).toVariant();
    else
      f.value = pref.defaultValue().toVariant();

    m_fields.push_back(std::move(f));
  }
  endResetModel();
}

void PreferenceFormModel::setFieldValue(int row, const QVariant &value) {
  if (row < 0 || std::cmp_greater_equal(row, m_fields.size())) return;
  m_fields[row].value = value;
  m_values[m_fields[row].id] = QJsonValue::fromVariant(value);
  auto idx = index(row);
  emit dataChanged(idx, idx, {ValueRole});
  m_saveTimer.start();
}

void PreferenceFormModel::save() {
  auto *manager = ServiceRegistry::instance()->rootItemManager();
  if (m_isProvider)
    manager->setProviderPreferenceValues(m_providerId, m_values);
  else
    manager->setItemPreferenceValues(m_itemId, m_values);
}

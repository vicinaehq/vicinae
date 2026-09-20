#include "ui/settings/preference-form-model.hpp"

#include "ui/views/view-utils.hpp"
#include <QJSValue>
#include <algorithm>
#include "internal/glaze-qt.hpp"
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
  case ComponentRole:
    return f.component;
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
          {LockedPathsRole, "lockedPaths"},
          {ComponentRole, "component"}};
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
        else if constexpr (std::is_same_v<T, Preference::AppPickerData>)
          return QStringLiteral("apppicker");
        else if constexpr (std::is_same_v<T, Preference::ShortcutData>)
          return QStringLiteral("shortcut");
        else if constexpr (std::is_same_v<T, Preference::CustomData>)
          return QStringLiteral("custom");
        else
          return QStringLiteral("text");
      },
      p.data());
}

static QVariantMap dropdownItem(const Preference::DropdownData::Option &opt) {
  return qml::makeDropdownItem(opt.value, opt.title, opt.icon ? qml::imageSourceFor(*opt.icon) : QString());
}

static bool populateDropdown(CompletionModel *model, const Preference &p) {
  auto d = p.data();
  auto *dd = std::get_if<Preference::DropdownData>(&d);
  if (!dd) return false;

  if (!dd->sections.empty()) {
    QVariantList sections;
    for (const auto &section : dd->sections) {
      QVariantList items;
      for (const auto &opt : section.options)
        items.append(dropdownItem(opt));
      sections.append(
          QVariantMap{{QStringLiteral("title"), section.title}, {QStringLiteral("items"), items}});
    }
    model->setSections(sections);
    return true;
  }

  if (dd->options.empty()) return false;
  QVariantList items;
  for (const auto &opt : dd->options)
    items.append(dropdownItem(opt));
  model->setItems(items);
  return true;
}

static void applyPickerFlags(const Preference &p, bool &multiple, bool &canChooseFiles,
                             bool &canChooseDirectories, QStringList &lockedPaths) {
  auto d = p.data();
  if (auto *ap = std::get_if<Preference::AppPickerData>(&d)) { multiple = ap->multiple; }
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

static bool isMultiValueType(const Preference &p) {
  auto d = p.data();
  if (auto *ap = std::get_if<Preference::AppPickerData>(&d)) return ap->multiple;
  return std::holds_alternative<Preference::FilePickerData>(d) ||
         std::holds_alternative<Preference::DirectoryPickerData>(d);
}

static glz::generic normalizeListValue(const glz::generic &v) {
  if (v.is_array()) return v;
  if (v.is_string() && !v.get_string().empty()) return glz::generic::array_t{v};
  return glz::generic::array_t{};
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
    if (f.dropdownModel && f.dropdownModel != appModel()) f.dropdownModel->deleteLater();
  }
  m_fields.clear();
}

CompletionModel *PreferenceFormModel::appModel() {
  if (!m_appModel) m_appModel = new AppSelectorModel(this);
  return m_appModel->model();
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

  if (std::holds_alternative<Preference::DropdownData>(pref.data())) {
    f.dropdownModel = new CompletionModel(this);
    if (!populateDropdown(f.dropdownModel, pref)) {
      f.dropdownModel->deleteLater();
      f.dropdownModel = nullptr;
    }
  } else if (std::holds_alternative<Preference::AppPickerData>(pref.data())) {
    f.dropdownModel = appModel();
  }

  applyPickerFlags(pref, f.multiple, f.canChooseFiles, f.canChooseDirectories, f.lockedPaths);
  if (auto data = pref.data(); auto *custom = std::get_if<Preference::CustomData>(&data)) {
    f.component = qml::componentUrl(custom->component);
  }

  const auto *stored = preferences::find(m_values, pref.name().toStdString());
  glz::generic raw = stored ? *stored : pref.defaultOrNull();
  if (isMultiValueType(pref)) raw = normalizeListValue(raw);
  f.value = glazeToQVariant(raw);

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
  m_values[m_fields[row].id.toStdString()] = qVariantToGlazeGeneric(resolved);
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

QUrl PreferenceFormModel::componentFor(const QString &fieldId) const {
  auto it = std::ranges::find(m_fields, fieldId, &Field::id);
  return it == m_fields.end() ? QUrl() : it->component;
}

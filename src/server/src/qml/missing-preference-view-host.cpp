#include "missing-preference-view-host.hpp"

#include <QJsonArray>
#include <common/enumerate.hpp>
#include <utility>
#include "extension/extension-command.hpp"
#include "navigation-controller.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

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

static void applyPickerFlags(const Preference &p, bool &multiple, bool &canChooseFiles,
                             bool &canChooseDirectories) {
  auto d = p.data();
  if (auto *fp = std::get_if<Preference::FilePickerData>(&d)) {
    multiple = fp->multiple;
    canChooseFiles = true;
    canChooseDirectories = false;
  }
  if (auto *dp = std::get_if<Preference::DirectoryPickerData>(&d)) {
    multiple = dp->multiple;
    canChooseFiles = false;
    canChooseDirectories = true;
  }
}

static QString checkboxLabel(const Preference &p) {
  auto d = p.data();
  if (auto *cb = std::get_if<Preference::CheckboxData>(&d)) return cb->label.value_or(QString());
  return {};
}

MissingPreferenceFormModel::MissingPreferenceFormModel(QObject *parent) : QAbstractListModel(parent) {}

int MissingPreferenceFormModel::rowCount(const QModelIndex &) const {
  return static_cast<int>(m_fields.size());
}

QVariant MissingPreferenceFormModel::data(const QModelIndex &index, int role) const {
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
  case OptionsRole:
    return f.options;
  case ReadOnlyRole:
    return false;
  case MultipleRole:
    return f.multiple;
  case CanChooseFilesRole:
    return f.canChooseFiles;
  case CanChooseDirectoriesRole:
    return f.canChooseDirectories;
  default:
    return {};
  }
}

QHash<int, QByteArray> MissingPreferenceFormModel::roleNames() const {
  return {{TypeRole, "type"},
          {FieldIdRole, "fieldId"},
          {LabelRole, "label"},
          {CheckboxLabelRole, "checkboxLabel"},
          {DescriptionRole, "description"},
          {PlaceholderRole, "placeholder"},
          {ValueRole, "value"},
          {OptionsRole, "options"},
          {ReadOnlyRole, "readOnly"},
          {MultipleRole, "multiple"},
          {CanChooseFilesRole, "canChooseFiles"},
          {CanChooseDirectoriesRole, "canChooseDirectories"}};
}

void MissingPreferenceFormModel::load(const std::vector<Preference> &preferences,
                                      const QJsonObject &existingValues) {
  beginResetModel();
  m_fields.clear();
  m_values = QJsonObject{};

  for (const auto &pref : preferences) {
    QJsonValue const value = existingValues.value(pref.name());
    bool const hasValue = !(value.isUndefined() || value.isNull());
    bool const hasDefault = !pref.defaultValue().isUndefined();
    bool const isMissing = pref.required() && !hasValue && !hasDefault;

    if (!isMissing) continue;

    Field f;
    f.type = preferenceType(pref);
    f.id = pref.name();
    f.label = pref.title();
    f.checkboxLabel = checkboxLabel(pref);
    f.description = pref.description();
    f.placeholder = pref.placeholder();
    f.options = dropdownOptions(pref);
    applyPickerFlags(pref, f.multiple, f.canChooseFiles, f.canChooseDirectories);

    if (f.type == QStringLiteral("checkbox")) {
      f.value = false;
      m_values[f.id] = false;
    }

    m_fields.push_back(std::move(f));
  }
  endResetModel();
}

void MissingPreferenceFormModel::setFieldValue(int row, const QVariant &value) {
  if (row < 0 || std::cmp_greater_equal(row, m_fields.size())) return;
  m_fields[row].value = value;
  m_values[m_fields[row].id] = QJsonValue::fromVariant(value);
  auto idx = index(row);
  emit dataChanged(idx, idx, {ValueRole});
}

static bool isEmptyPreferenceValue(const QJsonValue &v) {
  if (v.isNull() || v.isUndefined()) return true;
  if (v.isString()) return v.toString().isEmpty();
  if (v.isArray()) return v.toArray().isEmpty();
  return false;
}

MissingPreferenceFormModel::ValidateResult MissingPreferenceFormModel::validate() const {
  for (const auto &[i, f] : m_fields | vicinae::enumerate) {
    if (isEmptyPreferenceValue(m_values.value(f.id))) return {false, static_cast<int>(i)};
  }
  return {true, -1};
}

MissingPreferenceViewHost::MissingPreferenceViewHost(std::shared_ptr<ExtensionCommand> command,
                                                     const std::vector<Preference> &preferences,
                                                     const QJsonObject &preferenceValues)
    : m_command(std::move(command)), m_prefModel(new MissingPreferenceFormModel(this)) {
  m_commandIconSource = qml::imageSourceFor(m_command->iconUrl());
  m_prefModel->load(preferences, preferenceValues);
}

QUrl MissingPreferenceViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/MissingPreferenceView.qml"));
}

QVariantMap MissingPreferenceViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void MissingPreferenceViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  auto submitAction = new StaticAction(QStringLiteral("Save preferences"), ImageURL::builtin("enter-key"),
                                       [this]() { submit(); });
  section->addAction(submitAction);
  setActions(std::move(panel));
}

void MissingPreferenceViewHost::submit() {
  auto result = m_prefModel->validate();

  if (!result.valid) {
    context()->services->toastService()->failure("Please fill in all required fields");
    return;
  }

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  manager->setPreferenceValues(m_command->uniqueId(), m_prefModel->values());

  popSelf();
  context()->navigation->launch(m_command);
}

QString MissingPreferenceViewHost::commandName() const { return m_command->repositoryDisplayName(); }

QString MissingPreferenceViewHost::commandIconSource() const { return m_commandIconSource; }

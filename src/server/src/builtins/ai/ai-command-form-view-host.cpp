#include "ai-command-form-view-host.hpp"
#include <QUrl>
#include "builtins/ai/ai-commands.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-model-selector-utils.hpp"
#include "services/ai/ai-preferences.hpp"
#include "services/ai/ai-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/view-utils.hpp"

namespace {

const auto DEFAULT_ICON_ID = QStringLiteral("default");

QString outputId(AiCommandStore::Output output) {
  switch (output) {
  case AiCommandStore::Output::Paste:
    return QStringLiteral("paste");
  case AiCommandStore::Output::Copy:
    return QStringLiteral("copy");
  }
  return {};
}

} // namespace

AiCommandFormViewHost::AiCommandFormViewHost(AiCommandStore::Command command, Mode mode)
    : m_mode(mode), m_initial(std::move(command)) {}

QUrl AiCommandFormViewHost::qmlComponentUrl() const { return qml::componentUrl(u"AiCommandFormView"); }

QVariantMap AiCommandFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

QString AiCommandFormViewHost::initialNavigationTitle() const {
  switch (m_mode) {
  case Mode::Edit:
    return tr("Edit \"%1\"").arg(QString::fromStdString(m_initial->name));
  case Mode::Duplicate:
    return tr("Duplicate \"%1\"").arg(QString::fromStdString(m_initial->name));
  case Mode::Create:
    break;
  }
  return tr("Create AI Command");
}

ImageURL AiCommandFormViewHost::initialNavigationIcon() const {
  if (m_mode == Mode::Edit) return AiCommands::iconUrl(*m_initial);
  return AiCommands::defaultIconUrl().setBadge(BuiltinIcon::Plus);
}

const AiCommandStore::Command *AiCommandFormViewHost::editedCommand() const {
  return m_mode == Mode::Edit ? &*m_initial : nullptr;
}

void AiCommandFormViewHost::initialize() {
  BaseView::initialize();

  auto *services = context()->services;

  auto panel = std::make_unique<FormActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(editedCommand() ? tr("Save Command") : tr("Create Command"),
                                      ImageURL::builtin(BuiltinIcon::EnterKey), [this]() { submit(); }));
  setActions(std::move(panel));

  buildIconItems();
  m_selectedIcon = m_defaultIconEntry;

  if (m_initial) {
    m_name = m_mode == Mode::Duplicate ? tr("Copy of %1").arg(QString::fromStdString(m_initial->name))
                                       : QString::fromStdString(m_initial->name);
    m_prompt = QString::fromStdString(m_initial->prompt);
    m_modelId = QString::fromStdString(m_initial->model);
    m_output = m_initial->output;
    if (auto item = m_iconModel.itemDataById(QString::fromStdString(m_initial->icon)); !item.isEmpty()) {
      m_selectedIcon = item;
    }
  } else {
    const auto model = AI::defaultModel(*services->config());
    if (model && services->ai()->isAvailable(*model, AI::Capability::Completion)) {
      m_modelId = QString::fromStdString(model->toString());
    }
  }

  connect(services->ai(), &AI::Service::modelsChanged, this, &AiCommandFormViewHost::rebuildModels);
  rebuildModels();
  emit formChanged();
}

QVariantList AiCommandFormViewHost::promptCompletions() const {
  return {
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::TextCursor))},
          {QStringLiteral("title"), tr("Selected Text")},
          {QStringLiteral("value"), QString::fromLatin1(AiCommands::SELECTION_PLACEHOLDER)},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::CopyClipboard))},
          {QStringLiteral("title"), tr("Clipboard Text")},
          {QStringLiteral("value"), QString::fromLatin1(AiCommands::CLIPBOARD_PLACEHOLDER)},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::TextCursor))},
          {QStringLiteral("title"), tr("Argument")},
          {QStringLiteral("value"), QStringLiteral("argument")},
          {QStringLiteral("template"), QStringLiteral("{argument name=\"\"}")},
          {QStringLiteral("cursorOffset"), 16},
      },
  };
}

void AiCommandFormViewHost::buildIconItems() {
  QVariantList icons;
  icons.reserve(static_cast<qsizetype>(BuiltinIconService::mapping().size()) + 1);

  m_defaultIconEntry = qml::makeDropdownItem(
      DEFAULT_ICON_ID, tr("Default"), qml::imageSourceFor(ImageURL::builtin(AiCommands::DEFAULT_ICON)));
  icons.append(m_defaultIconEntry);

  for (const auto &[icon, name] : BuiltinIconService::mapping()) {
    auto url = ImageURL::builtinByName(QString::fromLatin1(name));
    icons.append(qml::makeDropdownItem(url.toString(), QString::fromUtf8(name), qml::imageSourceFor(url)));
  }

  m_iconModel.setItems(icons);
}

void AiCommandFormViewHost::rebuildModels() {
  m_modelItems = buildGroupedModelList(context()->services->ai(), AI::Capability::Completion);
  m_currentModel.clear();

  for (const auto &section : std::as_const(m_modelItems)) {
    const auto items = section.toMap()[QStringLiteral("items")].toList();
    for (const auto &item : items) {
      const auto map = item.toMap();
      if (map[QStringLiteral("id")].toString() == m_modelId) m_currentModel = map;
    }
  }

  emit modelsChanged();
}

void AiCommandFormViewHost::selectModel(const QVariantMap &item) {
  m_modelId = item[QStringLiteral("id")].toString();
  m_currentModel = item;
  emit modelsChanged();
}

QVariantList AiCommandFormViewHost::outputItems() const {
  using enum AiCommandStore::Output;
  return {QVariantMap{
      {QStringLiteral("items"),
       QVariantList{
           qml::makeDropdownItem(outputId(Paste), tr("Paste in Active App"),
                                 qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::TextCursor))),
           qml::makeDropdownItem(outputId(Copy), tr("Copy to Clipboard"),
                                 qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::CopyClipboard))),
       }},
  }};
}

QVariantMap AiCommandFormViewHost::currentOutput() const {
  const auto items = outputItems().first().toMap()[QStringLiteral("items")].toList();
  for (const auto &item : items) {
    if (item.toMap()[QStringLiteral("id")].toString() == outputId(m_output)) return item.toMap();
  }
  return {};
}

void AiCommandFormViewHost::selectOutput(const QVariantMap &item) {
  using enum AiCommandStore::Output;
  m_output = item[QStringLiteral("id")].toString() == outputId(Copy) ? Copy : Paste;
  emit formChanged();
}

void AiCommandFormViewHost::selectIcon(const QVariantMap &item) {
  m_selectedIcon = item;
  emit formChanged();
}

void AiCommandFormViewHost::submit() {
  auto *services = context()->services;
  auto *store = services->aiCommands();
  const auto *edited = editedCommand();
  const auto name = m_name.trimmed().toStdString();
  const auto prompt = m_prompt.trimmed().toStdString();

  m_nameError.clear();
  m_promptError.clear();
  m_modelError.clear();

  if (name.empty()) {
    m_nameError = tr("Enter a name");
  } else if (const auto *other = store->findByName(name); other && (!edited || other->id != edited->id)) {
    m_nameError = tr("Another command has this name");
  }
  if (prompt.empty()) m_promptError = tr("Enter a prompt");
  if (m_currentModel.isEmpty()) m_modelError = tr("Choose a model");

  emit errorsChanged();

  if (!m_nameError.isEmpty() || !m_promptError.isEmpty() || !m_modelError.isEmpty()) return;

  const auto iconId = m_selectedIcon[QStringLiteral("id")].toString();
  AiCommandStore::Command command{
      .id = edited ? edited->id : std::string(),
      .name = name,
      .icon = iconId == DEFAULT_ICON_ID ? std::string() : iconId.toStdString(),
      .prompt = prompt,
      .model = m_modelId.toStdString(),
      .output = m_output,
  };

  if (edited) {
    if (!store->update(std::move(command))) {
      services->toastService()->failure(tr("Failed to save command"));
      return;
    }
  } else {
    store->add(std::move(command));
  }

  services->toastService()->success(edited ? tr("Command saved") : tr("Command created"));
  popSelf();
}

void AiCommandFormViewHost::setName(const QString &value) {
  if (m_name == value) return;
  m_name = value;
  emit formChanged();
}

void AiCommandFormViewHost::setPrompt(const QString &value) {
  if (m_prompt == value) return;
  m_prompt = value;
  emit formChanged();
}

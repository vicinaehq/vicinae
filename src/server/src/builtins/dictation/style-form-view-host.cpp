#include "style-form-view-host.hpp"
#include <memory>
#include <ranges>
#include <QUrl>
#include "builtins/ai/ai-model-selector-utils.hpp"
#include "builtins/ai/ai.hpp"
#include "builtins/ai/quick-ai-preferences.hpp"
#include "builtins/dictation/dictation.hpp"
#include "common/entrypoint.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-service.hpp"
#include "services/app-service/app-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/view-utils.hpp"

DictationStyleFormViewHost::DictationStyleFormViewHost(DictationStyles::Style style)
    : m_initial(std::move(style)) {}

QUrl DictationStyleFormViewHost::qmlComponentUrl() const {
  return qml::componentUrl(u"DictationStyleFormView");
}

QVariantMap DictationStyleFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

QString DictationStyleFormViewHost::initialNavigationTitle() const {
  return m_initial ? tr("Edit \"%1\"").arg(QString::fromStdString(m_initial->name))
                   : tr("Create Dictation Style");
}

ImageURL DictationStyleFormViewHost::initialNavigationIcon() const {
  return m_initial ? Dictation::STYLE_ICON : ImageURL(Dictation::STYLE_ICON).setBadge(BuiltinIcon::Plus);
}

void DictationStyleFormViewHost::initialize() {
  BaseView::initialize();

  auto *services = context()->services;
  m_appSelectorModel = new AppSelectorModel(this);

  auto panel = std::make_unique<FormActionPanelState>();
  auto *section = panel->createSection();
  section->addAction(new StaticAction(m_initial ? tr("Save Style") : tr("Create Style"),
                                      ImageURL::builtin(BuiltinIcon::EnterKey), [this]() { submit(); }));
  setActions(std::move(panel));

  if (m_initial) {
    m_name = QString::fromStdString(m_initial->name);
    m_instructions = QString::fromStdString(m_initial->instructions);
    m_modelId = QString::fromStdString(m_initial->model);
    m_useAsDefault = services->dictation()->styles()->isDefault(m_initial->id);
    for (const auto &app : m_initial->apps)
      m_apps.append(QString::fromStdString(app));
  } else {
    const EntrypointId quickAI{std::string(QuickAI::REPOSITORY_ID), std::string(QuickAI::COMMAND_ID)};
    const auto values = services->rootItemManager()->getPreferenceValues(quickAI);
    const auto *model = preferences::find(values, "model");
    if (model && model->is_string()) {
      if (const auto ref = QuickAI::availableModel(*services->ai(), model->get_string())) {
        m_modelId = QString::fromStdString(ref->toString());
      }
    }
  }

  connect(services->ai(), &AI::Service::modelsChanged, this, &DictationStyleFormViewHost::rebuildModels);
  rebuildModels();
  emit formChanged();
}

void DictationStyleFormViewHost::rebuildModels() {
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

void DictationStyleFormViewHost::selectModel(const QVariantMap &item) {
  m_modelId = item[QStringLiteral("id")].toString();
  m_currentModel = item;
  emit modelsChanged();
}

QString DictationStyleFormViewHost::appsConflict() const {
  auto *services = context()->services;
  const auto *styles = services->dictation()->styles();

  for (const auto &app : m_apps) {
    const auto *owner = styles->owner(app.toStdString());
    if (!owner || (m_initial && owner->id == m_initial->id)) continue;
    const auto info = services->appDb()->findById(app);
    return tr("%1 already uses %2")
        .arg(info ? info->displayName() : app, QString::fromStdString(owner->name));
  }
  return {};
}

void DictationStyleFormViewHost::submit() {
  auto *services = context()->services;
  auto *styles = services->dictation()->styles();
  const auto name = m_name.trimmed().toStdString();
  const auto instructions = m_instructions.trimmed().toStdString();

  m_nameError.clear();
  m_instructionsError.clear();
  m_modelError.clear();

  if (name.empty()) {
    m_nameError = tr("Enter a name");
  } else if (const auto *other = styles->findByName(name);
             other && (!m_initial || other->id != m_initial->id)) {
    m_nameError = tr("Another style has this name");
  }
  if (instructions.empty()) m_instructionsError = tr("Describe how to rewrite the text");
  if (m_currentModel.isEmpty()) m_modelError = tr("Choose a model");
  m_appsError = appsConflict();

  emit errorsChanged();

  if (!m_nameError.isEmpty() || !m_instructionsError.isEmpty() || !m_modelError.isEmpty() ||
      !m_appsError.isEmpty()) {
    return;
  }

  DictationStyles::Style style{
      .id = m_initial ? m_initial->id : std::string(),
      .name = name,
      .instructions = instructions,
      .model = m_modelId.toStdString(),
      .apps = m_apps | std::views::transform([](const QString &app) { return app.toStdString(); }) |
              std::ranges::to<std::vector>(),
  };

  std::string id;
  if (m_initial) {
    id = style.id;
    if (!styles->update(std::move(style))) {
      services->toastService()->failure(tr("Failed to save style"));
      return;
    }
  } else {
    id = styles->add(std::move(style));
  }

  if (m_useAsDefault) {
    styles->setDefault(std::move(id));
  } else if (styles->isDefault(id)) {
    styles->setDefault(std::nullopt);
  }

  services->toastService()->success(m_initial ? tr("Style saved") : tr("Style created"));
  popSelf();
}

void DictationStyleFormViewHost::setName(const QString &value) {
  if (m_name == value) return;
  m_name = value;
  emit formChanged();
}

void DictationStyleFormViewHost::setInstructions(const QString &value) {
  if (m_instructions == value) return;
  m_instructions = value;
  emit formChanged();
}

void DictationStyleFormViewHost::setApps(const QStringList &value) {
  if (m_apps == value) return;
  m_apps = value;
  emit formChanged();
}

void DictationStyleFormViewHost::setUseAsDefault(bool value) {
  if (m_useAsDefault == value) return;
  m_useAsDefault = value;
  emit formChanged();
}

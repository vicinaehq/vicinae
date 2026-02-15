#include "qml-create-extension-view-host.hpp"
#include "extensions/developer/create/create-extension-success-view.hpp"
#include "navigation-controller.hpp"
#include "qml-utils.hpp"
#include "service-registry.hpp"
#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "utils/utils.hpp"
#include <QUrl>
#include <filesystem>

QmlCreateExtensionViewHost::QmlCreateExtensionViewHost(QWidget *parent) : QmlFormViewBase() {}

QUrl QmlCreateExtensionViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/CreateExtensionFormView.qml"));
}

QVariantMap QmlCreateExtensionViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlCreateExtensionViewHost *>(this))}};
}

void QmlCreateExtensionViewHost::initialize() {
  BaseView::initialize();

  // Build template items from boilerplate generator
  ExtensionBoilerplateGenerator gen;
  QVariantList items;
  for (const auto &tmpl : gen.commandBoilerplates()) {
    items.append(QVariantMap{
        {QStringLiteral("id"), tmpl.resource},
        {QStringLiteral("displayName"), tmpl.name},
    });
  }

  QVariantMap section;
  section[QStringLiteral("title")] = QString();
  section[QStringLiteral("items")] = items;
  m_templateItems.append(section);

  // Select first template as default
  if (!items.isEmpty()) {
    m_selectedTemplate = items.first().toMap();
  }

  // Set up action panel with submit action
  auto panel = std::make_unique<FormActionPanelState>();
  auto actionSection = panel->createSection();
  auto submitAction =
      new StaticAction(QStringLiteral("Create extension"), ImageURL::builtin("enter-key"), [this]() { submit(); });
  actionSection->addAction(submitAction);
  setActions(std::move(panel));
}

void QmlCreateExtensionViewHost::submit() {
  auto toast = context()->services->toastService();

  // Clear previous errors
  m_authorError.clear();
  m_titleError.clear();
  m_descriptionError.clear();
  m_locationError.clear();
  m_commandTitleError.clear();
  m_commandSubtitleError.clear();
  m_commandDescriptionError.clear();

  bool valid = true;

  if (m_author.size() < 3) {
    m_authorError = QStringLiteral("Min. 3 chars");
    valid = false;
  }
  if (m_title.size() < 3) {
    m_titleError = QStringLiteral("Min. 3 chars");
    valid = false;
  }
  if (m_description.size() < 16) {
    m_descriptionError = QStringLiteral("Min. 16 chars");
    valid = false;
  }

  {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path path = expandPath(m_location.toStdString());
    if (!fs::is_directory(path, ec)) {
      m_locationError = QStringLiteral("Must exist");
      valid = false;
    }
  }

  if (m_commandTitle.size() < 3) {
    m_commandTitleError = QStringLiteral("Min. 3 chars");
    valid = false;
  }
  if (m_commandSubtitle.size() < 3) {
    m_commandSubtitleError = QStringLiteral("Min. 3 chars");
    valid = false;
  }
  if (m_commandDescription.size() < 3) {
    m_commandDescriptionError = QStringLiteral("Min. 3 chars");
    valid = false;
  }

  emit errorsChanged();

  if (!valid) {
    toast->failure("Form has errors");
    return;
  }

  ExtensionBoilerplateConfig cfg;
  cfg.author = m_author;
  cfg.title = m_title;
  cfg.description = m_description;

  ExtensionBoilerplateConfig::CommandConfig cmdCfg;
  cmdCfg.title = m_commandTitle;
  cmdCfg.subtitle = m_commandSubtitle;
  cmdCfg.description = m_commandDescription;
  cmdCfg.templateId = m_selectedTemplate[QStringLiteral("id")].toString();
  cfg.commands.emplace_back(std::move(cmdCfg));

  ExtensionBoilerplateGenerator gen;
  std::filesystem::path targetDir = expandPath(m_location.toStdString());

  auto v = gen.generate(targetDir, cfg);

  if (!v) {
    toast->failure("Failed to create extension");
    qCritical() << "Failed to create extension with error" << v.error();
    return;
  }

  auto successView = new CreateExtensionSuccessView(cfg, v.value());

  popSelf();
  context()->navigation->pushView(successView);
  context()->navigation->setNavigationIcon(ImageURL::emoji("🥳"));
  context()->navigation->setNavigationTitle("Extension created!");
}

void QmlCreateExtensionViewHost::selectTemplate(const QVariantMap &item) {
  m_selectedTemplate = item;
  emit formChanged();
}

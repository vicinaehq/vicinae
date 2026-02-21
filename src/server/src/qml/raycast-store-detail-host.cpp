#include "raycast-store-detail-host.hpp"
#include "actions/extension/extension-actions.hpp"
#include "navigation-controller.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "utils/utils.hpp"
#include <QFutureWatcher>

RaycastStoreDetailHost::RaycastStoreDetailHost(const Raycast::Extension &extension)
    : m_ext(extension) {}

QUrl RaycastStoreDetailHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreDetailView.qml"));
}

QVariantMap RaycastStoreDetailHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<RaycastStoreDetailHost *>(this))}};
}

void RaycastStoreDetailHost::initialize() {
  BaseView::initialize();

  auto registry = context()->services->extensionRegistry();
  m_isInstalled = registry->isInstalled(m_ext.id);

  // TODO: remove test alert — temporarily enabled on all extensions for visual testing
  m_alert = {
      {QStringLiteral("type"), QStringLiteral("warning")},
      {QStringLiteral("message"),
       QStringLiteral("This extension targets macOS-only platforms and may not work correctly on Linux.")},
  };

  createActions();

  connect(registry, &ExtensionRegistry::extensionAdded, this, [this](const QString &id) {
    if (id != m_ext.id) return;
    m_isInstalled = true;
    emit extensionChanged();
    createActions();
  });

  connect(registry, &ExtensionRegistry::extensionUninstalled, this, [this](const QString &id) {
    if (id != m_ext.id) return;
    m_isInstalled = false;
    emit extensionChanged();
    createActions();
  });
}

QString RaycastStoreDetailHost::title() const { return m_ext.title; }
QString RaycastStoreDetailHost::description() const { return m_ext.description; }

QString RaycastStoreDetailHost::iconSource() const {
  return qml::imageSourceFor(m_ext.themedIcon());
}

QString RaycastStoreDetailHost::authorName() const { return m_ext.author.name; }

QString RaycastStoreDetailHost::authorAvatar() const {
  if (m_ext.author.avatar.isEmpty()) return qml::imageSourceFor(ImageURL::builtin("person"));
  return QStringLiteral("image://vicinae/http:%1?mask=circle").arg(m_ext.author.avatar);
}

QString RaycastStoreDetailHost::downloadCount() const {
  return formatCount(m_ext.download_count);
}

QStringList RaycastStoreDetailHost::platforms() const { return m_ext.platforms; }
bool RaycastStoreDetailHost::isInstalled() const { return m_isInstalled; }
bool RaycastStoreDetailHost::hasScreenshots() const { return m_ext.metadata_count > 0; }

QStringList RaycastStoreDetailHost::screenshots() const {
  QStringList urls;
  for (const auto &url : m_ext.screenshots()) {
    urls.append(QStringLiteral("image://vicinae/http:%1").arg(url.toString()));
  }
  return urls;
}

QVariantList RaycastStoreDetailHost::commands() const {
  QVariantList list;
  for (const auto &cmd : m_ext.commands) {
    list.append(QVariantMap{
        {QStringLiteral("title"), cmd.title},
        {QStringLiteral("description"), cmd.description},
        {QStringLiteral("iconSource"), qml::imageSourceFor(cmd.themedIcon())},
    });
  }
  return list;
}

QString RaycastStoreDetailHost::readmeUrl() const { return m_ext.readme_assets_path; }
QString RaycastStoreDetailHost::sourceUrl() const { return m_ext.source_url; }

QString RaycastStoreDetailHost::lastUpdate() const {
  return getRelativeTimeString(m_ext.updatedAtDateTime());
}

QVariantList RaycastStoreDetailHost::contributors() const {
  QVariantList list;
  for (const auto &user : m_ext.contributors) {
    QString avatar;
    if (user.avatar.isEmpty())
      avatar = qml::imageSourceFor(ImageURL::builtin("person"));
    else
      avatar = QStringLiteral("image://vicinae/http:%1?mask=circle").arg(user.avatar);

    list.append(QVariantMap{
        {QStringLiteral("name"), user.name},
        {QStringLiteral("avatar"), avatar},
    });
  }
  return list;
}

QStringList RaycastStoreDetailHost::categories() const { return {}; }
QVariantMap RaycastStoreDetailHost::alert() const { return m_alert; }

void RaycastStoreDetailHost::openUrl(const QString &url) {
  ServiceRegistry::instance()->appDb()->openTarget(url);
}

void RaycastStoreDetailHost::openScreenshot(int index) {
  auto urls = m_ext.screenshots();
  if (index < 0 || index >= static_cast<int>(urls.size())) return;

  ServiceRegistry::instance()->appDb()->openTarget(urls[index].toString());
}

QString RaycastStoreDetailHost::initialNavigationTitle() const {
  return QString("%1 - %2").arg(BaseView::initialNavigationTitle()).arg(m_ext.title);
}

void RaycastStoreDetailHost::createActions() {
  auto panel = std::make_unique<FormActionPanelState>();
  auto registry = context()->services->extensionRegistry();
  auto main = panel->createSection();

  if (!m_isInstalled) {
    auto install = new StaticAction(
        "Install extension", m_ext.themedIcon(), [ext = m_ext](const ApplicationContext *ctx) {
          using Watcher = QFutureWatcher<Raycast::DownloadExtensionResult>;
          auto store = ctx->services->raycastStore();
          auto watcher = new Watcher;
          auto toast = ctx->services->toastService();
          auto registry = ctx->services->extensionRegistry();

          toast->dynamic("Downloading extension...");

          QObject::connect(watcher, &Watcher::finished, [ctx, registry, toast, ext, watcher]() {
            auto result = watcher->result();
            watcher->deleteLater();

            if (!result) {
              toast->failure("Failed to download extension");
              return;
            }

            registry->installFromZip(QString("store.raycast.%1").arg(ext.name), result->toStdString(),
                                     [toast](bool ok) {
                                       if (!ok) {
                                         toast->failure("Failed to extract extension archive");
                                         return;
                                       }
                                       toast->success("Extension installed");
                                     });
          });

          auto downloadResult = store->downloadExtension(ext.download_url);
          watcher->setFuture(downloadResult);
        });
    main->addAction(install);
  } else {
    auto uninstall = new UninstallExtensionAction(m_ext.id);
    main->addAction(uninstall);
  }

  setActions(std::move(panel));
}

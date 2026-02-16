#include "qml-vicinae-store-detail-host.hpp"
#include "actions/extension/extension-actions.hpp"
#include "navigation-controller.hpp"
#include "qml-utils.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "utils/utils.hpp"
#include <QFutureWatcher>

QmlVicinaeStoreDetailHost::QmlVicinaeStoreDetailHost(const VicinaeStore::Extension &extension)
    : m_ext(extension) {}

QUrl QmlVicinaeStoreDetailHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreDetailView.qml"));
}

QVariantMap QmlVicinaeStoreDetailHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlVicinaeStoreDetailHost *>(this))}};
}

void QmlVicinaeStoreDetailHost::initialize() {
  BaseView::initialize();

  auto registry = context()->services->extensionRegistry();
  m_isInstalled = registry->isInstalled(m_ext.id);

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

QString QmlVicinaeStoreDetailHost::title() const { return m_ext.title; }
QString QmlVicinaeStoreDetailHost::description() const { return m_ext.description; }

QString QmlVicinaeStoreDetailHost::iconSource() const {
  return qml::imageSourceFor(ImageURL::http(m_ext.themedIcon()));
}

QString QmlVicinaeStoreDetailHost::authorName() const { return m_ext.author.name; }

QString QmlVicinaeStoreDetailHost::authorAvatar() const {
  if (m_ext.author.avatarUrl.isEmpty()) return qml::imageSourceFor(ImageURL::builtin("person"));
  return QStringLiteral("image://vicinae/http:%1?mask=circle").arg(m_ext.author.avatarUrl);
}

QString QmlVicinaeStoreDetailHost::downloadCount() const {
  return formatCount(m_ext.downloadCount);
}

QStringList QmlVicinaeStoreDetailHost::platforms() const {
  QStringList list;
  for (const auto &p : m_ext.platforms) list.append(p);
  return list;
}

bool QmlVicinaeStoreDetailHost::isInstalled() const { return m_isInstalled; }
bool QmlVicinaeStoreDetailHost::hasScreenshots() const { return false; }
QStringList QmlVicinaeStoreDetailHost::screenshots() const { return {}; }

QVariantList QmlVicinaeStoreDetailHost::commands() const {
  QVariantList list;
  for (const auto &cmd : m_ext.commands) {
    auto iconStr = cmd.themedIcon().has_value()
                       ? qml::imageSourceFor(ImageURL::http(cmd.themedIcon().value()))
                       : qml::imageSourceFor(ImageURL::http(m_ext.themedIcon()));
    list.append(QVariantMap{
        {QStringLiteral("title"), cmd.title},
        {QStringLiteral("description"), cmd.description},
        {QStringLiteral("iconSource"), iconStr},
    });
  }
  return list;
}

QString QmlVicinaeStoreDetailHost::readmeUrl() const { return m_ext.readmeUrl; }
QString QmlVicinaeStoreDetailHost::sourceUrl() const { return m_ext.sourceUrl; }

QString QmlVicinaeStoreDetailHost::lastUpdate() const {
  return getRelativeTimeString(m_ext.updatedAt);
}

QVariantList QmlVicinaeStoreDetailHost::contributors() const { return {}; }

QStringList QmlVicinaeStoreDetailHost::categories() const {
  QStringList list;
  for (const auto &cat : m_ext.categories) list.append(cat.name);
  return list;
}

// TODO: remove test alert — temporarily enabled on all extensions for visual testing
QVariantMap QmlVicinaeStoreDetailHost::alert() const {
  return {
      {QStringLiteral("type"), QStringLiteral("warning")},
      {QStringLiteral("message"),
       QStringLiteral("This extension targets macOS-only platforms and may not work correctly on Linux.")},
  };
}

void QmlVicinaeStoreDetailHost::openUrl(const QString &url) {
  ServiceRegistry::instance()->appDb()->openTarget(url);
}

QString QmlVicinaeStoreDetailHost::initialNavigationTitle() const {
  return QString("%1 - %2").arg(BaseView::initialNavigationTitle()).arg(m_ext.title);
}

void QmlVicinaeStoreDetailHost::createActions() {
  auto panel = std::make_unique<FormActionPanelState>();
  auto main = panel->createSection();

  if (!m_isInstalled) {
    auto install = new StaticAction(
        "Install extension", ImageURL::http(m_ext.themedIcon()),
        [ext = m_ext](const ApplicationContext *ctx) {
          using Watcher = QFutureWatcher<VicinaeStore::DownloadExtensionResult>;
          auto store = ctx->services->vicinaeStore();
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

            registry->installFromZip(QString("store.vicinae.%1").arg(ext.name), result->toStdString(),
                                     [toast](bool ok) {
                                       if (!ok) {
                                         toast->failure("Failed to extract extension archive");
                                         return;
                                       }
                                       toast->success("Extension installed");
                                     });
          });

          auto downloadResult = store->downloadExtension(ext.downloadUrl);
          watcher->setFuture(downloadResult);
        });
    main->addAction(install);
  } else {
    auto uninstall = new UninstallExtensionAction(m_ext.id);
    main->addAction(uninstall);
  }

  setActions(std::move(panel));
}

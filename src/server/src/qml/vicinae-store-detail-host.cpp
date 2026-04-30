#include "vicinae-store-detail-host.hpp"
#include "actions/extension/extension-actions.hpp"
#include "empty-view-host.hpp"
#include "navigation-controller.hpp"
#include "vicinae.hpp"
#include "view-utils.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "utils/utils.hpp"
#include <QFutureWatcher>

VicinaeStoreDetailHost::VicinaeStoreDetailHost(const QString &authorHandle, const QString &extensionName)
    : m_authorHandle(authorHandle), m_extensionName(extensionName) {}

QUrl VicinaeStoreDetailHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreDetailView.qml"));
}

QVariantMap VicinaeStoreDetailHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void VicinaeStoreDetailHost::initialize() {
  BaseView::initialize();

  setLoading(true);
  auto store = context()->services->vicinaeStore();
  auto *watcher = new QFutureWatcher<VicinaeStore::ListResult>(this);
  watcher->setFuture(store->fetchAll());

  connect(watcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this, [this, watcher]() {
    watcher->deleteLater();
    auto result = watcher->result();
    if (!result) {
      context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
      setLoading(false);
      return;
    }

    auto it = std::ranges::find_if(result->extensions, [&](const auto &ext) {
      return ext.author.handle == m_authorHandle && ext.name == m_extensionName;
    });
    if (it == result->extensions.end()) {
      auto id = QString("%1/%2").arg(m_authorHandle, m_extensionName);
      context()->navigation->replaceView(new EmptyViewHost(
          "Extension not found", QString("The extension \"%1\" could not be found in the store.").arg(id),
          ImageURL(BuiltinIcon::MagnifyingGlass).setFill(SemanticColor::Red)));
      return;
    }

    hydrate(*it);
    setLoading(false);
  });
}

void VicinaeStoreDetailHost::hydrate(const VicinaeStore::Extension &extension) {
  m_ext = extension;
  m_isReady = true;

  auto registry = context()->services->extensionRegistry();
  m_isInstalled = registry->isInstalled(m_ext.id);

  auto icon = ImageURL::builtin("cart");
  icon.setBackgroundTint(Omnicast::ACCENT_COLOR);
  setNavigationIcon(icon);
  setNavigationTitle(QString("Extension Store - %1").arg(m_ext.title));
  createActions();
  emit extensionChanged();

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

QString VicinaeStoreDetailHost::title() const { return m_ext.title; }
QString VicinaeStoreDetailHost::description() const { return m_ext.description; }

QString VicinaeStoreDetailHost::iconSource() const { return qml::imageSourceFor(m_ext.themedIcon()); }

QString VicinaeStoreDetailHost::authorName() const { return m_ext.author.name; }

QString VicinaeStoreDetailHost::authorAvatar() const {
  if (m_ext.author.avatarUrl.isEmpty()) return qml::imageSourceFor(ImageURL::builtin("person"));
  return qml::imageSourceFor(ImageURL::http(QUrl(m_ext.author.avatarUrl)).circle());
}

QString VicinaeStoreDetailHost::downloadCount() const { return formatCount(m_ext.downloadCount); }

QStringList VicinaeStoreDetailHost::platforms() const {
  return QStringList(m_ext.platforms.begin(), m_ext.platforms.end());
}

bool VicinaeStoreDetailHost::isReady() const { return m_isReady; }
bool VicinaeStoreDetailHost::isInstalled() const { return m_isInstalled; }
bool VicinaeStoreDetailHost::hasScreenshots() const { return false; }
QStringList VicinaeStoreDetailHost::screenshots() const { return {}; }

QVariantList VicinaeStoreDetailHost::commands() const {
  QVariantList list;
  for (const auto &cmd : m_ext.commands) {
    auto iconStr = qml::imageSourceFor(cmd.themedIcon().value_or(m_ext.themedIcon()));
    list.append(QVariantMap{
        {QStringLiteral("title"), cmd.title},
        {QStringLiteral("description"), cmd.description},
        {QStringLiteral("iconSource"), iconStr},
    });
  }
  return list;
}

QString VicinaeStoreDetailHost::readmeUrl() const { return m_ext.readmeUrl; }
QString VicinaeStoreDetailHost::sourceUrl() const { return m_ext.sourceUrl; }

QString VicinaeStoreDetailHost::lastUpdate() const { return getRelativeTimeString(m_ext.updatedAt); }

QVariantList VicinaeStoreDetailHost::contributors() const { return {}; }

QStringList VicinaeStoreDetailHost::categories() const {
  QStringList list;
  for (const auto &cat : m_ext.categories)
    list.append(cat.name);
  return list;
}

void VicinaeStoreDetailHost::openUrl(const QString &url) {
  ServiceRegistry::instance()->appDb()->openTarget(url);
}

QString VicinaeStoreDetailHost::initialNavigationTitle() const { return QStringLiteral("Extension Store"); }

void VicinaeStoreDetailHost::createActions() {
  auto panel = std::make_unique<FormActionPanelState>();
  auto main = panel->createSection();

  if (!m_isInstalled) {
    auto install = new StaticAction(
        "Install extension", m_ext.themedIcon(), [ext = m_ext](const ApplicationContext *ctx) {
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

  auto reportIssue =
      new StaticAction("Report issue", ImageURL::builtin("bug"), [](const ApplicationContext *ctx) {
        ctx->services->appDb()->openTarget(Omnicast::GH_EXTENSIONS_CREATE_ISSUE);
      });
  main->addAction(reportIssue);

  setActions(std::move(panel));
}

#include "vicinae-store-view-host.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "utils/utils.hpp"

QHash<int, QByteArray> VicinaeStoreSectionListModel::roleNames() const {
  auto roles = SectionListModel::roleNames();
  roles[DownloadCount] = "downloadCount";
  roles[AuthorAvatar] = "authorAvatar";
  roles[IsInstalled] = "isInstalled";
  roles[CompatTierRole] = "compatTier";
  return roles;
}

QVariant VicinaeStoreSectionListModel::data(const QModelIndex &index, int role) const {
  if (role >= DownloadCount) {
    int sourceIdx, itemIdx;
    if (!dataItemAt(index.row(), sourceIdx, itemIdx)) return {};
    const auto &entry = m_section->resolvedEntry(itemIdx);
    switch (role) {
    case DownloadCount:
      return formatCount(entry.extension.downloadCount);
    case AuthorAvatar: {
      const auto &avatar = entry.extension.author.avatarUrl;
      if (avatar.isEmpty()) return m_section->imageSourceFor(ImageURL::builtin("person"));
      return m_section->imageSourceFor(ImageURL::http(QUrl(avatar)).circle());
    }
    case IsInstalled:
      return entry.installed;
    case CompatTierRole:
      return -1;
    default:
      return {};
    }
  }
  return SectionListModel::data(index, role);
}

VicinaeStoreViewHost::VicinaeStoreViewHost() {
  connect(&m_watcher, &QFutureWatcher<VicinaeStore::ListResult>::finished, this,
          &VicinaeStoreViewHost::handleFinished);
}

QUrl VicinaeStoreViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreListingView.qml"));
}

QVariantMap VicinaeStoreViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void VicinaeStoreViewHost::initialize() {
  BaseView::initialize();

  m_model.setScope(ViewScope(context(), this));
  m_model.setSection(&m_section);
  m_model.addSource(&m_section);

  m_store = context()->services->vicinaeStore();

  setSearchPlaceholderText("Browse Vicinae extensions");

  connect(context()->services->extensionRegistry(), &ExtensionRegistry::extensionsChanged, this,
          &VicinaeStoreViewHost::refresh);
}

void VicinaeStoreViewHost::loadInitialData() { fetchExtensions(); }

void VicinaeStoreViewHost::textChanged(const QString &text) { m_model.setFilter(text); }

void VicinaeStoreViewHost::onReactivated() { m_model.refreshActionPanel(); }

void VicinaeStoreViewHost::fetchExtensions() {
  setLoading(true);
  m_watcher.setFuture(m_store->fetchAll());
}

void VicinaeStoreViewHost::handleFinished() {
  auto result = m_watcher.result();
  setLoading(false);

  if (!result) {
    qWarning() << "[VicinaeStore] fetch error:" << result.error();
    context()->services->toastService()->setToast("Failed to fetch extensions", ToastStyle::Danger);
    return;
  }

  m_section.setEntries(result->extensions, context()->services->extensionRegistry(),
                       QStringLiteral("Extensions"));
}

void VicinaeStoreViewHost::refresh() {
  if (auto *cached = m_store->cached()) {
    m_section.setEntries(*cached, context()->services->extensionRegistry(), QStringLiteral("Extensions"));
  } else {
    fetchExtensions();
  }
}

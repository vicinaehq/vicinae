#include "raycast-store.hpp"
#include "lib/glaze-qt.hpp"

RaycastStoreService::RaycastStoreService() {
  m_client.setBaseUrl(QStringLiteral("https://backend.raycast.com/api/v1"));
}

const http::RequestOptions RaycastStoreService::s_requestOpts = {
    .cachePolicy = QNetworkRequest::PreferCache,
    .allowHttp2 = false,
    .contentType = QStringLiteral("application/json"),
};

void RaycastStoreService::postProcess(std::vector<Raycast::Extension> &extensions) {
  for (auto &ext : extensions) {
    ext.id = QString("store.raycast.%1").arg(ext.name);
    for (auto &cmd : ext.commands) {
      cmd.extensionIcons = ext.icons;
    }
  }
}

QFuture<Raycast::ListResult> RaycastStoreService::search(const QString &query) {
  auto url = QString("/store_listings/search?q=%1").arg(query);

  return m_client.get<Raycast::ListApiResponse>(url, s_requestOpts)
      .then([](http::Client::Result<Raycast::ListApiResponse> result) -> Raycast::ListResult {
        if (!result) return std::unexpected(result.error());
        auto extensions = std::move(result->data);
        postProcess(extensions);
        return Raycast::ListResponse{std::move(extensions)};
      });
}

QFuture<Raycast::DownloadExtensionResult> RaycastStoreService::downloadExtension(const QUrl &url) {
  return m_client.getRaw(url.toString(), s_requestOpts)
      .then([](http::Client::Result<QByteArray> result) -> Raycast::DownloadExtensionResult {
        if (!result) return std::unexpected(result.error());
        return *std::move(result);
      });
}

QFuture<Raycast::ListResult>
RaycastStoreService::fetchExtensions(const Raycast::ListPaginationOptions &opts) {
  if (auto it = m_cachedPages.find(opts.page); it != m_cachedPages.end()) {
    return QtFuture::makeReadyValueFuture<Raycast::ListResult>(it->second);
  }

  auto url = QString("/store_listings?page=%1&per_page=%2").arg(opts.page).arg(opts.perPage);

  return m_client.get<Raycast::ListApiResponse>(url, s_requestOpts)
      .then([this,
             page = opts.page](http::Client::Result<Raycast::ListApiResponse> result) -> Raycast::ListResult {
        if (!result) return std::unexpected(result.error());
        auto extensions = std::move(result->data);
        postProcess(extensions);
        Raycast::ListResponse response{std::move(extensions)};
        m_cachedPages.insert({page, response});
        return response;
      });
}

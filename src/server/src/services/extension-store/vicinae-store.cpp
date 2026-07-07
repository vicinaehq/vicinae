#include "vicinae-store.hpp"
#include "generated/version.h"
#include "environment.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "utils/capabilities.hpp"
#include <algorithm>

namespace VicinaeStore {

std::optional<ImageURL> Icons::themedIcon() const {
  const auto &theme = ThemeService::instance().theme();
  if (theme.variant() == ThemeVariant::Dark && dark) return ImageURL::http(*dark);
  if (light) return ImageURL::http(*light);
  if (dark) return ImageURL::http(*dark);
  return {};
}

std::optional<ImageURL> Command::themedIcon() const { return icons.themedIcon(); }

ImageURL Extension::themedIcon() const {
  if (auto icon = icons.themedIcon()) return *icon;
  return ImageURL::builtin("plug");
}

} // namespace VicinaeStore

static bool availableOnCurrentPlatform(const VicinaeStore::Extension &ext) {
  return ext.platforms.empty() || std::ranges::contains(ext.platforms, platform::extensionPlatform(),
                                                        [](const QString &p) { return p.toLower(); });
}

static void postProcess(VicinaeStore::ListResponse &response) {
  std::erase_if(response.extensions, [](const auto &ext) { return !availableOnCurrentPlatform(ext); });
  for (auto &ext : response.extensions) {
    ext.id = QString("store.vicinae.%1").arg(ext.name);
  }
}

VicinaeStoreService::VicinaeStoreService(QObject *parent) : QObject(parent) {
  m_client.setBaseUrl(Environment::vicinaeApiBaseUrl());
}

void VicinaeStoreService::setBaseUrl(const QString &url) { m_client.setBaseUrl(url); }

QString VicinaeStoreService::baseUrl() const { return m_client.makeUrl({}).toString(); }

const http::RequestOptions VicinaeStoreService::s_requestOpts = {
    .cachePolicy = QNetworkRequest::PreferCache,
    .userAgent = QString("vicinae/%1").arg(VICINAE_GIT_TAG),
};

QFuture<VicinaeStore::ListResult>
VicinaeStoreService::fetchExtensions(const VicinaeStore::ListPaginationOptions &opts) {
  auto url = QString("/store/list?page=%1&limit=%2").arg(opts.page).arg(opts.limit);

  return m_client.get<VicinaeStore::ListResponse>(url, s_requestOpts)
      .then([](http::Client::Result<VicinaeStore::ListResponse> result) -> VicinaeStore::ListResult {
        if (!result) return std::unexpected(result.error());
        postProcess(*result);
        return *std::move(result);
      });
}

QFuture<VicinaeStore::ListResult> VicinaeStoreService::fetchAll() {
  return fetchExtensions({.limit = 500})
      .then(this, [](VicinaeStore::ListResult result) -> VicinaeStore::ListResult { return result; });
}

QFuture<VicinaeStore::ListResult> VicinaeStoreService::search(const QString &query) {
  auto url = QString("/store/search?q=%1").arg(query);

  return m_client.get<VicinaeStore::ListResponse>(url, s_requestOpts)
      .then([](http::Client::Result<VicinaeStore::ListResponse> result) -> VicinaeStore::ListResult {
        if (!result) return std::unexpected(result.error());
        postProcess(*result);
        return *std::move(result);
      });
}

QFuture<VicinaeStore::DownloadExtensionResult> VicinaeStoreService::downloadExtension(const QUrl &url) {
  return m_client.getRaw(url.toString(), s_requestOpts)
      .then([](http::Client::Result<QByteArray> result) -> VicinaeStore::DownloadExtensionResult {
        if (!result) return std::unexpected(result.error());
        return *std::move(result);
      });
}

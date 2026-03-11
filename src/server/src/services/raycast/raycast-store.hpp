#pragma once
#include "common/types.hpp"
#include "ui/image/url.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <QDateTime>
#include <QObject>
#include <QString>
#include <expected>
#include <unordered_map>
#include <vector>
#include "lib/http-client.hpp"

namespace Raycast {

struct Icons {
  std::optional<QString> light;
  std::optional<QString> dark;

  std::optional<ImageURL> themedIcon() const {
    const auto &theme = ThemeService::instance().theme();
    if (theme.variant() == ThemeVariant::Dark && dark) return ImageURL::http(*dark);
    if (theme.variant() == ThemeVariant::Light && light) return ImageURL::http(*light);
    if (light) return ImageURL::http(*light);
    if (dark) return ImageURL::http(*dark);
    return {};
  }
};

struct User {
  QString name;
  QString handle;
  QString bio;
  QString twitter_handle;
  QString github_handle;
  QString location;
  QString initials;
  QString avatar_placeholder_color;
  QString slack_community_username;
  QString slack_community_user_id;
  QString website_anchor;
  qint64 created_at = 0;
  QString website;
  QString username;
  std::optional<QString> avatar;

  ImageURL validUserIcon() const {
    if (!avatar) return ImageURL::builtin("person").setBackgroundTint(SemanticColor::Blue);
    return ImageURL::http(*avatar);
  }

  QDateTime createdAtDateTime() const { return QDateTime::fromSecsSinceEpoch(created_at); }
};

struct Command {
  QString id;
  QString name;
  QString title;
  QString subtitle;
  QString description;
  std::vector<QString> keywords;
  QString mode;
  bool disabled_by_default = false;
  bool beta = false;
  Icons icons;
  Icons extensionIcons;

  ImageURL themedIcon() const {
    if (auto icon = icons.themedIcon()) return *icon;
    if (auto icon = extensionIcons.themedIcon()) return *icon;
    return ImageURL::builtin("puzzle-piece");
  }
};

struct Extension {
  QString id;
  QString name;
  std::optional<QString> native_id;
  std::optional<std::vector<QString>> seo_categories;
  std::optional<std::vector<QString>> platforms;
  qint64 created_at = 0;
  User author;
  User owner;
  QString status;
  bool is_new = false;
  QString access;
  QString store_url;
  qint32 download_count = 0;
  std::optional<qint64> kill_listed_at;
  QString title;
  QString description;
  QString commit_sha;
  QString relative_path;
  QString api_version;
  std::vector<QString> categories;
  std::vector<QString> prompt_examples;
  qint32 metadata_count = 0;
  qint64 updated_at = 0;
  QString source_url;
  QString readme_url;
  QString readme_assets_path;
  Icons icons;
  QString download_url;
  std::vector<Command> commands;
  std::vector<User> contributors;

  ImageURL themedIcon() const {
    if (auto icon = icons.themedIcon()) return *icon;
    return ImageURL::builtin("puzzle-piece");
  }

  QDateTime createdAtDateTime() const { return QDateTime::fromSecsSinceEpoch(created_at); }
  QDateTime updatedAtDateTime() const { return QDateTime::fromSecsSinceEpoch(updated_at); }

  QDateTime killListedAtDateTime() const {
    return kill_listed_at ? QDateTime::fromSecsSinceEpoch(*kill_listed_at) : QDateTime();
  }

  std::vector<QUrl> screenshots() const {
    std::vector<QUrl> urls;
    urls.reserve(metadata_count);
    for (int i = 0; i < metadata_count; ++i) {
      urls.emplace_back(QString("%1metadata/%2-%3.png").arg(readme_assets_path).arg(name).arg(i + 1));
    }
    return urls;
  }

  bool isKillListed() const { return kill_listed_at.has_value(); }
};

struct ListApiResponse {
  std::vector<Extension> data;
};

struct ListResponse {
  std::vector<Extension> extensions;
};

struct ListPaginationOptions {
  int page = 1;
  int perPage = 50;
};

using ListResult = std::expected<ListResponse, std::string>;
using DownloadExtensionResult = std::expected<QByteArray, std::string>;

} // namespace Raycast

class RaycastStoreService : public QObject, NonCopyable {
public:
  RaycastStoreService();

  QFuture<Raycast::DownloadExtensionResult> downloadExtension(const QUrl &url);
  QFuture<Raycast::ListResult> fetchExtensions(const Raycast::ListPaginationOptions &opts = {});
  QFuture<Raycast::ListResult> search(const QString &query);

private:
  static const http::RequestOptions s_requestOpts;
  static void postProcess(std::vector<Raycast::Extension> &extensions);

  std::unordered_map<int, Raycast::ListResponse> m_cachedPages;
  http::Client m_client;
};

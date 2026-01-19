#pragma once
#include "common.hpp"
#include "common/types.hpp"
#include "ui/image/url.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include <expected>
#include <qcontainerfwd.h>
#include <qstringview.h>
#include <vector>
#include <qfuture.h>
#include <qnetworkaccessmanager.h>
#include <qobject.h>
#include <QString>
#include <QStringList>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QDateTime>
#include <QVariant>
#include <qobjectdefs.h>

namespace Raycast {

struct User;
struct Command;
struct Icons;
struct Extension;

struct Icons {
  QString light;
  QString dark;

  // Convert from QJsonObject
  static Icons fromJson(const QJsonObject &json) {
    Icons icons;
    icons.light = json["light"].toString();
    icons.dark = json["dark"].toString();
    return icons;
  }

  // Convert to QJsonObject
  QJsonObject toJson() const {
    QJsonObject json;
    if (!light.isEmpty()) json["light"] = light;
    if (!dark.isEmpty()) json["dark"] = dark;
    return json;
  }
};

// User structure (used for author, owner, and contributors)
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
  qint64 created_at;
  QString website;
  QString username;
  QString avatar;

  // Convert from QJsonObject
  static User fromJson(const QJsonObject &json) {
    User user;
    user.name = json["name"].toString();
    user.handle = json["handle"].toString();
    user.bio = json["bio"].toString();
    user.twitter_handle = json["twitter_handle"].toString();
    user.github_handle = json["github_handle"].toString();
    user.location = json["location"].toString();
    user.initials = json["initials"].toString();
    user.avatar_placeholder_color = json["avatar_placeholder_color"].toString();
    user.slack_community_username = json["slack_community_username"].toString();
    user.slack_community_user_id = json["slack_community_user_id"].toString();
    user.website_anchor = json["website_anchor"].toString();
    user.created_at = json["created_at"].toVariant().toLongLong();
    user.website = json["website"].toString();
    user.username = json["username"].toString();
    user.avatar = json["avatar"].toString();
    return user;
  }

  // Convert to QJsonObject
  QJsonObject toJson() const {
    QJsonObject json;
    json["name"] = name;
    json["handle"] = handle;
    json["bio"] = bio;
    if (!twitter_handle.isEmpty()) json["twitter_handle"] = twitter_handle;
    if (!github_handle.isEmpty()) json["github_handle"] = github_handle;
    if (!location.isEmpty()) json["location"] = location;
    json["initials"] = initials;
    json["avatar_placeholder_color"] = avatar_placeholder_color;
    if (!slack_community_username.isEmpty()) json["slack_community_username"] = slack_community_username;
    if (!slack_community_user_id.isEmpty()) json["slack_community_user_id"] = slack_community_user_id;
    if (!website_anchor.isEmpty()) json["website_anchor"] = website_anchor;
    json["created_at"] = created_at;
    if (!website.isEmpty()) json["website"] = website;
    json["username"] = username;
    if (!avatar.isEmpty()) json["avatar"] = avatar;
    return json;
  }

  // Return user icon or generic user icon
  ImageURL validUserIcon() const {
    if (avatar.isEmpty()) return ImageURL::builtin("person").setBackgroundTint(SemanticColor::Blue);

    return ImageURL::http(avatar);
  }

  // Get as QDateTime for convenience
  QDateTime createdAtDateTime() const { return QDateTime::fromSecsSinceEpoch(created_at); }
};

// Command structure
struct Command {
  QString id;
  QString name;
  QString title;
  QString subtitle;
  QString description;
  QStringList keywords;
  QString mode;
  bool disabled_by_default;
  bool beta;
  Icons icons;
  Icons extensionIcons;

  ImageURL extensionThemedIcon() const {
    switch (ThemeService::instance().theme().variant()) {
    case ThemeVariant::Light:
      if (!extensionIcons.light.isEmpty()) { return ImageURL::http(extensionIcons.light); }
      break;
    case ThemeVariant::Dark:
      if (!extensionIcons.dark.isEmpty()) { return ImageURL::http(extensionIcons.dark); }
      break;
    }

    if (!extensionIcons.light.isEmpty()) { return ImageURL::http(extensionIcons.light); }
    if (!extensionIcons.dark.isEmpty()) { return ImageURL::http(extensionIcons.dark); }

    return ImageURL::http(extensionIcons.dark);
  }

  ImageURL themedIcon() const {
    switch (ThemeService::instance().theme().variant()) {
    case ThemeVariant::Light:
      if (!icons.light.isEmpty()) { return ImageURL::http(icons.light); }
      break;
    case ThemeVariant::Dark:
      if (!icons.dark.isEmpty()) { return ImageURL::http(icons.dark); }
      break;
    }

    if (!icons.light.isEmpty()) { return ImageURL::http(icons.light); }
    if (!icons.dark.isEmpty()) { return ImageURL::http(icons.dark); }

    return extensionThemedIcon();
  }

  // Convert from QJsonObject
  static Command fromJson(Icons extensionIcons, const QJsonObject &json) {
    Command command;
    command.extensionIcons = extensionIcons;
    command.id = json["id"].toString();
    command.name = json["name"].toString();
    command.title = json["title"].toString();
    command.subtitle = json["subtitle"].toString();
    command.description = json["description"].toString();

    // Handle keywords array
    QJsonArray keywordsArray = json["keywords"].toArray();
    for (const QJsonValue &value : keywordsArray) {
      command.keywords.append(value.toString());
    }

    command.mode = json["mode"].toString();
    command.disabled_by_default = json["disabled_by_default"].toBool();
    command.beta = json["beta"].toBool();
    command.icons = Icons::fromJson(json["icons"].toObject());
    return command;
  }

  // Convert to QJsonObject
  QJsonObject toJson() const {
    QJsonObject json;
    json["id"] = id;
    json["name"] = name;
    json["title"] = title;
    json["subtitle"] = subtitle;
    json["description"] = description;

    QJsonArray keywordsArray;
    for (const QString &keyword : keywords) {
      keywordsArray.append(keyword);
    }
    json["keywords"] = keywordsArray;

    json["mode"] = mode;
    json["disabled_by_default"] = disabled_by_default;
    json["beta"] = beta;
    json["icons"] = icons.toJson();
    return json;
  }
};

// Main extension/application structure
struct Extension {
  QString id;
  QString storeId;
  QString name;
  QString native_id;
  QStringList seo_categories;
  QStringList platforms;
  qint64 created_at;
  User author;
  User owner;
  QString status;
  bool is_new;
  QString access;
  QString store_url;
  qint32 download_count;
  qint64 kill_listed_at;
  QString title;
  QString description;
  QString commit_sha;
  QString relative_path;
  QString api_version;
  QStringList categories;
  QStringList prompt_examples;
  qint32 metadata_count;
  qint64 updated_at;
  QString source_url;
  QString readme_url;
  QString readme_assets_path;
  Icons icons;
  QString download_url;
  QList<Command> commands;
  QList<User> contributors;
  QStringList tools;

  // Convert from QJsonObject
  static Extension fromJson(const QJsonObject &json) {
    Extension ext;
    ext.storeId = json["id"].toString();
    ext.name = json["name"].toString();
    ext.native_id = json["native_id"].toString();
    ext.id = QString("store.raycast.%1").arg(ext.name);

    // Handle seo_categories array
    QJsonArray seoCategoriesArray = json["seo_categories"].toArray();
    for (const QJsonValue &value : seoCategoriesArray) {
      ext.seo_categories.append(value.toString());
    }

    // Handle platforms array (can be null)
    if (!json["platforms"].isNull()) {
      QJsonArray platformsArray = json["platforms"].toArray();
      for (const QJsonValue &value : platformsArray) {
        ext.platforms.append(value.toString());
      }
    }

    ext.created_at = json["created_at"].toVariant().toLongLong();
    ext.author = User::fromJson(json["author"].toObject());
    ext.owner = User::fromJson(json["owner"].toObject());
    ext.status = json["status"].toString();
    ext.is_new = json["is_new"].toBool();
    ext.access = json["access"].toString();
    ext.store_url = json["store_url"].toString();
    ext.download_count = json["download_count"].toInt();

    // Handle kill_listed_at (can be null)
    if (!json["kill_listed_at"].isNull()) {
      ext.kill_listed_at = json["kill_listed_at"].toVariant().toLongLong();
    } else {
      ext.kill_listed_at = 0; // Use 0 to indicate null
    }

    ext.title = json["title"].toString();
    ext.description = json["description"].toString();
    ext.commit_sha = json["commit_sha"].toString();
    ext.relative_path = json["relative_path"].toString();
    ext.api_version = json["api_version"].toString();

    // Handle categories array
    QJsonArray categoriesArray = json["categories"].toArray();
    for (const QJsonValue &value : categoriesArray) {
      ext.categories.append(value.toString());
    }

    // Handle prompt_examples array
    QJsonArray promptExamplesArray = json["prompt_examples"].toArray();
    for (const QJsonValue &value : promptExamplesArray) {
      ext.prompt_examples.append(value.toString());
    }

    ext.metadata_count = json["metadata_count"].toInt();
    ext.updated_at = json["updated_at"].toVariant().toLongLong();
    ext.source_url = json["source_url"].toString();
    ext.readme_url = json["readme_url"].toString();
    ext.readme_assets_path = json["readme_assets_path"].toString();
    ext.icons = Icons::fromJson(json["icons"].toObject());
    ext.download_url = json["download_url"].toString();

    // Handle commands array
    QJsonArray commandsArray = json["commands"].toArray();
    for (const QJsonValue &value : commandsArray) {
      ext.commands.append(Command::fromJson(ext.icons, value.toObject()));
    }

    // Handle contributors array
    QJsonArray contributorsArray = json["contributors"].toArray();
    for (const QJsonValue &value : contributorsArray) {
      ext.contributors.append(User::fromJson(value.toObject()));
    }

    // Handle tools array
    QJsonArray toolsArray = json["tools"].toArray();
    for (const QJsonValue &value : toolsArray) {
      ext.tools.append(value.toString());
    }

    return ext;
  }

  // Convert to QJsonObject
  QJsonObject toJson() const {
    QJsonObject json;
    json["id"] = id;
    json["name"] = name;
    if (!native_id.isEmpty()) json["native_id"] = native_id;

    QJsonArray seoCategoriesArray;
    for (const QString &category : seo_categories) {
      seoCategoriesArray.append(category);
    }
    json["seo_categories"] = seoCategoriesArray;

    if (!platforms.isEmpty()) {
      QJsonArray platformsArray;
      for (const QString &platform : platforms) {
        platformsArray.append(platform);
      }
      json["platforms"] = platformsArray;
    }

    json["created_at"] = created_at;
    json["author"] = author.toJson();
    json["owner"] = owner.toJson();
    json["status"] = status;
    json["is_new"] = is_new;
    json["access"] = access;
    json["store_url"] = store_url;
    json["download_count"] = download_count;

    if (kill_listed_at != 0) { json["kill_listed_at"] = kill_listed_at; }

    json["title"] = title;
    json["description"] = description;
    json["commit_sha"] = commit_sha;
    json["relative_path"] = relative_path;
    json["api_version"] = api_version;

    QJsonArray categoriesArray;
    for (const QString &category : categories) {
      categoriesArray.append(category);
    }
    json["categories"] = categoriesArray;

    QJsonArray promptExamplesArray;
    for (const QString &example : prompt_examples) {
      promptExamplesArray.append(example);
    }
    json["prompt_examples"] = promptExamplesArray;

    json["metadata_count"] = metadata_count;
    json["updated_at"] = updated_at;
    json["source_url"] = source_url;
    json["readme_url"] = readme_url;
    json["readme_assets_path"] = readme_assets_path;
    json["icons"] = icons.toJson();
    json["download_url"] = download_url;

    QJsonArray commandsArray;
    for (const Command &command : commands) {
      commandsArray.append(command.toJson());
    }
    json["commands"] = commandsArray;

    QJsonArray contributorsArray;
    for (const User &contributor : contributors) {
      contributorsArray.append(contributor.toJson());
    }
    json["contributors"] = contributorsArray;

    QJsonArray toolsArray;
    for (const QString &tool : tools) {
      toolsArray.append(tool);
    }
    json["tools"] = toolsArray;

    return json;
  }

  /**
   * Return appropriate icon given the current theme.
   */
  ImageURL themedIcon() const {
    switch (ThemeService::instance().theme().variant()) {
    case ThemeVariant::Light:
      if (!icons.light.isEmpty()) { return ImageURL::http(icons.light); }
      break;
    case ThemeVariant::Dark:
      if (!icons.dark.isEmpty()) { return ImageURL::http(icons.dark); }
      break;
    }

    if (!icons.light.isEmpty()) { return ImageURL::http(icons.light); }
    if (!icons.dark.isEmpty()) { return ImageURL::http(icons.dark); }

    return ImageURL::http(icons.dark);
  }

  // Parse from QJsonDocument
  static Extension fromDocument(const QJsonDocument &doc) { return fromJson(doc.object()); }

  // Convert to QJsonDocument
  QJsonDocument toDocument() const { return QJsonDocument(toJson()); }

  // Convenience methods for QDateTime
  QDateTime createdAtDateTime() const { return QDateTime::fromSecsSinceEpoch(created_at); }

  QDateTime updatedAtDateTime() const { return QDateTime::fromSecsSinceEpoch(updated_at); }

  QDateTime killListedAtDateTime() const {
    return kill_listed_at != 0 ? QDateTime::fromSecsSinceEpoch(kill_listed_at) : QDateTime();
  }

  std::vector<QUrl> screenshots() const {
    std::vector<QUrl> urls;

    urls.reserve(metadata_count);

    for (int i = 0; i < metadata_count; ++i) {
      urls.emplace_back(QString("%1metadata/%2-%3.png").arg(readme_assets_path).arg(name).arg(i + 1));
    }

    return urls;
  }

  bool isKillListed() const { return kill_listed_at != 0; }
};

// Usage example helper function
inline Extension parseExtensionFromJson(const QByteArray &jsonData) {
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

  if (error.error != QJsonParseError::NoError) {
    qWarning() << "JSON parse error:" << error.errorString();
    return Extension(); // Return empty Extension on error
  }

  return Extension::fromDocument(doc);
}

struct ListFrontPageResponse {
  std::vector<Raycast::Extension> m_extensions;

public:
  ListFrontPageResponse(const std::vector<Raycast::Extension> &extensions) : m_extensions(extensions) {}
};

struct ListPaginationOptions {
  int page = 1;
  int perPage = 50;
};

using ListResult = std::expected<Raycast::ListFrontPageResponse, QString>;

using DownloadExtensionResult = std::expected<QByteArray, QString>;

} // namespace Raycast

class RaycastStoreService : public QObject, NonCopyable {
  std::unordered_map<int, Raycast::ListFrontPageResponse> m_cachedPages;
  QNetworkAccessManager *m_net = new QNetworkAccessManager(this);
  static constexpr const char *BASE_URL = "https://backend.raycast.com/api/v1";

  static QNetworkRequest createJsonApiRequest(const QUrl &url);

public:
  RaycastStoreService();

  /**
   * Download the extension bundle as a zip file.
   */
  QFuture<Raycast::DownloadExtensionResult> downloadExtension(const QUrl &url);
  QFuture<Raycast::ListResult> fetchExtensions(const Raycast::ListPaginationOptions &opts = {});
  QFuture<Raycast::ListResult> search(const QString &query);
};

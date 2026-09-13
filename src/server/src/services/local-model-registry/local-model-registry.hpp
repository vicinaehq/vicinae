#pragma once
#ifdef HAS_LOCAL_AI
#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <QObject>
#include <QString>
#include <QUrl>
#include "common/qt.hpp"
#include "common/types.hpp"
#include "model-download.hpp"
#include "local-model-catalogue.hpp"

struct LocalModel {
  LocalModelInfo info;
  bool installed = false;
  bool downloading = false;
};

/**
 * Local models: what the catalogue offers, what is on disk, and downloads in flight.
 *
 * Files land in `<dataDir>/models/` under their upstream name so hand-copied files work too.
 * Downloads come from Hugging Face; `HF_ENDPOINT` swaps the host for a mirror and `HF_TOKEN` is sent
 * when present. Progress is observable per download through the returned handle and, for anyone that
 * does not hold the handle, through the id-tagged signals on the registry.
 */
class LocalModelRegistry : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void modelsChanged() const;
  void downloadStarted(const QString &id) const;
  void downloadProgress(const QString &id, qint64 received, qint64 total) const;
  void downloadRetryScheduled(const QString &id, int attempt, int delaySeconds, const QString &reason) const;
  void downloadFinished(const QString &id) const;
  void downloadFailed(const QString &id, const QString &error) const;
  void downloadCancelled(const QString &id) const;

public:
  explicit LocalModelRegistry(QObject *parent = nullptr);
  ~LocalModelRegistry() override;

  std::vector<LocalModel> models(std::optional<AI::Capabilities> caps = std::nullopt) const;
  const LocalModelInfo *vadModel() const;
  std::optional<LocalModel> model(std::string_view id) const;
  bool isInstalled(std::string_view id) const;
  std::optional<std::filesystem::path> installedPath(std::string_view id) const;
  std::filesystem::path pathFor(const LocalModelInfo &info) const;
  const std::filesystem::path &modelsDir() const { return m_dir; }

  /**
   * Starts a download, or returns the one already in flight for this id.
   * The handle is owned by the registry and deleted after it settles; connect to it right away.
   */
  std::expected<ModelDownload *, std::string> download(std::string_view id);
  ModelDownload *activeDownload(std::string_view id) const;
  void cancelDownload(std::string_view id);

  std::expected<void, std::string> remove(std::string_view id);

  QUrl downloadUrl(const LocalModelInfo &info) const;
  const QString &baseUrl() const { return m_baseUrl; }

private:
  static QString resolveBaseUrl();
  static std::optional<QString> resolveToken();
  bool isInstalled(const LocalModelInfo &info) const;
  void settle(const std::string &id);

  std::filesystem::path m_dir;
  QString m_baseUrl;
  std::optional<QString> m_token;
  std::unordered_map<std::string, QObjectUniquePtr<ModelDownload>> m_downloads;
};
#endif

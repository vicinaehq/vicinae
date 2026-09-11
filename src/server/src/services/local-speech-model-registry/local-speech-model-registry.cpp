#include "local-speech-model-registry.hpp"
#include <algorithm>
#include <format>
#include <system_error>
#include <QTimer>
#include <qlogging.h>
#include <qtenvironmentvariables.h>
#include "vicinae.hpp"

namespace fs = std::filesystem;

LocalSpeechModelRegistry::LocalSpeechModelRegistry(QObject *parent)
    : QObject(parent), m_dir(Omnicast::dataDir() / "models" / "speech"), m_baseUrl(resolveBaseUrl()),
      m_token(resolveToken()) {
  std::error_code ec;
  fs::create_directories(m_dir, ec);
  if (ec) { qWarning() << "Could not create speech models directory" << m_dir.c_str() << ec.message(); }
}

LocalSpeechModelRegistry::~LocalSpeechModelRegistry() = default;

QString LocalSpeechModelRegistry::resolveBaseUrl() {
  auto url = qEnvironmentVariable("HF_ENDPOINT", "https://huggingface.co").trimmed();
  while (url.endsWith('/'))
    url.chop(1);
  return url;
}

std::optional<QString> LocalSpeechModelRegistry::resolveToken() {
  const auto token = qEnvironmentVariable("HF_TOKEN").trimmed();
  if (token.isEmpty()) return std::nullopt;
  return token;
}

fs::path LocalSpeechModelRegistry::pathFor(const SpeechModelInfo &info) const { return m_dir / info.file; }

QUrl LocalSpeechModelRegistry::downloadUrl(const SpeechModelInfo &info) const {
  return QUrl(QString("%1/%2/resolve/%3/%4")
                  .arg(m_baseUrl, QString::fromUtf8(info.repo), QString::fromUtf8(info.revision),
                       QString::fromUtf8(info.file)));
}

bool LocalSpeechModelRegistry::isInstalled(const SpeechModelInfo &info) const {
  std::error_code ec;
  const auto size = fs::file_size(pathFor(info), ec);
  return !ec && size == info.size;
}

bool LocalSpeechModelRegistry::isInstalled(std::string_view id) const {
  const auto *info = SpeechModelCatalogue::find(id);
  return info && isInstalled(*info);
}

std::optional<fs::path> LocalSpeechModelRegistry::installedPath(std::string_view id) const {
  const auto *info = SpeechModelCatalogue::find(id);
  if (!info || !isInstalled(*info)) return std::nullopt;
  return pathFor(*info);
}

const SpeechModelInfo *LocalSpeechModelRegistry::vadModel() const {
  const auto entries = SpeechModelCatalogue::entries();
  const auto it = std::ranges::find(entries, SpeechEngine::Vad, &SpeechModelInfo::engine);
  return it == entries.end() ? nullptr : &*it;
}

std::vector<SpeechModel> LocalSpeechModelRegistry::models(std::optional<SpeechEngine> engine) const {
  std::vector<SpeechModel> models;
  models.reserve(SpeechModelCatalogue::entries().size());
  for (const auto &info : SpeechModelCatalogue::entries()) {
    if (engine && info.engine != *engine) continue;
    models.emplace_back(SpeechModel{
        .info = info,
        .installed = isInstalled(info),
        .downloading = activeDownload(info.id) != nullptr,
    });
  }
  return models;
}

std::optional<SpeechModel> LocalSpeechModelRegistry::model(std::string_view id) const {
  const auto *info = SpeechModelCatalogue::find(id);
  if (!info) return std::nullopt;
  return SpeechModel{
      .info = *info,
      .installed = isInstalled(*info),
      .downloading = activeDownload(id) != nullptr,
  };
}

ModelDownload *LocalSpeechModelRegistry::activeDownload(std::string_view id) const {
  const auto it = m_downloads.find(std::string(id));
  if (it == m_downloads.end() || !it->second->isActive()) return nullptr;
  return it->second.get();
}

std::expected<ModelDownload *, std::string> LocalSpeechModelRegistry::download(std::string_view id) {
  const auto *info = SpeechModelCatalogue::find(id);
  if (!info) return std::unexpected(std::format("Unknown speech model '{}'", id));
  if (auto *active = activeDownload(id)) return active;
  if (isInstalled(*info)) return std::unexpected(std::format("Speech model '{}' is already installed", id));

  auto key = std::string(id);
  auto download = QObjectUniquePtr<ModelDownload>(new ModelDownload(ModelDownloadRequest{
      .url = downloadUrl(*info),
      .destination = pathFor(*info),
      .sha256 = std::string(info->sha256),
      .expectedSize = info->size,
      .bearer = m_token,
  }));
  auto *handle = download.get();
  const auto qid = QString::fromStdString(key);

  connect(handle, &ModelDownload::progress, this,
          [this, qid](qint64 received, qint64 total) { emit downloadProgress(qid, received, total); });
  connect(handle, &ModelDownload::retryScheduled, this,
          [this, qid](int attempt, int delaySeconds, const QString &reason) {
            emit downloadRetryScheduled(qid, attempt, delaySeconds, reason);
          });
  connect(handle, &ModelDownload::finished, this, [this, key, qid](const fs::path &) {
    emit downloadFinished(qid);
    emit modelsChanged();
    settle(key);
  });
  connect(handle, &ModelDownload::failed, this, [this, key, qid](const QString &error) {
    emit downloadFailed(qid, error);
    emit modelsChanged();
    settle(key);
  });
  connect(handle, &ModelDownload::cancelled, this, [this, key, qid]() {
    emit downloadCancelled(qid);
    emit modelsChanged();
    settle(key);
  });

  m_downloads.insert_or_assign(key, std::move(download));
  emit downloadStarted(qid);
  emit modelsChanged();
  handle->start();

  return handle;
}

void LocalSpeechModelRegistry::cancelDownload(std::string_view id) {
  if (auto *active = activeDownload(id)) active->cancel();
}

std::expected<void, std::string> LocalSpeechModelRegistry::remove(std::string_view id) {
  const auto *info = SpeechModelCatalogue::find(id);
  if (!info) return std::unexpected(std::format("Unknown speech model '{}'", id));

  cancelDownload(id);

  std::error_code ec;
  const auto path = pathFor(*info);
  auto partial = path;
  partial += ".part";

  fs::remove(partial, ec);
  ec.clear();
  fs::remove(path, ec);
  if (ec) return std::unexpected(std::format("Could not remove {}: {}", path.string(), ec.message()));

  emit modelsChanged();
  return {};
}

// Deferred so slots connected after ours still see a live handle during this emission.
void LocalSpeechModelRegistry::settle(const std::string &id) {
  QTimer::singleShot(0, this, [this, id]() {
    const auto it = m_downloads.find(id);
    if (it != m_downloads.end() && !it->second->isActive()) m_downloads.erase(it);
  });
}

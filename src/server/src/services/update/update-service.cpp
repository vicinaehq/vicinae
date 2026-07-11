#include "update-service.hpp"
#include "builtin_icon.hpp"
#include "environment.hpp"
#include "generated/version.h"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "vicinae.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <system_error>

namespace fs = std::filesystem;

static constexpr std::chrono::milliseconds RELAUNCH_DELAY{800};

UpdateService::UpdateService(ToastService &toast, std::unique_ptr<AbstractUpdateInstaller> installer)
    : m_toast(toast), m_installer(std::move(installer)) {
  using namespace std::chrono_literals;

  m_client.setUserAgent(QString("vicinae/%1").arg(VICINAE_GIT_TAG));
  m_statePath = Omnicast::stateDir() / "updates.json";

  const std::string currentTag = Environment::updateVersionOverride().value_or(VICINAE_GIT_TAG);
  m_currentVersion = vicinae::Semver::parse(currentTag);

  if (!m_currentVersion) {
    qInfo() << "Current version" << currentTag.c_str() << "is not a release tag, update checks are disabled";
  }

  loadState();

  connect(&m_timer, &QTimer::timeout, this, &UpdateService::performCheck);
  m_timer.setInterval(6h);

  connect(m_installer.get(), &AbstractUpdateInstaller::stageChanged, this,
          [this](const QString &stage) { m_toast.dynamic(stage); });
  connect(m_installer.get(), &AbstractUpdateInstaller::finished, this, [this]() {
    setStatus(Status::Installed);
    m_toast.success("Update installed", "Restarting…");
    QTimer::singleShot(RELAUNCH_DELAY, this, [this]() { relaunch(); });
  });
  connect(m_installer.get(), &AbstractUpdateInstaller::failed, this, &UpdateService::failInstall);

  if (checksSupported()) {
    m_timer.start();
    performCheck();
  }
}

QString UpdateService::currentVersionTag() const { return VICINAE_GIT_TAG; }

void UpdateService::checkNow() { performCheck(); }

void UpdateService::performCheck() {
  if (!checksSupported()) return;
  if (m_status == Status::Downloading || m_status == Status::Installing || m_status == Status::Installed)
    return;

  setStatus(Status::Checking);

  m_client.get<github::Release>(Environment::updateFeedUrl())
      .then([this](const http::Client::Result<github::Release> &res) {
        if (!res) {
          qWarning() << "Update check failed:" << res.error();
          if (m_status == Status::Checking) setStatus(Status::Idle);
          return;
        }

        handleRelease(res.value());
      });
}

void UpdateService::handleRelease(const github::Release &release) {
  auto const remote = vicinae::Semver::parse(release.tag_name);
  bool const newer = remote && m_currentVersion && *remote > *m_currentVersion;
  bool const skipped = m_state.skippedVersion == release.tag_name;

  if (!newer || release.draft || release.prerelease || skipped) {
    m_available.reset();
    setStatus(Status::Idle);
    return;
  }

  const QString assetName = m_installer->assetName();
  auto const matchesAsset = [&](const github::ReleaseAsset &asset) {
    return QString::fromStdString(asset.name) == assetName;
  };
  auto const asset = std::ranges::find_if(release.assets, matchesAsset);

  if (asset == release.assets.end()) {
    qInfo() << "Release" << release.tag_name.c_str() << "has no" << assetName << "asset, ignoring";
    m_available.reset();
    setStatus(Status::Idle);
    return;
  }

  AvailableUpdate update;

  update.tag = QString::fromStdString(release.tag_name);
  update.version = update.tag.startsWith('v') ? update.tag.mid(1) : update.tag;
  update.releaseUrl = QString::fromStdString(release.html_url);
  update.assetUrl = QString::fromStdString(asset->browser_download_url);

  qInfo() << "Update available:" << update.tag;

  m_available = std::move(update);
  setStatus(Status::UpdateAvailable);
}

void UpdateService::downloadAndInstall() {
  if (!m_available) return;
  if (m_status == Status::Downloading || m_status == Status::Installing || m_status == Status::Installed)
    return;

  std::error_code ec;
  const fs::path downloadDir = Omnicast::cacheDir() / "updates";

  fs::create_directories(downloadDir, ec);

  const fs::path archivePath = downloadDir / m_installer->assetName().toStdString();

  setStatus(Status::Downloading);

  const QString tag = m_available->tag;
  const QString version = m_available->version;

  m_toast.dynamic(QString("Downloading Vicinae %1…").arg(tag));

  auto *download = m_client.download(m_available->assetUrl, archivePath);

  connect(download, &http::Download::progress, this, [this, tag](qint64 received, qint64 total) {
    if (total > 0) {
      int const pct = static_cast<int>(received * 100 / total);
      m_toast.dynamic(QString("Downloading Vicinae %1… %2%").arg(tag).arg(pct));
    }
  });
  connect(download, &http::Download::failed, this, &UpdateService::failInstall);
  connect(download, &http::Download::finished, this, [this, version](const fs::path &path) {
    setStatus(Status::Installing);
    m_toast.dynamic("Installing update…");
    m_installer->install(path, version);
  });
}

void UpdateService::skipAvailableVersion() {
  if (!m_available) return;

  m_state.skippedVersion = m_available->tag.toStdString();
  saveState();
  m_available.reset();
  setStatus(Status::Idle);
}

void UpdateService::relaunch() { m_installer->relaunch(); }

void UpdateService::setStatus(Status status) {
  m_status = status;
  emit updateChanged();
}

void UpdateService::failInstall(const QString &error) {
  qWarning() << "Update failed:" << error;
  setStatus(Status::Failed);
  m_toast.failure("Update failed", error);
}

void UpdateService::saveState() {
  std::error_code ec;

  fs::create_directories(m_statePath.parent_path(), ec);

  if (auto const error = glz::write_file_json(m_state, m_statePath.string(), m_buf)) {
    qWarning() << "Failed to write update state file at" << m_statePath.string() << glz::format_error(error);
  }
}

void UpdateService::loadState() {
  if (!fs::is_regular_file(m_statePath)) return;

  if (auto const error = glz::read_file_json(m_state, m_statePath.string(), m_buf)) {
    qWarning() << "Failed to read update state file at" << m_statePath.string() << glz::format_error(error);
  }
}

InstallUpdateAction::InstallUpdateAction()
    : AbstractAction(QStringLiteral("Install Update"), BuiltinIcon::Download) {}

void InstallUpdateAction::execute(ApplicationContext *ctx) {
  auto *updates = ctx->services->updateService();
  auto const status = updates->status();

  if (status == UpdateService::Status::Downloading || status == UpdateService::Status::Installing) {
    ctx->services->toastService()->setToast("An update is already in progress");
    return;
  }

  updates->downloadAndInstall();
}

SkipUpdateVersionAction::SkipUpdateVersionAction()
    : AbstractAction(QStringLiteral("Skip This Version"), BuiltinIcon::Xmark) {}

void SkipUpdateVersionAction::execute(ApplicationContext *ctx) {
  auto *updates = ctx->services->updateService();

  if (const auto &update = updates->available()) {
    ctx->services->toastService()->setToast(QString("Skipped %1").arg(update->tag));
  }

  updates->skipAvailableVersion();
}

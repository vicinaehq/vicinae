#include "services/media-control/macos/mediaremote-media-control.hpp"
#import <AppKit/AppKit.h>
#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <qlogging.h>
#include "services/ai/cli/cli-process.hpp"
#include "vicinae.hpp"

namespace {

constexpr auto PERL = "/usr/bin/perl";
constexpr auto SCRIPT_NAME = "mediaremote-adapter.pl";
constexpr auto FRAMEWORK_NAME = "MediaRemoteAdapter.framework";

QString applicationName(pid_t pid, const QString &fallback) {
  @autoreleasepool {
    NSRunningApplication *app = [NSRunningApplication runningApplicationWithProcessIdentifier:pid];
    if (app && app.localizedName) return QString::fromNSString(app.localizedName);
  }
  return fallback;
}

} // namespace

std::optional<MediaRemoteMediaControl::Adapter> MediaRemoteMediaControl::locateAdapter() {
  std::vector<Adapter> candidates;
  candidates.reserve(2);

  const QDir resources(QString::fromStdString(Omnicast::bundleResourceDir().string()));
  const QDir frameworks(QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../Frameworks"));
  candidates.emplace_back(resources.filePath(SCRIPT_NAME), frameworks.filePath(FRAMEWORK_NAME));
#ifdef MEDIAREMOTE_ADAPTER_DEV_SCRIPT
  candidates.emplace_back(QStringLiteral(MEDIAREMOTE_ADAPTER_DEV_SCRIPT),
                          QStringLiteral(MEDIAREMOTE_ADAPTER_DEV_FRAMEWORK));
#endif

  for (auto &candidate : candidates) {
    if (QFileInfo::exists(candidate.script) && QFileInfo::exists(candidate.framework)) return candidate;
  }
  return std::nullopt;
}

MediaRemoteMediaControl::MediaRemoteMediaControl() : m_adapter(locateAdapter()) {
  m_restart.setSingleShot(true);
  m_restart.setInterval(RESTART_DELAY_MS);
  connect(&m_restart, &QTimer::timeout, this, &MediaRemoteMediaControl::startStream);

  if (!m_adapter) {
    qWarning() << "MediaRemote adapter not found, media players cannot be controlled";
    return;
  }
  startStream();
}

MediaRemoteMediaControl::~MediaRemoteMediaControl() {
  if (m_stream) m_stream->shutdown();
}

void MediaRemoteMediaControl::startStream() {
  if (m_stream) m_stream->shutdown();
  m_stream = new AI::JsonLineProcess(
      PERL, {m_adapter->script, m_adapter->framework, "stream", "--no-artwork", "--no-diff", "--debounce=50"},
      this);
  connect(m_stream, &AI::JsonLineProcess::lineReceived, this, &MediaRemoteMediaControl::handleLine);
  connect(m_stream, &AI::JsonLineProcess::exited, this, &MediaRemoteMediaControl::handleExit);
  m_stream->start();
}

void MediaRemoteMediaControl::handleLine(const std::string &line) {
  const auto doc = QJsonDocument::fromJson(QByteArray::fromRawData(line.data(), line.size()));
  if (!doc.isObject()) return;

  m_operational = true;
  m_restarts = 0;

  const auto payload = doc.object().value("payload").toObject();
  const auto bundleId = payload.value("bundleIdentifier").toString();

  if (bundleId.isEmpty()) {
    if (!m_nowPlaying) return;
    m_nowPlaying.reset();
    emit playersChanged();
    return;
  }

  const auto pid = static_cast<pid_t>(payload.value("processIdentifier").toInt());
  m_nowPlaying = MediaPlayer{
      .id = bundleId,
      .identity = applicationName(pid, bundleId),
      .appId = bundleId,
      .title = payload.value("title").toString(),
      .artist = payload.value("artist").toString(),
      .status = payload.value("playing").toBool() ? PlaybackStatus::Playing : PlaybackStatus::Paused,
      .canGoNext = true,
      .canGoPrevious = true,
  };
  emit playersChanged();
}

void MediaRemoteMediaControl::handleExit(const std::string &reason) {
  m_operational = false;
  if (m_nowPlaying) {
    m_nowPlaying.reset();
    emit playersChanged();
  }
  if (m_stream) m_stream->shutdown();

  if (m_restarts++ >= MAX_RESTARTS) {
    qWarning() << "MediaRemote adapter keeps exiting, giving up:" << reason.c_str();
    return;
  }
  qWarning() << "MediaRemote adapter exited, restarting:" << reason.c_str();
  m_restart.start();
}

std::vector<MediaPlayer> MediaRemoteMediaControl::players() const {
  if (!m_nowPlaying) return {};
  return {*m_nowPlaying};
}

bool MediaRemoteMediaControl::send(const QString &playerId, int command) {
  if (!m_adapter || !m_nowPlaying || m_nowPlaying->id != playerId) return false;
  setLastPlayerId(playerId);
  return QProcess::startDetached(PERL,
                                 {m_adapter->script, m_adapter->framework, "send", QString::number(command)});
}

bool MediaRemoteMediaControl::playPause(const QString &playerId) {
  const bool playing = m_nowPlaying && m_nowPlaying->status == PlaybackStatus::Playing;
  if (!send(playerId, playing ? PAUSE : PLAY)) return false;
  // The stream confirms a moment later, callers acting right away need the new state now
  m_nowPlaying->status = playing ? PlaybackStatus::Paused : PlaybackStatus::Playing;
  emit playersChanged();
  return true;
}

bool MediaRemoteMediaControl::next(const QString &playerId) { return send(playerId, NEXT); }

bool MediaRemoteMediaControl::previous(const QString &playerId) { return send(playerId, PREVIOUS); }

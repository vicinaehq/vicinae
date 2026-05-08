#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include "wpctl-audio-control.hpp"

WpctlAudioControl::WpctlAudioControl() : m_wpctlPath(QStandardPaths::findExecutable("wpctl")) {
  if (m_wpctlPath.isEmpty()) { qWarning() << "wpctl not found, audio control will not work"; }
}

QString WpctlAudioControl::id() const { return "wpctl"; }

float WpctlAudioControl::getVolume() const {
  auto info = queryDefaultSinkVolume();
  return info ? info->volume : 0.0f;
}

bool WpctlAudioControl::setVolume(float level) {
  return runWpctl({"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(level, 'f', 2)}).has_value();
}

bool WpctlAudioControl::adjustVolume(float delta) {
  auto prefix = delta >= 0 ? "+" : "-";
  auto value = QString::number(std::abs(delta), 'f', 2) + prefix;
  return runWpctl({"set-volume", "@DEFAULT_AUDIO_SINK@", value}).has_value();
}

bool WpctlAudioControl::isMuted() const {
  auto info = queryDefaultSinkVolume();
  return info && info->muted;
}

bool WpctlAudioControl::setMuted(bool muted) {
  return runWpctl({"set-mute", "@DEFAULT_AUDIO_SINK@", muted ? "1" : "0"}).has_value();
}

bool WpctlAudioControl::toggleMute() {
  return runWpctl({"set-mute", "@DEFAULT_AUDIO_SINK@", "toggle"}).has_value();
}

std::vector<AudioSink> WpctlAudioControl::listSinks() const {
  auto output = runWpctl({"status"});
  if (!output) return {};

  std::vector<AudioSink> sinks;
  static const QRegularExpression sinkRe(
      R"(^[│ ]+(\*?)\s+(\d+)\.\s+(.+?)\s+\[vol:\s+([\d.]+)(\s+MUTED)?\]\s*$)");

  bool inAudioSinks = false;
  for (const auto &line : output->split('\n')) {
    if (line.contains("Audio")) { continue; }

    if (line.contains(QString::fromUtf8("├─ Sinks:")) || line.contains(QString::fromUtf8("└─ Sinks:"))) {
      inAudioSinks = true;
      continue;
    }

    if (inAudioSinks && (line.contains(QString::fromUtf8("├─")) || line.contains(QString::fromUtf8("└─")))) {
      break;
    }

    if (!inAudioSinks) continue;

    auto match = sinkRe.match(line);
    if (!match.hasMatch()) continue;

    auto &sink = sinks.emplace_back();
    sink.isDefault = !match.captured(1).isEmpty();
    sink.id = match.captured(2);
    sink.name = match.captured(3).trimmed();
    sink.description = sink.name;
    sink.volume = match.captured(4).toFloat();
    sink.muted = !match.captured(5).isEmpty();
  }

  return sinks;
}

bool WpctlAudioControl::setDefaultSink(const QString &sinkId) {
  return runWpctl({"set-default", sinkId}).has_value();
}

std::optional<WpctlAudioControl::VolumeInfo> WpctlAudioControl::queryDefaultSinkVolume() const {
  auto output = runWpctl({"get-volume", "@DEFAULT_AUDIO_SINK@"});
  if (!output) return std::nullopt;

  static const QRegularExpression re(R"(Volume:\s+([\d.]+)(\s+\[MUTED\])?)");
  auto match = re.match(*output);
  if (!match.hasMatch()) return std::nullopt;

  return VolumeInfo{
      .volume = match.captured(1).toFloat(),
      .muted = !match.captured(2).isEmpty(),
  };
}

std::optional<QString> WpctlAudioControl::runWpctl(const QStringList &args) const {
  if (m_wpctlPath.isEmpty()) return std::nullopt;

  QProcess proc;
  proc.start(m_wpctlPath, args);

  if (!proc.waitForFinished(3000)) {
    qWarning() << "wpctl timed out:" << args;
    return std::nullopt;
  }

  if (proc.exitCode() != 0) {
    qWarning() << "wpctl failed:" << args << proc.readAllStandardError();
    return std::nullopt;
  }

  return QString::fromUtf8(proc.readAllStandardOutput());
}

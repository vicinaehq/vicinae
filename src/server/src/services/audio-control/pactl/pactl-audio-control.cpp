#include <QProcess>
#include <QStandardPaths>
#include <glaze/glaze.hpp>
#include "lib/glaze-qt.hpp"
#include "pactl-audio-control.hpp"

namespace {

struct PactlVolume {
  QString value_percent;
};

struct PactlPort {
  QString name;
  QString description;
};

struct PactlSink {
  QString name;
  QString description;
  bool mute = false;
  std::map<std::string, PactlVolume> volume;
  std::vector<PactlPort> ports;
  QString active_port;
};

} // namespace

template <> struct glz::meta<PactlVolume> : glz::snake_case {};
template <> struct glz::meta<PactlPort> : glz::snake_case {};
template <> struct glz::meta<PactlSink> : glz::snake_case {};

static AudioSink toAudioSink(const PactlSink &src, const QString &defaultName) {
  AudioSink sink;
  sink.name = src.name;
  sink.description = src.description;
  sink.muted = src.mute;
  sink.isDefault = (src.name == defaultName);

  if (!src.volume.empty()) {
    auto percent = src.volume.begin()->second.value_percent;
    sink.volume = static_cast<float>(percent.chopped(1).toDouble() / 100.0);
  }

  for (const auto &port : src.ports) {
    if (port.name == src.active_port) {
      sink.activePort = port.description;
      break;
    }
  }

  return sink;
}

PactlAudioControl::PactlAudioControl() : m_pactlPath(QStandardPaths::findExecutable("pactl")) {
  if (m_pactlPath.isEmpty()) { qWarning() << "pactl not found, audio control will not work"; }
}

QString PactlAudioControl::id() const { return "pactl"; }

float PactlAudioControl::getVolume() const {
  auto sink = getDefaultSink();
  return sink ? sink->volume : 0.0f;
}

bool PactlAudioControl::setVolume(float level) {
  level = std::clamp(level, 0.0f, 1.0f);
  auto percent = QString::number(qRound(level * 100)) + "%";
  return run({"set-sink-volume", "@DEFAULT_SINK@", percent}).has_value();
}

bool PactlAudioControl::adjustVolume(float delta) {
  auto current = getVolume();
  return setVolume(current + delta);
}

bool PactlAudioControl::isMuted() const {
  auto sink = getDefaultSink();
  return sink && sink->muted;
}

bool PactlAudioControl::setMuted(bool muted) {
  return run({"set-sink-mute", "@DEFAULT_SINK@", muted ? "1" : "0"}).has_value();
}

bool PactlAudioControl::toggleMute() {
  return run({"set-sink-mute", "@DEFAULT_SINK@", "toggle"}).has_value();
}

std::vector<AudioSink> PactlAudioControl::listSinks() const {
  auto defaultName = run({"get-default-sink"});
  if (!defaultName) return {};

  auto json = run({"--format=json", "list", "sinks"});
  if (!json) return {};

  std::vector<PactlSink> pactlSinks;
  auto data = json->toStdString();

  if (auto err = glz::read<glz::opts{.error_on_unknown_keys = false}>(pactlSinks, data)) {
    qWarning() << "Failed to parse pactl sink list:" << glz::format_error(err, data);
    return {};
  }

  auto trimmedDefault = defaultName->trimmed();
  std::vector<AudioSink> sinks;
  sinks.reserve(pactlSinks.size());

  for (const auto &ps : pactlSinks) {
    sinks.emplace_back(toAudioSink(ps, trimmedDefault));
  }

  return sinks;
}

bool PactlAudioControl::setDefaultSink(const QString &sinkName) {
  return run({"set-default-sink", sinkName}).has_value();
}

std::optional<AudioSink> PactlAudioControl::getDefaultSink() const {
  auto allSinks = listSinks();

  for (auto &sink : allSinks) {
    if (sink.isDefault) return std::move(sink);
  }

  return std::nullopt;
}

std::optional<QString> PactlAudioControl::run(const QStringList &args) const {
  if (m_pactlPath.isEmpty()) return std::nullopt;

  QProcess proc;
  proc.start(m_pactlPath, args);

  if (!proc.waitForFinished(3000)) {
    qWarning() << "pactl timed out:" << args;
    return std::nullopt;
  }

  if (proc.exitCode() != 0) {
    qWarning() << "pactl failed:" << args << proc.readAllStandardError();
    return std::nullopt;
  }

  return QString::fromUtf8(proc.readAllStandardOutput());
}

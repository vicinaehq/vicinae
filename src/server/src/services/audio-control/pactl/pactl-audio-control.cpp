#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <glaze/glaze.hpp>
#include "pactl-audio-control.hpp"

struct PactlVolume {
  std::string value_percent;
};

struct PactlPort {
  std::string name;
  std::string description;
};

struct PactlSink {
  std::string name;
  std::string description;
  bool mute = false;
  std::map<std::string, PactlVolume> volume;
  std::vector<PactlPort> ports;
  std::string active_port;
};

template <> struct glz::meta<PactlVolume> : glz::snake_case {};
template <> struct glz::meta<PactlPort> : glz::snake_case {};
template <> struct glz::meta<PactlSink> : glz::snake_case {};

static AudioSink toAudioSink(const PactlSink &src, std::string_view defaultName) {
  AudioSink sink;
  sink.name = QString::fromStdString(src.name);
  sink.description = QString::fromStdString(src.description);
  sink.muted = src.mute;
  sink.isDefault = (src.name == defaultName);

  if (!src.volume.empty()) {
    auto &percent = src.volume.begin()->second.value_percent;
    sink.volume = static_cast<float>(std::stod(percent) / 100.0);
  }

  for (const auto &port : src.ports) {
    if (port.name == src.active_port) {
      sink.activePort = QString::fromStdString(port.description);
      break;
    }
  }

  return sink;
}

PactlAudioControl::PactlAudioControl() : m_pactlPath(QStandardPaths::findExecutable("pactl").toStdString()) {
  if (m_pactlPath.empty()) { qWarning() << "pactl not found, audio control will not work"; }
}

QString PactlAudioControl::id() const { return "pactl"; }

float PactlAudioControl::getVolume() const {
  auto sink = getDefaultSink();
  return sink ? sink->volume : 0.0f;
}

std::optional<float> PactlAudioControl::setVolume(float level) {
  level = std::clamp(level, 0.0f, 1.0f);
  auto percent = std::to_string(static_cast<int>(std::round(level * 100))) + "%";
  if (!run({"set-sink-volume", "@DEFAULT_SINK@", percent})) return std::nullopt;
  return level;
}

std::optional<float> PactlAudioControl::adjustVolume(float delta) {
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

  if (auto err = glz::read<glz::opts{.error_on_unknown_keys = false}>(pactlSinks, *json)) {
    qWarning() << "Failed to parse pactl sink list:" << glz::format_error(err, *json);
    return {};
  }

  auto trimmedDefault = std::string_view{*defaultName};
  while (!trimmedDefault.empty() && trimmedDefault.back() == '\n') {
    trimmedDefault.remove_suffix(1);
  }

  std::vector<AudioSink> sinks;
  sinks.reserve(pactlSinks.size());

  for (const auto &ps : pactlSinks) {
    sinks.emplace_back(toAudioSink(ps, trimmedDefault));
  }

  return sinks;
}

bool PactlAudioControl::setDefaultSink(const QString &sinkName) {
  return run({"set-default-sink", sinkName.toStdString()}).has_value();
}

std::optional<AudioSink> PactlAudioControl::getDefaultSink() const {
  auto allSinks = listSinks();

  for (auto &sink : allSinks) {
    if (sink.isDefault) return std::move(sink);
  }

  return std::nullopt;
}

std::optional<std::string> PactlAudioControl::run(std::initializer_list<std::string_view> args) const {
  if (m_pactlPath.empty()) return std::nullopt;

  QStringList qargs;
  qargs.reserve(static_cast<int>(args.size()));
  for (auto arg : args) {
    qargs.emplaceBack(QString::fromUtf8(arg.data(), arg.size()));
  }

  QProcess proc;
  proc.start(QString::fromStdString(m_pactlPath), qargs);

  if (!proc.waitForFinished(3000)) {
    qWarning() << "pactl timed out:" << qargs;
    return std::nullopt;
  }

  if (proc.exitCode() != 0) {
    qWarning() << "pactl failed:" << qargs << proc.readAllStandardError();
    return std::nullopt;
  }

  auto output = proc.readAllStandardOutput();
  return std::string(output.constData(), output.size());
}

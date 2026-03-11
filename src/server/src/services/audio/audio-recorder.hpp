#pragma once
#include <QAudioFormat>
#include <QAudioSource>
#include <QElapsedTimer>
#include <QMediaDevices>
#include <QObject>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

namespace Audio {

class Recorder : public QObject {
  Q_OBJECT

signals:
  void levelChanged();
  void stateChanged();
  void errorOccurred(const QString &message);

public:
  enum class State { Idle, Recording, Paused };

  explicit Recorder(QObject *parent = nullptr);
  ~Recorder() override;

  bool start();
  void pause();
  void resume();
  void stop();
  void discard();

  State state() const { return m_state; }
  float level() const { return m_level; }
  qint64 elapsedMs() const;

  std::optional<std::filesystem::path> outputPath() const { return m_outputPath; }

private:
  void processAudioData();
  QAudioFormat targetFormat() const;

  std::unique_ptr<QAudioSource> m_source;
  QIODevice *m_ioDevice = nullptr;
  QElapsedTimer m_elapsed;
  qint64 m_pausedElapsed = 0;

  std::vector<std::int16_t> m_pcmBuffer;
  QAudioFormat m_format;
  float m_level = 0.0f;
  State m_state = State::Idle;
  std::optional<std::filesystem::path> m_outputPath;
};

} // namespace Audio

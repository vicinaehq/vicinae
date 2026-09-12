#pragma once
#include <QAudioFormat>
#include <QAudioSource>
#include <QElapsedTimer>
#include <QMediaDevices>
#include <QObject>
#include <cstdint>
#include <memory>
#include <qaudioformat.h>
#include <qstringview.h>
#include <vector>

namespace Audio {

// Wrapper around the resulting audio stream, in PCMF32 format
// Has convenience methods to convert to pcmint16, wav, etc...
class Recording {
public:
  using PCMF32 = std::vector<float>;

  explicit Recording(PCMF32 data, QAudioFormat format);

  std::vector<std::int16_t> toInt16() const;
  std::span<const float> toF32() const;
  QByteArray toWav() const;
  QAudioFormat format() const;

private:
  PCMF32 m_data;
  QAudioFormat m_format;
};

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

  // mark the recording as finished and move the data out of the recorder
  Recording finish() const;

private:
  void processAudioData();
  QAudioFormat targetFormat() const;

  std::unique_ptr<QAudioSource> m_source;
  QIODevice *m_ioDevice = nullptr;
  QElapsedTimer m_elapsed;
  qint64 m_pausedElapsed = 0;

  std::vector<float> m_pcmBuffer;
  QAudioFormat m_format;
  float m_level = 0.0f;
  State m_state = State::Idle;
};

} // namespace Audio

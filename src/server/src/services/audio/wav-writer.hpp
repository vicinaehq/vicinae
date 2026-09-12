#pragma once
#include "services/audio/audio-recorder.hpp"
#include <QAudioFormat>
#include <qstringview.h>

namespace Audio {

/// Writes raw PCM samples as a WAV file.
QByteArray writeWav(const Recording &recording);

} // namespace Audio

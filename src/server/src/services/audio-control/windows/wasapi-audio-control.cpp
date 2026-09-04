#include "wasapi-audio-control.hpp"
#include "utils/scoped-com.hpp"
#include <algorithm>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace {

ComPtr<IAudioEndpointVolume> defaultEndpointVolume() {
  ComPtr<IMMDeviceEnumerator> enumerator;
  ComPtr<IMMDevice> device;
  ComPtr<IAudioEndpointVolume> volume;

  if (FAILED(
          CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator))) ||
      FAILED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))) {
    return nullptr;
  }

  device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, &volume);
  return volume;
}

std::optional<float> readVolume(IAudioEndpointVolume *volume) {
  float level = 0.0f;
  if (!volume || FAILED(volume->GetMasterVolumeLevelScalar(&level))) return std::nullopt;
  return level;
}

} // namespace

QString WasapiAudioControl::id() const { return "wasapi"; }

float WasapiAudioControl::getVolume() const {
  ScopedCom com;
  return readVolume(defaultEndpointVolume().Get()).value_or(0.0f);
}

std::optional<float> WasapiAudioControl::setVolume(float level) {
  ScopedCom com;
  auto volume = defaultEndpointVolume();
  if (!volume) return std::nullopt;

  level = std::clamp(level, 0.0f, 1.0f);
  if (FAILED(volume->SetMasterVolumeLevelScalar(level, nullptr))) return std::nullopt;
  return readVolume(volume.Get()).value_or(level);
}

std::optional<float> WasapiAudioControl::adjustVolume(float delta) { return setVolume(getVolume() + delta); }

bool WasapiAudioControl::isMuted() const {
  ScopedCom com;
  auto volume = defaultEndpointVolume();
  BOOL muted = FALSE;
  return volume && SUCCEEDED(volume->GetMute(&muted)) && muted;
}

bool WasapiAudioControl::setMuted(bool muted) {
  ScopedCom com;
  auto volume = defaultEndpointVolume();
  return volume && SUCCEEDED(volume->SetMute(muted ? TRUE : FALSE, nullptr));
}

bool WasapiAudioControl::toggleMute() { return setMuted(!isMuted()); }

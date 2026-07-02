#include <algorithm>
#include <optional>
#include <CoreAudio/CoreAudio.h>
#include "coreaudio-audio-control.hpp"

namespace {

std::optional<AudioDeviceID> defaultOutputDevice() {
  AudioObjectPropertyAddress addr{kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal,
                                  kAudioObjectPropertyElementMain};
  AudioDeviceID device = kAudioObjectUnknown;
  UInt32 size = sizeof(device);

  if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, &device) != noErr ||
      device == kAudioObjectUnknown) {
    return std::nullopt;
  }

  return device;
}

std::optional<std::pair<UInt32, UInt32>> stereoChannels(AudioDeviceID device) {
  AudioObjectPropertyAddress addr{kAudioDevicePropertyPreferredChannelsForStereo,
                                  kAudioObjectPropertyScopeOutput, kAudioObjectPropertyElementMain};
  UInt32 channels[2] = {0, 0};
  UInt32 size = sizeof(channels);

  if (AudioObjectGetPropertyData(device, &addr, 0, nullptr, &size, channels) != noErr) {
    return std::nullopt;
  }

  return std::pair{channels[0], channels[1]};
}

std::optional<float> readVolume(AudioDeviceID device) {
  AudioObjectPropertyAddress addr{kAudioDevicePropertyVolumeScalar, kAudioObjectPropertyScopeOutput,
                                  kAudioObjectPropertyElementMain};
  Float32 volume = 0.0f;
  UInt32 size = sizeof(volume);

  if (AudioObjectHasProperty(device, &addr) &&
      AudioObjectGetPropertyData(device, &addr, 0, nullptr, &size, &volume) == noErr) {
    return volume;
  }

  auto channels = stereoChannels(device);
  if (!channels) return std::nullopt;

  float sum = 0.0f;
  int count = 0;
  for (UInt32 channel : {channels->first, channels->second}) {
    addr.mElement = channel;
    size = sizeof(volume);
    if (AudioObjectGetPropertyData(device, &addr, 0, nullptr, &size, &volume) == noErr) {
      sum += volume;
      ++count;
    }
  }

  if (count == 0) return std::nullopt;
  return sum / static_cast<float>(count);
}

bool writeVolume(AudioDeviceID device, float level) {
  Float32 volume = std::clamp(level, 0.0f, 1.0f);
  AudioObjectPropertyAddress addr{kAudioDevicePropertyVolumeScalar, kAudioObjectPropertyScopeOutput,
                                  kAudioObjectPropertyElementMain};
  Boolean settable = false;

  if (AudioObjectHasProperty(device, &addr) &&
      AudioObjectIsPropertySettable(device, &addr, &settable) == noErr && settable) {
    return AudioObjectSetPropertyData(device, &addr, 0, nullptr, sizeof(volume), &volume) == noErr;
  }

  auto channels = stereoChannels(device);
  if (!channels) return false;

  bool ok = true;
  int count = 0;
  for (UInt32 channel : {channels->first, channels->second}) {
    addr.mElement = channel;
    if (AudioObjectIsPropertySettable(device, &addr, &settable) != noErr || !settable) {
      ok = false;
      continue;
    }
    if (AudioObjectSetPropertyData(device, &addr, 0, nullptr, sizeof(volume), &volume) != noErr) ok = false;
    ++count;
  }

  return count > 0 && ok;
}

std::optional<bool> readMute(AudioDeviceID device) {
  AudioObjectPropertyAddress addr{kAudioDevicePropertyMute, kAudioObjectPropertyScopeOutput,
                                  kAudioObjectPropertyElementMain};
  UInt32 muted = 0;
  UInt32 size = sizeof(muted);

  if (AudioObjectHasProperty(device, &addr) &&
      AudioObjectGetPropertyData(device, &addr, 0, nullptr, &size, &muted) == noErr) {
    return muted != 0;
  }

  auto channels = stereoChannels(device);
  if (!channels) return std::nullopt;

  bool all = true;
  int count = 0;
  for (UInt32 channel : {channels->first, channels->second}) {
    addr.mElement = channel;
    size = sizeof(muted);
    if (AudioObjectHasProperty(device, &addr) &&
        AudioObjectGetPropertyData(device, &addr, 0, nullptr, &size, &muted) == noErr) {
      all = all && (muted != 0);
      ++count;
    }
  }

  if (count == 0) return std::nullopt;
  return all;
}

bool writeMute(AudioDeviceID device, bool muted) {
  UInt32 value = muted ? 1 : 0;
  AudioObjectPropertyAddress addr{kAudioDevicePropertyMute, kAudioObjectPropertyScopeOutput,
                                  kAudioObjectPropertyElementMain};
  Boolean settable = false;

  if (AudioObjectHasProperty(device, &addr) &&
      AudioObjectIsPropertySettable(device, &addr, &settable) == noErr && settable) {
    return AudioObjectSetPropertyData(device, &addr, 0, nullptr, sizeof(value), &value) == noErr;
  }

  auto channels = stereoChannels(device);
  if (!channels) return false;

  bool ok = true;
  int count = 0;
  for (UInt32 channel : {channels->first, channels->second}) {
    addr.mElement = channel;
    if (!AudioObjectHasProperty(device, &addr) ||
        AudioObjectIsPropertySettable(device, &addr, &settable) != noErr || !settable) {
      continue;
    }
    if (AudioObjectSetPropertyData(device, &addr, 0, nullptr, sizeof(value), &value) != noErr) ok = false;
    ++count;
  }

  return count > 0 && ok;
}

std::optional<QString> stringProperty(AudioObjectID object, AudioObjectPropertySelector selector) {
  AudioObjectPropertyAddress addr{selector, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
  CFStringRef ref = nullptr;
  UInt32 size = sizeof(ref);

  if (AudioObjectGetPropertyData(object, &addr, 0, nullptr, &size, &ref) != noErr || !ref) {
    return std::nullopt;
  }

  QString value = QString::fromCFString(ref);
  CFRelease(ref);
  return value;
}

bool hasOutputStreams(AudioDeviceID device) {
  AudioObjectPropertyAddress addr{kAudioDevicePropertyStreams, kAudioObjectPropertyScopeOutput,
                                  kAudioObjectPropertyElementMain};
  UInt32 size = 0;
  return AudioObjectGetPropertyDataSize(device, &addr, 0, nullptr, &size) == noErr && size > 0;
}

} // namespace

QString CoreAudioControl::id() const { return "coreaudio"; }

float CoreAudioControl::getVolume() const {
  auto device = defaultOutputDevice();
  if (!device) return 0.0f;
  return readVolume(*device).value_or(0.0f);
}

std::optional<float> CoreAudioControl::setVolume(float level) {
  auto device = defaultOutputDevice();
  if (!device) return std::nullopt;

  level = std::clamp(level, 0.0f, 1.0f);
  if (!writeVolume(*device, level)) return std::nullopt;
  return level;
}

std::optional<float> CoreAudioControl::adjustVolume(float delta) { return setVolume(getVolume() + delta); }

bool CoreAudioControl::isMuted() const {
  auto device = defaultOutputDevice();
  return device && readMute(*device).value_or(false);
}

bool CoreAudioControl::setMuted(bool muted) {
  auto device = defaultOutputDevice();
  if (!device) return false;
  return writeMute(*device, muted);
}

bool CoreAudioControl::toggleMute() { return setMuted(!isMuted()); }

std::vector<AudioSink> CoreAudioControl::listSinks() const {
  AudioObjectPropertyAddress addr{kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal,
                                  kAudioObjectPropertyElementMain};
  UInt32 size = 0;
  if (AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr, 0, nullptr, &size) != noErr) return {};

  std::vector<AudioDeviceID> devices(size / sizeof(AudioDeviceID));
  if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, devices.data()) !=
      noErr) {
    return {};
  }

  auto defaultDevice = defaultOutputDevice();

  std::vector<AudioSink> sinks;
  sinks.reserve(devices.size());

  for (auto device : devices) {
    if (!hasOutputStreams(device)) continue;

    auto uid = stringProperty(device, kAudioDevicePropertyDeviceUID);
    if (!uid) continue;

    AudioSink sink;
    sink.name = *uid;
    sink.description = stringProperty(device, kAudioObjectPropertyName).value_or(*uid);
    sink.volume = readVolume(device).value_or(0.0f);
    sink.muted = readMute(device).value_or(false);
    sink.isDefault = defaultDevice && *defaultDevice == device;
    sinks.emplace_back(std::move(sink));
  }

  return sinks;
}

bool CoreAudioControl::setDefaultSink(const QString &sinkName) {
  CFStringRef uid = sinkName.toCFString();
  AudioObjectPropertyAddress translate{kAudioHardwarePropertyTranslateUIDToDevice,
                                       kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain};
  AudioDeviceID device = kAudioObjectUnknown;
  UInt32 size = sizeof(device);

  OSStatus status =
      AudioObjectGetPropertyData(kAudioObjectSystemObject, &translate, sizeof(uid), &uid, &size, &device);
  CFRelease(uid);
  if (status != noErr || device == kAudioObjectUnknown) return false;

  AudioObjectPropertyAddress addr{kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal,
                                  kAudioObjectPropertyElementMain};
  return AudioObjectSetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, sizeof(device), &device) ==
         noErr;
}

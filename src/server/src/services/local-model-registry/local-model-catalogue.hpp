#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include <QCoreApplication>
#include <QString>
#include <QColor>
#include "services/ai/ai-capability.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/image/url.hpp"

enum class LocalEngine : std::uint8_t { Whisper, Parakeet, Vad, Llama };
enum class LocalModelVendor : std::uint8_t { OpenAI, Nvidia, Silero };

/**
 * A model Vicinae can download and run itself. Entries with no capability are auxiliary files
 * (e.g. voice activity detection) pulled in by another model and never shown to the user.
 */
struct LocalModelInfo {
  std::string_view id;
  std::string_view name;
  const char *description;
  const char *languages = nullptr;
  LocalEngine engine;
  LocalModelVendor vendor;
  AI::Capabilities caps = 0;
  std::string_view repo;
  std::string_view revision;
  std::string_view file;
  std::string_view quantization;
  std::uint64_t size;
  std::string_view sha256;
};

namespace LocalModelCatalogue {

constexpr auto TRANSLATION_CONTEXT = "LocalModelCatalogue";

// Registered with lupdate as an alias of QT_TRANSLATE_NOOP in the translations target.
#define LOCAL_MODEL_TR(text) QT_TRANSLATE_NOOP("LocalModelCatalogue", text)

constexpr auto WHISPER_REPO = std::string_view("ggerganov/whisper.cpp");
constexpr auto WHISPER_REVISION = std::string_view("5359861c739e955e79d9a303bcbc70fb988958b1");
constexpr auto PARAKEET_REPO = std::string_view("ggml-org/parakeet-GGUF");
constexpr auto PARAKEET_REVISION = std::string_view("35156454d1a39de06863303dd209fd2bed6ee079");
constexpr auto VAD_REPO = std::string_view("ggml-org/whisper-vad");
constexpr auto VAD_REVISION = std::string_view("9ffd54a1e1ee413ddf265af9913beaf518d1639b");

// clang-format off
constexpr auto ENTRIES = std::to_array<LocalModelInfo>({
  {
    .id = "whisper-large-v3-turbo-q5_0",
    .name = "Whisper Large v3 Turbo (compact)",
    .description = LOCAL_MODEL_TR("Recommended. Near large-v3 accuracy in every language, several times faster. Needs a GPU for comfortable dictation."),
        .languages = LOCAL_MODEL_TR("About 100 languages"),
    .engine = LocalEngine::Whisper,
    .vendor = LocalModelVendor::OpenAI,
    .caps = AI::Capability::Transcription,
    .repo = WHISPER_REPO,
    .revision = WHISPER_REVISION,
    .file = "ggml-large-v3-turbo-q5_0.bin",
        .quantization = "q5_0",
    .size = 574041195,
    .sha256 = "394221709cd5ad1f40c46e6031ca61bce88931e6e088c188294c6d5a55ffa7e2",
  },
  {
    .id = "whisper-large-v3-turbo",
    .name = "Whisper Large v3 Turbo",
    .description = LOCAL_MODEL_TR("Full precision turbo. Marginally better than the compact file at three times the size."),
        .languages = LOCAL_MODEL_TR("About 100 languages"),
    .engine = LocalEngine::Whisper,
    .vendor = LocalModelVendor::OpenAI,
    .caps = AI::Capability::Transcription,
    .repo = WHISPER_REPO,
    .revision = WHISPER_REVISION,
    .file = "ggml-large-v3-turbo.bin",
        .quantization = "f16",
    .size = 1624555275,
    .sha256 = "1fc70f774d38eb169993ac391eea357ef47c88757ef72ee5943879b7e8e2bc69",
  },
  {
    .id = "whisper-large-v3-q5_0",
    .name = "Whisper Large v3",
    .description = LOCAL_MODEL_TR("Best accuracy outside English. Slower than turbo, GPU strongly recommended."),
        .languages = LOCAL_MODEL_TR("About 100 languages"),
    .engine = LocalEngine::Whisper,
    .vendor = LocalModelVendor::OpenAI,
    .caps = AI::Capability::Transcription,
    .repo = WHISPER_REPO,
    .revision = WHISPER_REVISION,
    .file = "ggml-large-v3-q5_0.bin",
        .quantization = "q5_0",
    .size = 1081140203,
    .sha256 = "d75795ecff3f83b5faa89d1900604ad8c780abd5739fae406de19f23ecd98ad1",
  },
  {
    .id = "whisper-small-q5_1",
    .name = "Whisper Small",
    .description = LOCAL_MODEL_TR("Lightweight multilingual model. Usable on CPU-only machines with a good microphone."),
        .languages = LOCAL_MODEL_TR("About 100 languages"),
    .engine = LocalEngine::Whisper,
    .vendor = LocalModelVendor::OpenAI,
    .caps = AI::Capability::Transcription,
    .repo = WHISPER_REPO,
    .revision = WHISPER_REVISION,
    .file = "ggml-small-q5_1.bin",
        .quantization = "q5_1",
    .size = 190085487,
    .sha256 = "ae85e4a935d7a567bd102fe55afc16bb595bdb618e11b2fc7591bc08120411bb",
  },
  {
    .id = "whisper-small.en-q5_1",
    .name = "Whisper Small (English)",
    .description = LOCAL_MODEL_TR("Lightweight English-only model. Slightly more accurate than the multilingual small model for English."),
        .languages = LOCAL_MODEL_TR("English"),
    .engine = LocalEngine::Whisper,
    .vendor = LocalModelVendor::OpenAI,
    .caps = AI::Capability::Transcription,
    .repo = WHISPER_REPO,
    .revision = WHISPER_REVISION,
    .file = "ggml-small.en-q5_1.bin",
        .quantization = "q5_1",
    .size = 190098681,
    .sha256 = "bfdff4894dcb76bbf647d56263ea2a96645423f1669176f4844a1bf8e478ad30",
  },
  {
    .id = "parakeet-tdt-0.6b-v3-q8_0",
    .name = "Parakeet v3",
    .description = LOCAL_MODEL_TR("Very fast, comfortable on CPU. English and 24 other European languages, detected automatically."),
        .languages = LOCAL_MODEL_TR("English and 24 European languages"),
    .engine = LocalEngine::Parakeet,
    .vendor = LocalModelVendor::Nvidia,
    .caps = AI::Capability::Transcription,
    .repo = PARAKEET_REPO,
    .revision = PARAKEET_REVISION,
    .file = "ggml-parakeet-tdt-0.6b-v3-q8_0.bin",
        .quantization = "q8_0",
    .size = 668757119,
    .sha256 = "4d64e9e96c2792186d072fde0034df0ad670cf680a2f53069052ead827fd600e",
  },
  {
    .id = "parakeet-tdt-0.6b-v3-f16",
    .name = "Parakeet v3 (full precision)",
    .description = LOCAL_MODEL_TR("Full precision Parakeet. Same languages, twice the size of the compact file."),
        .languages = LOCAL_MODEL_TR("English and 24 European languages"),
    .engine = LocalEngine::Parakeet,
    .vendor = LocalModelVendor::Nvidia,
    .caps = AI::Capability::Transcription,
    .repo = PARAKEET_REPO,
    .revision = PARAKEET_REVISION,
    .file = "ggml-parakeet-tdt-0.6b-v3-f16.bin",
        .quantization = "f16",
    .size = 1255897319,
    .sha256 = "833bffc9513b2cae867ee9e51633cfd11e4d51aaa5597c8ac02159385a2b426f",
  },
  {
    .id = "silero-vad-v6.2.0",
    .name = "Silero VAD",
    .description = LOCAL_MODEL_TR("Voice activity detector used to trim silence before transcription."),
        .languages = LOCAL_MODEL_TR("Any"),
    .engine = LocalEngine::Vad,
    .vendor = LocalModelVendor::Silero,
    .repo = VAD_REPO,
    .revision = VAD_REVISION,
    .file = "ggml-silero-v6.2.0.bin",
        .quantization = "f16",
    .size = 885098,
    .sha256 = "2aa269b785eeb53a82983a20501ddf7c1d9c48e33ab63a41391ac6c9f7fb6987",
  },
});
// clang-format on

constexpr std::span<const LocalModelInfo> entries() { return ENTRIES; }

constexpr const LocalModelInfo *find(std::string_view id) {
  auto it = std::ranges::find(ENTRIES, id, &LocalModelInfo::id);
  return it == ENTRIES.end() ? nullptr : &*it;
}

constexpr bool hasUniqueIds() {
  for (std::size_t i = 0; i < ENTRIES.size(); ++i) {
    for (std::size_t j = i + 1; j < ENTRIES.size(); ++j) {
      if (ENTRIES[i].id == ENTRIES[j].id || ENTRIES[i].file == ENTRIES[j].file) return false;
    }
  }
  return true;
}

constexpr bool hasValidChecksums() {
  return std::ranges::all_of(ENTRIES,
                             [](const LocalModelInfo &m) { return m.sha256.size() == 64 && m.size > 0; });
}

static_assert(hasUniqueIds(), "local model catalogue ids and file names must be unique");
static_assert(hasValidChecksums(), "local model catalogue entries need a sha256 and a size");

inline ImageURL vendorIcon(LocalModelVendor vendor) {
  switch (vendor) {
  case LocalModelVendor::OpenAI:
    return ImageURL::builtin(BuiltinIcon::Openai)
        .setBackgroundTint(QColor(Qt::white))
        .setFill(QColor(Qt::black));
  case LocalModelVendor::Nvidia:
    return ImageURL::builtin(BuiltinIcon::Microphone)
        .setBackgroundTint(QColor(0x76, 0xB9, 0x00))
        .setFill(QColor(Qt::white));
  case LocalModelVendor::Silero:
    return ImageURL::builtin(BuiltinIcon::SpeakerOn)
        .setBackgroundTint(QColor(0x6B, 0x72, 0x80))
        .setFill(QColor(Qt::white));
  }
  return ImageURL::builtin(BuiltinIcon::Microphone);
}

inline QString translatedDescription(const LocalModelInfo &model) {
  return QCoreApplication::translate(TRANSLATION_CONTEXT, model.description);
}

} // namespace LocalModelCatalogue

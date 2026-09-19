#include "local-provider.hpp"
#include <ranges>
#include <qlogging.h>
#include <qtconcurrentrun.h>
#include <ggml-backend.h>
#include <string>
#include "local-chat-completion.hpp"
#include "parakeet.h"
#include "ui/image/image-url.hpp"
#include "ui/image/url.hpp"
#include "utils.hpp"
#include "whisper.h"

namespace AI {

namespace {

TranscriptionResult runParakeet(const Audio::Recording &recording, const std::filesystem::path &path) {
  ggml_backend_load_all();
  parakeet_full_params fparams =
      parakeet_full_default_params(parakeet_sampling_strategy::PARAKEET_SAMPLING_GREEDY);
  auto ctx = parakeet_init_from_file_with_params(path.string().c_str(), parakeet_context_default_params());

  qDebug() << "transcribing using parakeet full, model" << path;

  if (parakeet_full(ctx, fparams, recording.toF32().data(), recording.toF32().size()) != 0) {
    return std::unexpected("Failed to transcribe");
  }

  qDebug() << "Transcription is done.";

  const int n_segments = parakeet_full_n_segments(ctx);
  std::string text{};

  for (int i = 0; i < n_segments; ++i) {
    text += parakeet_full_get_segment_text(ctx, i);
  }

  parakeet_free(ctx);

  return TranscriptionResponse{.text = std::move(text)};
}

TranscriptionResult runWhisper(const Audio::Recording &recording, const std::filesystem::path &path,
                               const std::optional<std::string> &language, bool useGpu,
                               const std::string &initialPrompt) {
  ggml_backend_load_all();
  whisper_context_params params = whisper_context_default_params();

  params.use_gpu = useGpu;

  whisper_context *ctx = whisper_init_from_file_with_params(path.string().c_str(), params);
  whisper_full_params fparams =
      whisper_full_default_params(whisper_sampling_strategy::WHISPER_SAMPLING_BEAM_SEARCH);
  fparams.language = language ? language->c_str() : "auto";
  fparams.initial_prompt = initialPrompt.c_str();

  qDebug() << "transcribing using whisper full, model" << path;
  qDebug() << "initial prompt" << fparams.initial_prompt;

  if (whisper_full(ctx, fparams, recording.toF32().data(), recording.toF32().size()) != 0) {
    return std::unexpected("Failed to transcribe");
  }

  qDebug() << "Transcription is done.";

  const int n_segments = whisper_full_n_segments(ctx);
  std::string text{};

  for (int i = 0; i < n_segments; ++i) {
    text += whisper_full_get_segment_text(ctx, i);
  }

  TranscriptionResponse response{.text = std::move(text)};
  if (const char *lang = whisper_lang_str(whisper_full_lang_id(ctx))) response.language = lang;

  whisper_free(ctx);

  return response;
}

} // namespace

LocalProvider::LocalProvider(LocalModelRegistry &registry)
    : m_registry(registry),
      m_description(tr("Vicinae-managed models, mostly for dictation purposes.").toStdString()) {
  m_progressThrottle.setSingleShot(true);
  m_progressThrottle.setInterval(PROGRESS_THROTTLE_MS);
  connect(&m_progressThrottle, &QTimer::timeout, this, &AbstractProvider::managedModelsChanged);
}

std::optional<ImageUrl> LocalProvider::icon() const {
  return ImageUrl{ImageURL::local(QStringLiteral(":/icons/vicinae.png"))};
}

void LocalProvider::start() {
  connect(&m_registry, &LocalModelRegistry::modelsChanged, this, &AbstractProvider::modelsUpdated);
  connect(&m_registry, &LocalModelRegistry::modelsChanged, this, &AbstractProvider::managedModelsChanged);
  connect(&m_registry, &LocalModelRegistry::downloadProgress, this, [this]() {
    if (!m_progressThrottle.isActive()) m_progressThrottle.start();
  });
}

std::vector<ManagedModel> LocalProvider::managedModels() const {
  std::vector<ManagedModel> models;
  const auto available = m_registry.models();
  models.reserve(available.size());

  for (const auto &model : available) {
    const auto &info = model.info;

    ManagedModel managed{
        .id = std::string(info.id),
        .name = std::string(info.name),
        .description = LocalModelCatalogue::translatedDescription(info).toStdString(),
        .icon = ImageUrl{LocalModelCatalogue::vendorIcon(info.vendor)},
        .caps = info.caps,
        .size = info.size,
        .precision = std::string(info.quantization),
        .state = model.installed ? ManagedModel::State::Installed : ManagedModel::State::Absent,
    };
    if (info.languages) {
      managed.languages =
          QCoreApplication::translate(LocalModelCatalogue::TRANSLATION_CONTEXT, info.languages).toStdString();
    }
    if (auto *download = m_registry.activeDownload(info.id)) {
      managed.state = ManagedModel::State::Downloading;
      if (download->bytesTotal() > 0) {
        managed.progress =
            static_cast<double>(download->bytesReceived()) / static_cast<double>(download->bytesTotal());
      }
    }
    models.emplace_back(std::move(managed));
  }
  return models;
}

std::expected<void, std::string> LocalProvider::downloadModel(std::string_view id) {
  return m_registry.download(id).transform([](auto *) {});
}

void LocalProvider::cancelDownload(std::string_view id) { m_registry.cancelDownload(id); }

std::expected<void, std::string> LocalProvider::removeModel(std::string_view id) {
  return m_registry.remove(id);
}

ModelList LocalProvider::listModels(const ListModelFilters &filters) const {
  ModelList models;
  const auto available = m_registry.models(filters.caps);
  models.reserve(available.size());

  for (const auto &model : available) {
    if (!model.installed) continue;
    models.emplace_back(toModel(model.info));
  }

  return models;
}

// Deliberately never chosen as a fallback until local transcription is implemented.
std::optional<Model> LocalProvider::findBestModel(Capabilities caps, Preference) const {
  if (auto models = m_registry.models(caps); !models.empty()) return toModel(models.front().info);
  return std::nullopt;
}

std::shared_ptr<AbstractChatCompletionStream>
LocalProvider::createChatCompletion(std::string_view id, const ChatCompletionPayload &payload) {
  auto model = m_registry.model(id);
  if (!model || !model->installed || model->info.engine != LocalEngine::Llama) return nullptr;
  return std::make_shared<LocalChatCompletion>(m_registry.pathFor(model->info), payload);
}

void LocalProvider::preloadModel(std::string_view modelId) {
  auto model = m_registry.model(modelId);

  if (!model) return;

  constexpr auto isTranscriptionEngine = [](LocalEngine engine) {
    return engine == LocalEngine::Parakeet || engine == LocalEngine::Whisper;
  };

  if (isTranscriptionEngine(model->info.engine)) {
    // TODO: preload transcription context for this model
  }
}

void LocalProvider::configure(const ProviderFields &fields) {
  const auto keepLoaded = fields.string("keepLoaded");

  m_useGpu = fields.boolean("useGpu", true);

  if (keepLoaded == "always") {
    m_keepLoaded.reset();
  } else {
    m_keepLoaded = std::chrono::seconds{qStringFromStdView(fields.string("keepLoaded")).toUInt()};
  }
}

QFuture<TranscriptionResult> LocalProvider::transcribe(Audio::Recording recording,
                                                       const TranscriptionOptions &opts) {
  if (!opts.model)
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected("No model was specified"));

  auto model = m_registry.model(*opts.model);

  if (!model) {
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected("Model could not be found"));
  }

  auto path = m_registry.pathFor(model->info);

  // TODO: move this in its own process in order to avoid crashing Vicinae if for some reason
  // whisper crashes. Also, we need to keep the context alive in order to avoid cold starts every time
  // like it is the case right now. But we don't want to keep it initialized at all times, as it can be
  // withold a lot of resources.

  switch (model->info.engine) {
  case LocalEngine::Parakeet:
    return QtConcurrent::run([recording = std::move(recording), path = std::move(path)]() {
      return runParakeet(recording, path);
    });
  case LocalEngine::Whisper: {
    std::string initialPrompt;

    for (const auto &word : opts.vocabulary) {
      if (!initialPrompt.empty()) initialPrompt += ", ";
      initialPrompt += word;
    }

    return QtConcurrent::run([recording = std::move(recording), path = std::move(path),
                              language = opts.language, useGpu = opts.useGpu.value_or(m_useGpu),
                              initialPrompt = std::move(initialPrompt)]() {
      return runWhisper(recording, path, language, useGpu, initialPrompt);
    });
  }
  default:
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(
        std::unexpected("Cannot run inference for this engine"));
  }
}

Model LocalProvider::toModel(const LocalModelInfo &info) {
  return Model{
      .id = std::string(info.id),
      .name = std::string(info.name),
      .description = LocalModelCatalogue::translatedDescription(info).toStdString(),
      .icon = ImageUrl{LocalModelCatalogue::vendorIcon(info.vendor)},
      .caps = info.caps,
  };
}

} // namespace AI

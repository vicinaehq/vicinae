#include "local-provider.hpp"
#include <chrono>
#include <string>
#include <qlogging.h>
#include "ui/image/image-url.hpp"
#include "ui/image/url.hpp"
#include "utils.hpp"

namespace AI {

namespace {

constexpr auto KEEP_LOADED_ALWAYS = "always";

bool isTranscriptionEngine(LocalEngine engine) {
  return engine == LocalEngine::Whisper || engine == LocalEngine::Parakeet;
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

void LocalProvider::configure(const ProviderFields &fields) {
  const auto keepLoaded = fields.string("keepLoaded");
  if (keepLoaded == KEEP_LOADED_ALWAYS) {
    m_inference.setIdleTimeout(std::nullopt);
  } else {
    m_inference.setIdleTimeout(std::chrono::seconds{qStringFromStdView(keepLoaded).toUInt()});
  }
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

std::optional<Model> LocalProvider::findBestModel(Capabilities caps, Preference) const {
  if (auto models = m_registry.models(caps); !models.empty()) return toModel(models.front().info);
  return std::nullopt;
}

std::shared_ptr<AbstractChatCompletionStream>
LocalProvider::createChatCompletion(std::string_view, const ChatCompletionPayload &) {
  return nullptr;
}

void LocalProvider::preloadModel(std::string_view modelId) {
  auto model = m_registry.model(modelId);
  if (!model || !model->installed || !isTranscriptionEngine(model->info.engine)) return;

  m_inference.preload(model->info.engine, m_registry.pathFor(model->info));
}

void LocalProvider::cancelPreload(std::string_view) { m_inference.cancelPreload(); }

QFuture<TranscriptionResult> LocalProvider::transcribe(Audio::Recording recording,
                                                       const TranscriptionOptions &opts) {
  if (!opts.model)
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected("No model was specified"));

  auto model = m_registry.model(*opts.model);

  if (!model || !model->installed) {
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(std::unexpected("Model could not be found"));
  }

  InferenceRuntime::Transcription request{
      .engine = model->info.engine,
      .model = m_registry.pathFor(model->info),
      .language = opts.language,
  };

  if (model->info.engine == LocalEngine::Whisper && !opts.vocabulary.empty()) {
    std::string prompt;
    for (const auto &word : opts.vocabulary) {
      if (!prompt.empty()) prompt += ", ";
      prompt += word;
    }
    request.initialPrompt = std::move(prompt);
  }

  return m_inference.transcribe(recording, request);
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

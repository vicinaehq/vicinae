#include "mistral-provider.hpp"
#include <format>
#include <ranges>
#include <qlogging.h>
#include "services/builtin-icon/builtin-icon.hpp"

namespace AI {

namespace {

Model toModel(const mistral::ListModelsResponse::ModelInfo &info) {
  Model model;
  model.id = info.id;
  model.name = info.name;
  model.description = info.description;

  if (info.capabilities.completion_chat) { model.caps |= Capability::Completion; }
  if (info.capabilities.vision) { model.caps |= Capability::Vision; }
  if (info.capabilities.function_calling) { model.caps |= Capability::ToolCalling; }
  if (info.capabilities.audio_transcription) { model.caps |= Capability::Transcription; }
  if (info.capabilities.ocr) { model.caps |= Capability::OCR; }

  return model;
}

} // namespace

MistralProvider::MistralProvider() {
  m_client.setBaseUrl("https://api.mistral.ai/v1/");
  connect(&m_listWatcher, &decltype(m_listWatcher)::finished, this, &MistralProvider::handleListResult);
}

std::optional<ImageUrl> MistralProvider::icon() const { return ImageUrl{BuiltinIcon::Mistral}; }

void MistralProvider::configure(const ProviderFields &fields) {
  auto apiKey = QString::fromStdString(fields.string("apiKey"));
  if (apiKey == m_apiKey) return;

  m_apiKey = apiKey;
  m_client.setBearer(std::move(apiKey));

  if (!m_started) return;

  m_listWatcher.cancel();
  m_models = {};
  emit modelsUpdated();
  start();
}

void MistralProvider::start() {
  m_started = true;
  m_listWatcher.setFuture(fetchModels());
}

ModelList MistralProvider::listModels(const ListModelFilters &) const {
  return m_models.data | std::views::transform(toModel) |
         std::views::filter([](auto &&m) { return m.id == m.name && m.caps; }) | std::ranges::to<ModelList>();
}

std::optional<Model> MistralProvider::findBestModel(Capabilities caps, Preference) const {
  for (const auto &model : m_models.data) {
    auto m = toModel(model);
    if (m.caps & caps) return m;
  }

  return std::nullopt;
}

std::shared_ptr<AbstractChatCompletionStream>
MistralProvider::createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) {
  StandardChatCompletionPayload p;
  p.model = modelId;
  p.messages = payload.messages;
  p.tools = payload.tools;
  return StandardChatCompletionStream::makeShared(m_client, p);
}

QFuture<TranscriptionResult> MistralProvider::transcribe(Audio::Recording recording,
                                                         const TranscriptionOptions &opts) {
  auto formData = new http::FormData;

  formData->addField("model", QByteArray::fromStdString(opts.model.value_or("voxtral-mini-2507")));
  if (opts.language) formData->addField("language", QByteArray::fromStdString(*opts.language));
  formData->addFile(recording.toWav(), "audio/wav");

  return m_client.post<mistral::TranscriptionResponse>("/audio/transcriptions", formData)
      .then([](Result<mistral::TranscriptionResponse> res) -> TranscriptionResult {
        if (!res) { return std::unexpected(std::format("Transcription failed: {}", res.error())); }
        return TranscriptionResult(res->text);
      });
}

QFuture<Result<mistral::ListModelsResponse>> MistralProvider::fetchModels() {
  return m_client.get<mistral::ListModelsResponse>("/models");
}

void MistralProvider::handleListResult() {
  if (m_listWatcher.isCanceled()) return;

  auto res = m_listWatcher.result();
  if (!res) {
    qWarning() << "Failed to fetch model list from mistral.ai" << res.error();
    return;
  }

  m_models = res.value();
  emit modelsUpdated();
}

} // namespace AI

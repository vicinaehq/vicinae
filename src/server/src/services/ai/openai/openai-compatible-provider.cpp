#include "openai-compatible-provider.hpp"
#include "services/ai/history-compaction.hpp"
#include <algorithm>
#include <format>
#include <ranges>
#include <qlogging.h>

namespace AI {

namespace {
constexpr std::size_t MAX_TRANSCRIPTION_PROMPT_BYTES = 600;
}

OpenAICompatibleProvider::OpenAICompatibleProvider(std::string id, std::string_view type,
                                                   std::string defaultUrl)
    : m_id(std::move(id)), m_info(*findProviderType(type)), m_defaultUrl(std::move(defaultUrl)) {
  connect(&m_listWatcher, &decltype(m_listWatcher)::finished, this, &OpenAICompatibleProvider::handleModels);
  m_listRetry.setSingleShot(true);
  connect(&m_listRetry, &QTimer::timeout, this, &OpenAICompatibleProvider::start);
}

OpenAICompatibleProvider::~OpenAICompatibleProvider() { m_listWatcher.cancel(); }

void OpenAICompatibleProvider::configure(const PreferenceValues &values) {
  auto fields = readPreferences<OpenAICompatibleFields>(values);
  m_name = std::move(fields.name);

  auto url = std::move(fields.url);
  if (url.empty()) url = m_defaultUrl;
  while (url.ends_with('/')) {
    url.pop_back();
  }

  auto apiKey = std::move(fields.apiKey);
  if (url == m_url && apiKey == m_apiKey) return;

  m_url = std::move(url);
  m_apiKey = std::move(apiKey);
  m_client.setBaseUrl(QString::fromStdString(openAIRoot(m_url) + "/"));
  m_client.setBearer(QString::fromStdString(m_apiKey));
  qInfo() << "AI provider" << m_id << "now targets" << m_url;

  if (!m_started) return;

  m_listWatcher.cancel();
  m_listRetry.stop();
  m_listRetryDelay = MIN_LIST_RETRY_DELAY;
  m_models.clear();
  emit modelsUpdated();
  start();
}

void OpenAICompatibleProvider::start() {
  m_started = true;
  m_listWatcher.setFuture(fetchModels());
}

ModelList OpenAICompatibleProvider::listModels(const ListModelFilters &filters) const {
  return m_models | std::views::filter([&](const Model &model) {
           return !filters.caps || (model.caps & *filters.caps);
         }) |
         std::ranges::to<ModelList>();
}

std::optional<Model> OpenAICompatibleProvider::findBestModel(Capabilities caps, Preference preference) const {
  // the model list says nothing about which ones are fast or cheap
  if (preference != Preference::None) return std::nullopt;
  for (const auto &model : m_models) {
    if (model.caps & caps) return model;
  }
  return std::nullopt;
}

std::shared_ptr<AbstractChatCompletionStream>
OpenAICompatibleProvider::createChatCompletion(std::string_view modelId,
                                               const ChatCompletionPayload &payload) {
  Model model;
  if (auto it = std::ranges::find(m_models, modelId, &Model::id); it != m_models.end()) model = *it;

  auto compacted = payload;
  compacted.messages = compactHistory(payload.messages);

  auto request = makeRequest(modelId, model, compacted);
  return openai::ChatCompletionStream::makeShared(m_client, std::move(request), std::move(model));
}

void OpenAICompatibleProvider::addTranscriptionFields(http::FormData &form, std::string_view,
                                                      const TranscriptionOptions &options) const {
  if (options.language) form.addField("language", QByteArray::fromStdString(*options.language));

  // limited to 224 tokens, whatever the words: the newest entries go last, so those are the ones kept
  std::string prompt;
  for (const auto word : options.vocabulary | std::views::reverse) {
    if (prompt.size() + word.size() + 2 > MAX_TRANSCRIPTION_PROMPT_BYTES) break;
    prompt.insert(0, prompt.empty() ? std::string(word) : std::format("{}, ", word));
  }
  if (!prompt.empty()) form.addField("prompt", QByteArray::fromStdString(prompt));
}

QFuture<TranscriptionResult> OpenAICompatibleProvider::transcribe(Audio::Recording recording,
                                                                  const TranscriptionOptions &opts) {
  auto model = opts.model;
  if (!model) {
    if (auto best = findBestModel(Capability::Transcription)) model = best->id;
  }
  if (!model) {
    return QtFuture::makeReadyValueFuture<TranscriptionResult>(
        std::unexpected("This provider has no transcription model"));
  }

  auto *formData = new http::FormData;
  formData->addField("model", QByteArray::fromStdString(*model));
  addTranscriptionFields(*formData, *model, opts);
  formData->addFile(recording.toWav(), "audio/wav", "file", "audio.wav");

  return m_client.post<openai::TranscriptionResponse>("audio/transcriptions", formData)
      .then([](Result<openai::TranscriptionResponse> res) -> TranscriptionResult {
        if (!res) return std::unexpected(std::format("Transcription failed: {}", res.error()));
        return TranscriptionResponse{.text = std::move(res->text), .language = std::move(res->language)};
      });
}

QFuture<Result<ModelList>> OpenAICompatibleProvider::fetchModels() {
  return m_client.get<openai::ModelListResponse>("models").then(
      this, [this](Result<openai::ModelListResponse> res) -> Result<ModelList> {
        if (!res) return std::unexpected(res.error());

        ModelList models;
        models.reserve(res->data.size());
        for (auto &entry : res->data) {
          const auto caps = capabilitiesFor(entry.id);
          if (!caps) continue;
          models.emplace_back(Model{.id = entry.id, .name = std::move(entry.id), .caps = caps});
        }
        return models;
      });
}

void OpenAICompatibleProvider::handleModels() {
  if (m_listWatcher.isCanceled()) return;

  auto res = m_listWatcher.result();
  if (!res) {
    qWarning() << "Failed to fetch models for AI provider" << m_id << ":" << res.error();
    // the network or a local server may just not be up yet, as happens when logging in
    m_listRetry.start(m_listRetryDelay);
    m_listRetryDelay = std::min<std::chrono::seconds>(m_listRetryDelay * 2, MAX_LIST_RETRY_DELAY);
    return;
  }

  m_listRetryDelay = MIN_LIST_RETRY_DELAY;
  m_models = std::move(*res);
  emit modelsUpdated();
}

} // namespace AI

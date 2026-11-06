#include "audiocpp-provider.hpp"
#include <ranges>
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

namespace {

Model toModel(const audiocpp::ListModelsResponse::Entry &entry) {
  Model model;
  model.id = entry.id;
  model.name = entry.id;
  model.icon = ImageUrl{BuiltinIcon::Waveform};
  model.caps = Capability::Transcription;
  return model;
}

} // namespace

AudioCppProvider::AudioCppProvider(std::string id)
    : OpenAICompatibleProvider(std::move(id), "audio-cpp", "http://localhost:8080") {}

void AudioCppProvider::addTranscriptionFields(http::FormData &form, std::string_view,
                                              const TranscriptionOptions &options) const {
  if (options.language) form.addField("language", QByteArray::fromStdString(*options.language));
}

QFuture<Result<ModelList>> AudioCppProvider::fetchModels() {
  return client().get<audiocpp::ListModelsResponse>("models").then(
      [](Result<audiocpp::ListModelsResponse> res) -> Result<ModelList> {
        if (!res) return std::unexpected(res.error());
        return res->data | std::views::transform(toModel) | std::ranges::to<ModelList>();
      });
}

} // namespace AI

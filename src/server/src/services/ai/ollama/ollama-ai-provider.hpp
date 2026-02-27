#include "services/ai/ai-provider.hpp"
#include <glaze/json/read.hpp>

namespace AI {

struct OllamaConfig {
  std::string url;
  std::optional<std::string> pwd;
};

class OllamaProvider : public AbstractProvider {
  std::string id() const override { return "ollama"; }

  ModelList listModels(const ListModelFilters &filters = {}) const override { return {}; }

  std::optional<Model> findBestModel(Capabilities caps,
                                     Preference preference = Preference::None) const override {
    return std::nullopt;
  }

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(const ChatCompletionPayload &payload) const override {
    return {};
  }

  bool initalize(const glz::raw_json &json) override {
    if (const auto error = glz::read_json(m_cfg, json.str)) { return false; }
    return true;
  }

private:
  OllamaConfig m_cfg;
};

}; // namespace AI

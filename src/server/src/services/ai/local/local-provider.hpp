#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <qfuture.h>
#include <QTimer>
#include "services/ai/ai-provider.hpp"
#include "services/audio/audio-recorder.hpp"
#include "services/local-model-registry/local-model-catalogue.hpp"
#include "services/local-model-registry/local-model-registry.hpp"

namespace AI {

class LocalProvider : public AbstractProvider {
  Q_OBJECT

public:
  static constexpr std::string_view ID = "local";
  static constexpr int PROGRESS_THROTTLE_MS = 150;

  explicit LocalProvider(LocalModelRegistry &registry);

  std::string id() const override { return std::string(ID); }
  std::string_view type() const override { return ID; }
  std::string displayName() const override { return tr("Built-in").toStdString(); }
  std::optional<ImageUrl> icon() const override;
  std::string_view description() const override { return m_description; }

  void configure(const ProviderFields &) override;
  void start() override;

  bool managesModels() const override { return true; }
  std::vector<ManagedModel> managedModels() const override;
  std::expected<void, std::string> downloadModel(std::string_view id) override;
  void cancelDownload(std::string_view id) override;
  std::expected<void, std::string> removeModel(std::string_view id) override;

  ModelList listModels(const ListModelFilters &filters = {}) const override;
  std::optional<Model> findBestModel(Capabilities caps, Preference = Preference::None) const override;

  std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view id, const ChatCompletionPayload &payload) override;
  void preloadModel(std::string_view modelId) override;
  QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                          const TranscriptionOptions &opts = {}) override;

private:
  static Model toModel(const LocalModelInfo &info);

  LocalModelRegistry &m_registry;
  std::string m_description;
  QTimer m_progressThrottle;
  bool m_useGpu = true;

  // std::nullopt if we want to keep it loaded at all times
  std::optional<std::chrono::seconds> m_keepLoaded;
};

} // namespace AI

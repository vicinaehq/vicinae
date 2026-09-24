#pragma once
#include "ai-capability.hpp"
#include "ai-tool.hpp"
#include <cstdint>
#include <expected>
#include <glaze/core/common.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/generic.hpp>
#include <optional>
#include <qdir.h>
#include <qfuture.h>
#include <qimage.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include "command/preference.hpp"
#include "common/context.hpp"
#include "common/qt.hpp"
#include "services/audio/audio-recorder.hpp"
#include "ui/image/image-url.hpp"

namespace AI {

template <typename T> using Result = std::expected<T, std::string>;

struct Model {
  std::string id;
  std::string name;
  std::optional<std::string> description;
  std::optional<ImageUrl> icon;
  Capabilities caps = 0;
};

struct ModelRef {
  static std::expected<ModelRef, std::string> fromString(std::string_view model) {
    auto pos = model.find(':');

    if (pos == std::string::npos) {
      return std::unexpected("Expected at least one ':' to separate provider from model id");
    }

    return ModelRef(std::string{model.substr(0, pos)}, std::string{model.substr(pos + 1)});
  }

  std::string toString() const {
    std::string s;
    s.reserve(provider.size() + id.size() + 1);
    return s.append(provider).append(":").append(id);
  }

  std::string provider;
  std::string id;
};

struct ProviderModel : public Model {
  ModelRef ref;
  bool enabled = true;

  ProviderModel(std::string providerId, Model &&model)
      : Model(std::move(model)), ref{std::move(providerId), this->id} {}
};

inline std::vector<std::string> stringifyCapabilities(Capabilities caps) {
  std::vector<std::string> strs;

  if (caps & Completion) strs.emplace_back("completion");
  if (caps & Vision) strs.emplace_back("vision");
  if (caps & Thinking) strs.emplace_back("thinking");
  if (caps & ToolCalling) strs.emplace_back("tools");
  if (caps & Embedding) strs.emplace_back("embedding");
  if (caps & Transcription) strs.emplace_back("transcription");

  return strs;
}

/**
 * Indicates what kind of model is preferred for the current task at hand.
 * This is generally most about speed/cost.
 * Providers may ignore this completely.
 */
enum class Preference { None, Fast, Reasoning };

using ModelList = std::vector<Model>;

struct ListModelFilters {
  std::optional<Capabilities> caps;
  std::optional<int> limit;
};

enum class ChatRole { System, User, Assistant, Tool, Developer };

constexpr std::string_view roleName(ChatRole role) {
  switch (role) {
  case ChatRole::System:
    return "system";
  case ChatRole::User:
    return "user";
  case ChatRole::Assistant:
    return "assistant";
  case ChatRole::Tool:
    return "tool";
  case ChatRole::Developer:
    return "developer";
  }
  return "user";
}

struct TextPart {
  std::string text;
};

struct ImagePart {
  std::string mimeType;
  QByteArray base64;
};

struct ToolCallPart {
  std::string id;
  std::string name;
  std::string arguments;
};

struct ToolResultPart {
  std::string callId;
  std::string content;
};

using ChatPart = std::variant<TextPart, ImagePart, ToolCallPart, ToolResultPart>;

/**
 * Tool calls live on assistant messages and their results on tool messages, paired by call id, so a
 * history can be replayed on any provider.
 */
struct ChatMessage {
  ChatRole role;
  std::vector<ChatPart> parts;

  static ChatMessage fromText(ChatRole role, std::string text) {
    ChatMessage message{.role = role};
    message.parts.emplace_back(TextPart{std::move(text)});
    return message;
  }

  std::string text() const {
    std::string result;
    for (const auto &part : parts) {
      if (const auto *text = std::get_if<TextPart>(&part)) result += text->text;
    }
    return result;
  }
};

using ChatHistory = std::vector<ChatMessage>;

class AbstractChatCompletionStream : public QObject {
  Q_OBJECT

signals:
  void toolCallRequested(const ToolCallPart &call) const;
  void dataAdded(const std::string &text) const;
  void errorOccurred(const std::string &reason) const;
  void finished() const;

public:
  ~AbstractChatCompletionStream() override = default;
  virtual bool start() = 0;
  virtual bool abort() = 0;

  const Model &model() const { return m_model; }

protected:
  void setModel(Model model) { m_model = std::move(model); }

private:
  Model m_model;
};

enum class ThinkingMode { None, Low, Medium, High };

struct ChatCompletionPayload {
  /**
   * How much thinking the model should do before answering, if applicable to the selected model.
   * Some providers may not support this at all.
   */
  ThinkingMode thinking = ThinkingMode::Medium;
  ChatHistory messages;
  std::optional<float> temperature;
  std::vector<AbstractTool *> tools;
};

struct TranscriptionOptions {
  std::optional<std::string> model;

  // language to use. Some providers need the language in order
  // to operate properly, others don't care and automatically
  // detect it.
  std::optional<std::string> language;

  // list of words we should bias towards.
  // Some class of models (such as parakeet models) cannot be
  // biased. Non-owning: only valid for the duration of the call.
  std::span<const std::string_view> vocabulary;
};

struct TranscriptionResponse {
  std::string text;
  // ISO 639-1 code when the engine knows or detected it
  std::optional<std::string> language;
};

using TranscriptionResult = std::expected<TranscriptionResponse, std::string>;

/**
 * A model a provider can install on the user's behalf, with its current state.
 */
struct ManagedModel {
  enum class State : std::uint8_t { Absent, Downloading, Installed };

  std::string id;
  std::string name;
  std::string description;
  std::optional<ImageUrl> icon;
  Capabilities caps = 0;
  std::uint64_t size = 0;
  std::string precision;
  std::string languages;
  State state = State::Absent;
  double progress = -1.0;
};

class AbstractProvider : public QObject {
  Q_OBJECT

signals:
  void modelsUpdated() const;
  void managedModelsChanged() const;

public:
  AbstractProvider() = default;
  ~AbstractProvider() override = default;

  /**
   * Unique identifier for this provider.
   */
  virtual std::string id() const = 0;

  /**
   * Entry of `PROVIDER_TYPES` this instance was created from.
   */
  virtual std::string_view type() const = 0;

  /**
   * Called once before `start` and again on every settings change. Providers are never recreated
   * for a settings change, so this is where they refresh and drop work started under old values.
   */
  virtual void configure(const PreferenceValues &fields) = 0;

  /**
   * Name shown to the user. Defaults to the id.
   */
  virtual std::string displayName() const { return id(); }

  /**
   * Providers that download and store models themselves expose their catalogue here. The catalogue
   * includes models that are not installed yet; `listModels` only ever returns usable ones.
   */
  virtual bool managesModels() const { return false; }
  virtual std::vector<ManagedModel> managedModels() const { return {}; }
  virtual std::expected<void, std::string> downloadModel(std::string_view) {
    return std::unexpected("This provider does not manage models");
  }
  virtual void cancelDownload(std::string_view) {}
  virtual std::expected<void, std::string> removeModel(std::string_view) {
    return std::unexpected("This provider does not manage models");
  }

  /**
   * An icon representing the provider, if applicable.
   */
  virtual std::optional<ImageUrl> icon() const = 0;

  /**
   * A human-readable description of this provider.
   */
  virtual std::string_view description() const = 0;

  virtual void start() = 0;

  /**
   * A request to preload said model, if applicable.
   *
   * Typically used with dictation models: when the recorder starts,
   * a preload request is sent so that the audio can be transcribed
   * without suffering from cold start.
   *
   * This mostly applies to local inference, cloud models do not need this
   * for obvious reasons.
   *
   */
  virtual void preloadModel(std::string_view modelId) {}

  /**
   * The use announced by `preloadModel` will not happen.
   */
  virtual void cancelPreload(std::string_view modelId) {}

  /**
   * List all models that match the filter's criterias.
   */
  virtual ModelList listModels(const ListModelFilters &filters = {}) const = 0;

  /**
   * Find the best model for the provided set of capabilities and, if relevant, factor in the assigned task
   * preference.
   */
  virtual std::optional<Model> findBestModel(Capabilities caps,
                                             Preference preference = Preference::None) const = 0;

  /**
   * Create a streaming chat completion that can be started by calling the `start` method.
   * Streaming completions will continously emit the `dataAdded` signal with the new data until there is no
   * more data available, in which case `finished` is emitted. The completion can be aborted at anytime by
   * calling `abort`.
   */
  virtual std::shared_ptr<AbstractChatCompletionStream>
  createChatCompletion(std::string_view modelId, const ChatCompletionPayload &payload) = 0;

  virtual QFuture<TranscriptionResult> transcribe(Audio::Recording recording,
                                                  const TranscriptionOptions &opts = {}) = 0;
};

}; // namespace AI

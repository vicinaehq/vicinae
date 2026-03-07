#pragma once
#include <cstdint>
#include <expected>
#include <glaze/core/common.hpp>
#include <optional>
#include <qfuture.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include <vector>
#include "image-url.hpp"

namespace AI {

template <typename T> using Result = std::expected<T, std::string>;

/**
 * What a model is capable of.
 * Some models can do many 2026-03-05T21things at once.
 */
enum Capability : std::uint8_t {
  Completion = 1 << 0,
  Vision = 1 << 1,
  Thinking = 1 << 2,
  ToolCalling = 1 << 3,
  Embedding = 1 << 4,
  Transcription = 1 << 5
};

using Capabilities = std::uint32_t;

struct Model {
  std::string id;
  std::string name;
  std::optional<std::string> description;
  std::optional<ImageUrl> icon;
  Capabilities caps;
};

struct ProviderModel : public Model {
  std::string providerId;

  ProviderModel(std::string providerId, Model &&model)
      : Model(std::move(model)), providerId(std::move(providerId)) {}
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

class AbstractChatCompletionStream : public QObject {
  Q_OBJECT

signals:
  void dataAdded(const std::string &text) const;
  void errorOccured(const std::string &reason) const;
  void finished() const;

public:
  ~AbstractChatCompletionStream() override = default;
  virtual bool start() = 0;
  virtual bool abort() = 0;
};

enum class ChatRole { System, User, Assistant };

struct ChatMessage {
  ChatRole role;
  std::string value;
};

using ChatHistory = std::vector<ChatMessage>;

struct ChatCompletionPayload {
  std::string modelId;
  ChatHistory messages;
};

struct TranscriptionResult {
  std::string text;
};

class AbstractProvider : public QObject {
  Q_OBJECT

signals:
  void modelsUpdated() const;

public:
  AbstractProvider() = default;
  ~AbstractProvider() override = default;

  /**
   * Unique identifier for this provider.
   */
  virtual std::string id() const = 0;

  /**
   * An icon representing the provider, if applicable.
   */
  virtual std::optional<ImageUrl> icon() const = 0;

  virtual void start() = 0;

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
  createChatCompletion(const ChatCompletionPayload &payload) const = 0;

  virtual QFuture<TranscriptionResult> transcribe() const = 0;
};

}; // namespace AI

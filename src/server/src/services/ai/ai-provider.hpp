#pragma once
#include <cstdint>
#include <glaze/core/common.hpp>
#include <optional>
#include <qfuture.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>
#include <vector>
#include "image-url.hpp"

namespace AI {

/**
 * What a model is capable of.
 * Some models can often do many things at once.
 */
enum Capability : std::uint8_t { Chat, Vision, Transcription };

using Capabilities = std::uint32_t;

struct Model {
  std::string id;
  std::string name;
  std::optional<std::string> description;
  std::optional<ImageUrl> icon;
  Capabilities caps;
};

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

class AbstractProvider {
public:
  AbstractProvider() = default;
  virtual ~AbstractProvider() = default;

  /**
   * Unique identifier for this provider.
   */
  virtual std::string id() const = 0;

  /**
   * An icon representing the provider, if applicable.
   */
  virtual std::optional<ImageUrl> icon() const = 0;

  /**
   * TODO: maybe don't use initialize pattern, but it is quite convenient.
   *
   * Initialize provider from generic json used for configuration.
   * The shape of the object is arbitrary and will be different from provider to provider.
   * If this method returns false, it is assumed that the provider failed to initialize, and the AI service
   * will not consider it.
   */
  virtual bool initalize(const glz::raw_json &json) = 0;

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

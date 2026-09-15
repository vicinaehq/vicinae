#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <filesystem>
#include <format>
#include <memory>
#include <string>
#include <vector>
#include <QtConcurrent>
#include <llama.h>
#include <ggml-backend.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include "services/ai/ai-provider.hpp"

// Chat completion using llama.cpp. Loads the model per call; keeping it warm belongs to the helper
// process. No tool support, this is meant for small fixed-purpose models.
class LocalChatCompletion : public AI::AbstractChatCompletionStream {
public:
  LocalChatCompletion(std::filesystem::path model, AI::ChatCompletionPayload payload)
      : m_model(std::move(model)), m_payload(std::move(payload)) {}

  ~LocalChatCompletion() override {
    m_aborted = true;
    m_completion.waitForFinished();
  }

  bool start() override {
    m_completion.setFuture(QtConcurrent::run([this]() { runCompletion(); }));
    return true;
  }

  bool abort() override {
    m_aborted = true;
    return true;
  }

private:
  // Qwen3 chat template output for enable_thinking=false. llama_chat_apply_template has no
  // template kwargs, so the marker is appended by hand.
  static constexpr auto NO_THINKING_MARKER = "<think>\n\n</think>\n\n";
  static constexpr int MAX_PREDICT = 2048;

  struct ModelDeleter {
    void operator()(llama_model *model) const { llama_model_free(model); }
  };
  struct ContextDeleter {
    void operator()(llama_context *ctx) const { llama_free(ctx); }
  };
  struct SamplerDeleter {
    void operator()(llama_sampler *sampler) const { llama_sampler_free(sampler); }
  };

  void fail(std::string reason) {
    QMetaObject::invokeMethod(this, [this, reason = std::move(reason)]() { emit errorOccured(reason); });
  }

  static const char *roleName(AI::ChatRole role) {
    using R = AI::ChatRole;
    switch (role) {
    case R::System:
      return "system";
    case R::User:
      return "user";
    case R::Assistant:
      return "assistant";
    case R::Developer:
      return "developer";
    case R::Tool:
      return "tool";
    }
    return "user";
  }

  void runCompletion() {
    ggml_backend_load_all();

    auto mparams = llama_model_default_params();
    mparams.n_gpu_layers = 99;

    std::unique_ptr<llama_model, ModelDeleter> model(llama_model_load_from_file(m_model.c_str(), mparams));
    if (!model) return fail(std::format("Failed to load model from file: {}", m_model.string()));
    if (m_aborted) return;

    std::vector<llama_chat_message> msgs;
    msgs.reserve(m_payload.messages.size());
    for (const auto &msg : m_payload.messages) {
      msgs.emplace_back(roleName(msg.role), msg.value.c_str());
    }

    const char *tmpl = llama_model_chat_template(model.get(), nullptr);
    std::vector<char> buf(4096);
    int len = llama_chat_apply_template(tmpl, msgs.data(), msgs.size(), true, buf.data(), buf.size());
    if (len > static_cast<int>(buf.size())) {
      buf.resize(static_cast<std::size_t>(len));
      len = llama_chat_apply_template(tmpl, msgs.data(), msgs.size(), true, buf.data(), buf.size());
    }
    if (len < 0) return fail("Failed to apply chat template");

    std::string prompt(buf.data(), static_cast<std::size_t>(len));
    prompt += NO_THINKING_MARKER;

    const auto *vocab = llama_model_get_vocab(model.get());
    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);
    if (n_prompt <= 0) return fail("Failed to tokenize prompt");

    std::vector<llama_token> tokens(static_cast<std::size_t>(n_prompt));
    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), tokens.data(), n_prompt, true, true) < 0) {
      return fail("Failed to tokenize prompt");
    }

    // output length tracks input length for rewriting tasks
    const int n_predict = std::min(MAX_PREDICT, static_cast<int>(n_prompt * 1.3) + 32);

    auto cparams = llama_context_default_params();
    cparams.n_ctx = static_cast<std::uint32_t>(n_prompt + n_predict);
    cparams.n_batch = static_cast<std::uint32_t>(n_prompt);

    std::unique_ptr<llama_context, ContextDeleter> ctx(llama_init_from_model(model.get(), cparams));
    if (!ctx) return fail("Failed to initialize llama context");

    std::unique_ptr<llama_sampler, SamplerDeleter> sampler(
        llama_sampler_chain_init(llama_sampler_chain_default_params()));
    llama_sampler_chain_add(sampler.get(), llama_sampler_init_greedy());

    llama_batch batch = llama_batch_get_one(tokens.data(), static_cast<std::int32_t>(tokens.size()));
    // the batch points at this token across iterations
    llama_token tok = 0;

    for (int n = 0; n < n_predict && !m_aborted; ++n) {
      if (llama_decode(ctx.get(), batch) != 0) return fail("Failed to decode");

      tok = llama_sampler_sample(sampler.get(), ctx.get(), -1);
      if (llama_vocab_is_eog(vocab, tok)) break;

      std::array<char, 256> piece{};
      const int pieceLen = llama_token_to_piece(vocab, tok, piece.data(), piece.size(), 0, true);
      if (pieceLen < 0) return fail("Failed to detokenize");

      QMetaObject::invokeMethod(
          this, [this, text = std::string{piece.data(), static_cast<std::size_t>(pieceLen)}]() {
            emit dataAdded(text);
          });

      batch = llama_batch_get_one(&tok, 1);
    }

    if (m_aborted) return;
    QMetaObject::invokeMethod(this, [this]() { emit finished(); });
  }

  std::filesystem::path m_model;
  AI::ChatCompletionPayload m_payload;
  QFutureWatcher<void> m_completion;
  std::atomic<bool> m_aborted = false;
};

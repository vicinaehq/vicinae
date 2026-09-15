#pragma once
#include <filesystem>
#include <QtConcurrent>
#include <format>
#include <llama.h>
#include <ggml-backend.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include "services/ai/ai-provider.hpp"

// chat completion using llama.cpp
// this is experimental, and we don't make use of this yet.
// tool support would be a lot of work to add, so maybe we will only use this for the easy stuff.
class LocalChatCompletion : public AI::AbstractChatCompletionStream {
public:
  LocalChatCompletion(const std::filesystem::path &model, AI::ChatCompletionPayload payload)
      : m_model(model), m_payload(std::move(payload)) {}

  bool start() override {
    m_completion.setFuture(
        QtConcurrent::run([this, path = m_model, payload = m_payload]() { runCompletion(path, payload); }));
    return true;
  }

  bool abort() override {
    // right now, does nothing
    m_completion.cancel();
    return false;
  }

private:
  void runCompletion(std::filesystem::path path, AI::ChatCompletionPayload payload) {
    ggml_backend_load_all();
    auto mparams = llama_model_default_params();
    mparams.n_gpu_layers = 99;

    auto model = llama_model_load_from_file(path.c_str(), mparams);

    if (!model) {
      qDebug() << "Failed to load model" << path.c_str();
      emit errorOccured(std::format("Failed to load model from file: {}", path.c_str()));
      return;
    }

    const char *tmpl = llama_model_chat_template(model, /*name=*/nullptr);

    std::vector<llama_chat_message> msgs{};

    msgs.reserve(payload.messages.size());

    for (const auto &msg : payload.messages) {
      constexpr auto getRole = [](AI::ChatRole role) {
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
      };
      msgs.push_back({getRole(msg.role), msg.value.c_str()});
    }

    std::vector<char> buf(4096);
    int len = llama_chat_apply_template(tmpl, msgs.data(), msgs.size(), /*add_assistant=*/true, buf.data(),
                                        buf.size());
    if (len > (int)buf.size()) { // buffer too small: resize and retry
      buf.resize(len);
      len = llama_chat_apply_template(tmpl, msgs.data(), msgs.size(), true, buf.data(), buf.size());
    }
    std::string prompt(buf.data(), len);

    prompt += "<think></think>";

    auto vocab = llama_model_get_vocab(model);

    qDebug() << "get vocab";

    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), nullptr, 0,
                                         /*add_special=*/true, /*parse_special=*/true);

    qDebug() << "tokenizing";

    std::vector<llama_token> tokens(n_prompt);

    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), tokens.data(), tokens.size(), true, true) < 0) {
      qDebug() << "failed to tokenize";
      QMetaObject::invokeMethod(this, [this]() { emit errorOccured("Failed to tokenize prompt"); });
      return;
    }

    qDebug() << "tokenized";

    const int n_predict = 64;

    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx = n_prompt + n_predict;
    cparams.n_batch = n_prompt;
    llama_context *ctx = llama_init_from_model(model, cparams);

    if (!ctx) { emit errorOccured("Failed to initialize llama context"); }

    qDebug() << "add sampler chain";

    llama_sampler *smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    // llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    qDebug() << "add batch";

    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());

    for (int n = 0; n < n_predict;) {
      if (llama_decode(ctx, batch)) { emit errorOccured("Failed to llama_decode"); }

      // sample the next token from the last logits
      llama_token tok = llama_sampler_sample(smpl, ctx, -1);

      if (llama_vocab_is_eog(vocab, tok)) break; // end of generation

      qDebug() << "GOT TOKEN";

      // detokenize and print
      std::array<char, 256> buf;
      int len = llama_token_to_piece(vocab, tok, buf.data(), buf.size(), 0, true);

      if (len < 0) {
        emit errorOccured("Failed to detokenize");
        return;
      }

      QMetaObject::invokeMethod(this, [this, token = std::string{buf.data(), static_cast<size_t>(len)}]() {
        emit dataAdded(token);
      });

      // feed the sampled token back in
      batch = llama_batch_get_one(&tok, 1);
      n++;
    }

    qDebug() << "end of generation";

    llama_sampler_free(smpl);
    llama_free(ctx);
    llama_model_free(model);

    QMetaObject::invokeMethod(this, [this]() { emit finished(); });
  }

  std::filesystem::path m_model;
  AI::ChatCompletionPayload m_payload;
  QFutureWatcher<void> m_completion;
};

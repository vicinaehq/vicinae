#pragma once
#include <expected>
#include <format>
#include <functional>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/schema.hpp>
#include <glaze/json/write.hpp>
#include <qfuture.h>
#include <string>
#include <optional>

namespace AI {
struct ToolOutput {
  std::string content;
  std::optional<std::string> displayText;
  std::optional<std::string> statusText;
  bool failed = false;
};

template <typename T> struct ToolTask {
  QFuture<std::expected<T, std::string>> future;
  std::function<void()> cancel;
};

class AbstractTool {

public:
  struct ToolSchema {
    std::string type{"function"};
    struct {
      std::string name;
      std::string description;
      glz::raw_json parameters;
    } function;
  };
  using RawToolResult = std::expected<ToolOutput, std::string>;
  using RawToolTask = ToolTask<ToolOutput>;

  virtual ~AbstractTool() = default;
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual RawToolTask runRaw(std::string_view object) = 0;
  virtual std::optional<std::string> invocationSummary(std::string_view arguments) const { return {}; }

  ToolSchema toolSchema() const {
    auto input = generateInputSchema();
    return ToolSchema{.function = {.name = name(), .description = description(), .parameters = input}};
  }

  std::string generateSchema() const {
    auto input = generateInputSchema();
    std::string buf;
    auto schema = ToolSchema{.function = {.name = name(), .description = description(), .parameters = input}};

    [[maybe_unused]] auto res = glz::write_json(schema, buf);

    return buf;
  }

  virtual std::string generateInputSchema() const = 0;
};

template <glz::has_reflect T, glz::has_reflect U> class AbstractTypedTool : public AbstractTool {
  std::string name() const override = 0;
  std::string description() const override = 0;

  RawToolTask runRaw(std::string_view object) final {
    T payload;
    if (auto const error = glz::read_json(payload, object)) {
      return {QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected(glz::format_error(error))), {}};
    }

    auto task = run(payload);
    auto future = task.future.then([](const std::expected<U, std::string> &res) -> RawToolResult {
      if (!res) return std::unexpected(res.error());

      std::string output;

      if (auto const error = glz::write_json(res.value(), output)) {
        return std::unexpected(glz::format_error(error));
      }

      return ToolOutput{.content = std::move(output)};
    });
    return {std::move(future), std::move(task.cancel)};
  }

  std::string generateInputSchema() const override {
    static thread_local std::string buf;
    if (buf.empty()) { [[maybe_unused]] auto res = glz::write_json_schema<T>(buf); }
    return buf;
  }

  virtual ToolTask<U> run(const T &payload) const = 0;
};

}; // namespace AI

#pragma once
#include <expected>
#include <format>
#include <glaze/core/common.hpp>
#include <glaze/core/meta.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/schema.hpp>
#include <glaze/json/write.hpp>
#include <qfuture.h>
#include <string>

namespace AI {
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
  using RawToolResult = std::expected<std::string, std::string>;
  using FutureRawToolResult = QFuture<RawToolResult>;

  virtual ~AbstractTool() = default;
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual FutureRawToolResult runRaw(std::string_view object) = 0;

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

  using Result = QFuture<std::expected<U, std::string>>;

  FutureRawToolResult runRaw(std::string_view object) final {
    T payload;
    if (auto const error = glz::read_json(payload, object)) {
      return QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected(glz::format_error(error)));
    }

    return run(payload).then([](const std::expected<U, std::string> &res) -> RawToolResult {
      if (!res) return std::unexpected(res.error());

      std::string output;

      if (auto const error = glz::write_json(res.value(), output)) {
        return std::unexpected(glz::format_error(error));
      }

      return RawToolResult{output};
    });
  }

  std::string generateInputSchema() const override {
    static thread_local std::string buf;
    if (buf.empty()) { [[maybe_unused]] auto res = glz::write_json_schema<T>(buf); }
    return buf;
  }

  virtual QFuture<std::expected<U, std::string>> run(const T &payload) const = 0;
};

struct FunFactReq {
  std::string subject;

  struct glaze_json_schema {
    glz::schema subject{.description = "What to tell a fun fact about"};
  };
};

struct FunFactResponse {
  std::string fact;
};

class GenerateFunFact : public AbstractTypedTool<FunFactReq, FunFactResponse> {
public:
  std::string name() const override { return "generate_fun_fact"; }
  std::string description() const override { return "Generate a fun fact about anything"; }
  QFuture<std::expected<FunFactResponse, std::string>> run(const FunFactReq &payload) const override {
    FunFactResponse res{std::format("I love {}, they are so fun!", payload.subject)};
    return QtFuture::makeReadyValueFuture<std::expected<FunFactResponse, std::string>>(res);
  }
};

}; // namespace AI

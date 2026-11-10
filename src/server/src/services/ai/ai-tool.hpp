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
#include <QByteArrayView>
#include <qfuture.h>
#include <string>
#include <optional>
#include "common/json.hpp"

namespace AI {
struct ToolOutput {
  std::string content;
  std::optional<std::string> displayText;
  std::optional<std::string> statusText;
  bool failed = false;
};

struct TruncatedText {
  std::string_view text;
  bool truncated = false;
};

/**
 * Cut text to at most `maxBytes` without splitting a UTF-8 sequence.
 */
constexpr TruncatedText truncateUtf8(std::string_view text, std::size_t maxBytes) {
  if (text.size() <= maxBytes) return {.text = text};
  auto end = maxBytes;
  while (end > 0 && (static_cast<unsigned char>(text[end]) & 0xC0) == 0x80)
    --end;
  return {.text = text.substr(0, end), .truncated = true};
}

/**
 * Whether the data is valid UTF-8 text a model can read, as opposed to binary content.
 */
inline bool isReadableText(std::string_view data) {
  return QByteArrayView(data).isValidUtf8() && !data.contains('\0');
}

template <typename T> struct ToolTask {
  QFuture<std::expected<T, std::string>> future;
  std::function<void()> cancel;
};

class AbstractTool {

public:
  struct ToolSchema {
    std::string type{"function"};
    struct Function {
      std::string name;
      std::string description;
      glz::raw_json parameters;
    };
    Function function;
  };
  using RawToolResult = std::expected<ToolOutput, std::string>;
  using RawToolTask = ToolTask<ToolOutput>;

  virtual ~AbstractTool() = default;
  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
  virtual RawToolTask runRaw(std::string_view object) = 0;
  virtual bool isEnabled() const { return true; }
  virtual bool requiresApproval(std::string_view arguments) const { return false; }
  virtual bool handlesApproval() const { return false; }
  virtual bool isDeferred() const { return false; }
  virtual std::optional<std::string> invocationSummary(std::string_view arguments) const { return {}; }

  ToolSchema toolSchema() const {
    auto input = generateInputSchema();
    return ToolSchema{.function = {.name = name(), .description = description(), .parameters = input}};
  }

  virtual std::string generateInputSchema() const = 0;
};

}; // namespace AI

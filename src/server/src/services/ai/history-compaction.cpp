#include "history-compaction.hpp"
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <string>
#include <vector>
#include "common/enumerate.hpp"

namespace AI {
namespace {

constexpr std::size_t SHORTENED_TOOL_RESULT_SIZE = 2 * 1024;
constexpr std::size_t CURRENT_TURN_TOOL_RESULTS_SIZE = 256 * 1024;
constexpr std::size_t MAX_HISTORY_SIZE = 512 * 1024;
// Images are billed by dimensions rather than encoded size, this is roughly what one costs as text.
constexpr std::size_t IMAGE_SIZE = 8 * 1024;
constexpr std::size_t TURNS_KEEPING_IMAGES = 2;
constexpr std::string_view SHORTENED_NOTICE =
    "\n[Output shortened, run the tool again if you need all of it]";
constexpr std::string_view IMAGE_REMOVED_NOTICE = "[An image was attached here]";

void shorten(ToolResultPart &result) {
  if (result.content.size() <= SHORTENED_TOOL_RESULT_SIZE) return;
  auto shortened = std::string(truncateUtf8(result.content, SHORTENED_TOOL_RESULT_SIZE).text);
  shortened += SHORTENED_NOTICE;
  result.content = std::move(shortened);
}

std::size_t partSize(const ChatPart &part) {
  if (const auto *text = std::get_if<TextPart>(&part)) return text->text.size();
  if (const auto *call = std::get_if<ToolCallPart>(&part)) return call->arguments.size();
  if (const auto *result = std::get_if<ToolResultPart>(&part)) return result->content.size();
  return IMAGE_SIZE;
}

std::size_t messageSize(const ChatMessage &message) {
  return std::ranges::fold_left(message.parts, std::size_t{0},
                                [](std::size_t sum, const ChatPart &part) { return sum + partSize(part); });
}

} // namespace

ChatHistory compactHistory(const ChatHistory &history) {
  std::vector<std::size_t> turns;
  turns.reserve(history.size());
  for (const auto &[index, message] : vicinae::enumerate(history)) {
    if (message.role == ChatRole::User) turns.emplace_back(static_cast<std::size_t>(index));
  }
  if (turns.empty()) return history;

  const auto currentTurn = turns.back();
  const auto firstTurnWithImages = turns[turns.size() - std::min(turns.size(), TURNS_KEEPING_IMAGES)];

  ChatHistory compacted = history;

  std::size_t currentToolOutput = 0;
  for (auto index = compacted.size(); index-- > 0;) {
    for (auto &part : compacted[index].parts | std::views::reverse) {
      if (index < firstTurnWithImages && std::holds_alternative<ImagePart>(part)) {
        part = TextPart{std::string(IMAGE_REMOVED_NOTICE)};
      } else if (auto *result = std::get_if<ToolResultPart>(&part)) {
        if (index >= currentTurn) currentToolOutput += result->content.size();
        // the newest output is what the model is working on, so it's the last to go
        if (index < currentTurn || currentToolOutput > CURRENT_TURN_TOOL_RESULTS_SIZE) shorten(*result);
      }
    }
  }

  auto total = std::ranges::fold_left(
      compacted, std::size_t{0}, [](std::size_t sum, const ChatMessage &m) { return sum + messageSize(m); });

  std::size_t dropped = 0;
  for (const auto turn : turns | std::views::drop(1)) {
    if (total <= MAX_HISTORY_SIZE) break;
    for (auto index = dropped; index < turn; ++index) {
      if (compacted[index].role != ChatRole::System && compacted[index].role != ChatRole::Developer)
        total -= messageSize(compacted[index]);
    }
    dropped = turn;
  }

  if (dropped > 0) {
    std::erase_if(compacted, [&, index = std::size_t{0}](const ChatMessage &message) mutable {
      const auto current = index++;
      return current < dropped && message.role != ChatRole::System && message.role != ChatRole::Developer;
    });
  }

  return compacted;
}

} // namespace AI

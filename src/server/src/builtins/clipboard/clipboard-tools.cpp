#include <format>
#include <QDateTime>
#include "clipboard-tools.hpp"
#include "services/clipboard/clipboard-service.hpp"

template <> struct glz::meta<ClipboardOfferKind> {
  using enum ClipboardOfferKind;
  static constexpr auto value =
      glz::enumerate("unknown", Unknown, "text", Text, "link", Link, "image", Image, "file", File);
};

namespace {

constexpr int DEFAULT_LIMIT = 10;
constexpr int MAX_LIMIT = 20;
constexpr std::size_t MAX_ENTRY_BYTES = 32 * 1024;

struct SearchInput {
  std::string query;
  std::optional<ClipboardOfferKind> kind;
  std::optional<int> limit;
};

struct SearchResult {
  std::string id;
  ClipboardOfferKind kind;
  std::string preview;
  std::string copied_at;
  bool pinned;
  std::optional<std::string> url_host;
};

struct ReadInput {
  std::string id;
};

AI::AbstractTool::RawToolTask ready(AI::AbstractTool::RawToolResult result) {
  return {QtFuture::makeReadyValueFuture(std::move(result)), {}};
}

} // namespace

std::string SearchClipboardTool::description() const {
  return "Search the user's clipboard history, most recent first. Returns entry ids with a short preview. "
         "Use "
         "read_entry to get the full content of a text entry. An empty query lists the latest entries.";
}

std::string SearchClipboardTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"query":{"type":"string","description":"Text to search for. Empty to list recent entries."},"kind":{"type":"string","enum":["text","link","image","file"],"description":"Only return entries of this kind."},"limit":{"type":"integer","minimum":1,"maximum":20,"description":"Maximum number of entries. Defaults to 10."}},"required":["query"],"additionalProperties":false})";
}

std::optional<std::string> SearchClipboardTool::invocationSummary(std::string_view arguments) const {
  SearchInput input;
  if (glz::read_json(input, arguments) || input.query.empty()) return std::nullopt;
  return input.query;
}

AI::AbstractTool::RawToolTask SearchClipboardTool::runRaw(std::string_view arguments) {
  SearchInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  const auto limit = std::clamp(input.limit.value_or(DEFAULT_LIMIT), 1, MAX_LIMIT);
  auto future =
      m_clipboard.listAll(limit, 0, {.query = QString::fromStdString(input.query), .kind = input.kind})
          .then([](const PaginatedResponse<ClipboardHistoryEntry> &response) -> RawToolResult {
            std::vector<SearchResult> results;
            results.reserve(response.data.size());

            for (const auto &entry : response.data) {
              results.emplace_back(SearchResult{
                  .id = entry.id.toStdString(),
                  .kind = entry.kind,
                  .preview = entry.textPreview.toStdString(),
                  .copied_at = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(entry.updatedAt))
                                   .toString(Qt::ISODate)
                                   .toStdString(),
                  .pinned = entry.pinnedAt != 0,
                  .url_host = entry.urlHost.transform([](const QString &host) { return host.toStdString(); }),
              });
            }

            std::string out;
            if (const auto error = glz::write_json(results, out))
              return std::unexpected(glz::format_error(error));
            return AI::ToolOutput{.content = std::move(out)};
          });

  return {std::move(future), {}};
}

std::string ReadClipboardEntryTool::description() const {
  return "Read the full content of a text, link or file entry from the clipboard history, using an id "
         "returned "
         "by search_history. Content is limited to 32 KiB. Images cannot be read.";
}

std::string ReadClipboardEntryTool::generateInputSchema() const {
  return R"({"type":"object","properties":{"id":{"type":"string","description":"Clipboard entry id."}},"required":["id"],"additionalProperties":false})";
}

AI::AbstractTool::RawToolTask ReadClipboardEntryTool::runRaw(std::string_view arguments) {
  ReadInput input;
  if (const auto error = glz::read_json(input, arguments))
    return ready(std::unexpected(glz::format_error(error, arguments)));

  const auto data = m_clipboard.getMainOfferData(QString::fromStdString(input.id));
  if (!data) return ready(std::unexpected(std::format("Could not read clipboard entry {}.", input.id)));

  const std::string_view content(data->constData(), data->size());
  if (!AI::isReadableText(content))
    return ready(std::unexpected(std::string("This entry is not text and cannot be read.")));

  const auto [text, truncated] = AI::truncateUtf8(content, MAX_ENTRY_BYTES);
  auto out = std::string(text);
  if (truncated) out += std::format("\n\n[Truncated: the entry is {} bytes long.]", content.size());
  return ready(AI::ToolOutput{.content = std::move(out)});
}

#include "web-search-tool.hpp"

#include <format>
#include <QCoreApplication>
#include <glaze/json/read.hpp>
#include "services/ai/web-search-service.hpp"

namespace AI {

struct Input {
  std::string query;
};

bool WebSearchTool::isEnabled() const {
  return WebSearchService::enabled() && WebSearchService::configured();
}

std::optional<std::string> WebSearchTool::invocationSummary(std::string_view arguments) const {
  Input input;
  if (glz::read_json(input, arguments)) return std::nullopt;
  return input.query;
}

AbstractTool::RawToolTask WebSearchTool::runRaw(std::string_view arguments) {
  Input input;
  if (const auto error = glz::read_json(input, arguments))
    return {
        QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected(glz::format_error(error, arguments))),
        {}};
  if (input.query.empty()) {
    return {QtFuture::makeReadyValueFuture<RawToolResult>(std::unexpected("Search query cannot be empty")),
            {}};
  }

  auto future = WebSearchService::search(std::move(input.query));
  return {future.then([](WebSearchService::SearchResult result) -> RawToolResult {
            if (!result) return std::unexpected(result.error());

            std::string content;
            for (const auto &item : *result) {
              content += std::format("Title: {}\nURL: {}\n{}\n\n", item.title, item.url, item.content);
            }
            if (content.empty()) content = "No web results found.";
            return ToolOutput{.content = std::move(content),
                              .displayText =
                                  QCoreApplication::translate("WebSearchTool", "%n result(s)", nullptr,
                                                              static_cast<int>(result->size()))
                                      .toStdString()};
          }),
          {}};
}

} // namespace AI

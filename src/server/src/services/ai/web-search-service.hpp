#pragma once

#include <array>
#include <expected>
#include <optional>
#include <qfuture.h>
#include <QtGlobal>
#include <string>
#include <string_view>
#include <vector>

namespace AI {

struct WebSearchResult {
  std::string title;
  std::string url;
  std::string content;
};

enum class WebSearchProvider { TinyFish, Serper, Brave, Tavily, Exa, SerpApi };

struct WebSearchProviderInfo {
  WebSearchProvider provider;
  std::string_view id;
  std::string_view name;
  // Untranslated: pass through QCoreApplication::translate with TR_CONTEXT.
  const char *description;
  std::string_view keyUrl;
};

enum class WebSearchDepth { Quick, Standard, Deep };

struct WebSearchDepthInfo {
  WebSearchDepth depth;
  std::string_view id;
  // Untranslated: pass through QCoreApplication::translate with TR_CONTEXT.
  const char *label;
};

class WebSearchService {
public:
  using SearchResult = std::expected<std::vector<WebSearchResult>, std::string>;

  static constexpr const char *TR_CONTEXT = "WebSearchService";

  static constexpr auto PROVIDERS = std::to_array<WebSearchProviderInfo>({
      {WebSearchProvider::TinyFish, "tinyfish", "TinyFish",
       QT_TRANSLATE_NOOP("WebSearchService", "Free, no card"), "https://agent.tinyfish.ai/api-keys"},
      {WebSearchProvider::Serper, "serper", "Serper", QT_TRANSLATE_NOOP("WebSearchService", "Google results"),
       "https://serper.dev"},
      {WebSearchProvider::Brave, "brave", "Brave Search",
       QT_TRANSLATE_NOOP("WebSearchService", "Independent index"), "https://api-dashboard.search.brave.com"},
      {WebSearchProvider::Tavily, "tavily", "Tavily", QT_TRANSLATE_NOOP("WebSearchService", "Built for AI"),
       "https://app.tavily.com"},
      {WebSearchProvider::Exa, "exa", "Exa", QT_TRANSLATE_NOOP("WebSearchService", "Neural search"),
       "https://dashboard.exa.ai"},
      {WebSearchProvider::SerpApi, "serpapi", "SerpApi",
       QT_TRANSLATE_NOOP("WebSearchService", "Google results"), "https://serpapi.com/dashboard"},
  });

  static constexpr auto DEPTHS = std::to_array<WebSearchDepthInfo>({
      {WebSearchDepth::Quick, "quick", QT_TRANSLATE_NOOP("WebSearchService", "Quick")},
      {WebSearchDepth::Standard, "standard", QT_TRANSLATE_NOOP("WebSearchService", "Standard")},
      {WebSearchDepth::Deep, "deep", QT_TRANSLATE_NOOP("WebSearchService", "Deep")},
  });

  // Kept outside the "ai:<providerId>" scopes so removing a provider can never clear it.
  static constexpr std::string_view API_KEY_SCOPE = "ai-web-search";

  static const WebSearchProviderInfo *findProvider(std::string_view id);
  static std::optional<WebSearchDepth> parseDepth(std::string_view id);
  static std::string_view depthId(WebSearchDepth depth);

  static bool enabled();
  // Whether the selected provider has an API key.
  static bool configured();
  static const WebSearchProviderInfo &provider();
  static WebSearchDepth depth();
  static void setApiKey(std::string_view key);

  static QFuture<SearchResult> search(std::string query);
  // Same as search(), but runs even while web search is disabled, so a key can be checked first.
  static QFuture<SearchResult> test();

private:
  static QFuture<SearchResult> run(std::string query);
};

} // namespace AI

#include "web-search-service.hpp"

#include <algorithm>
#include <chrono>
#include <format>
#include <optional>
#include <ranges>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <QMetaEnum>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPromise>
#include <QRegularExpression>
#include <QUrl>
#include <QUrlQuery>
#include <qjsonvalue.h>
#include <qlogging.h>
#include "internal/http-client.hpp"
#include "service-registry.hpp"
#include "services/ai/ai-preferences.hpp"
#include "services/local-storage/local-storage-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

namespace AI {

// Named, not anonymous: glaze cannot reflect types with internal linkage.
namespace web_search_detail {

using SearchResult = WebSearchService::SearchResult;
using Reply = std::expected<QByteArray, std::string>;

constexpr auto REQUEST_TIMEOUT = std::chrono::seconds(30);
constexpr qsizetype MAX_CONTENT_LENGTH = 1500;
constexpr qsizetype MAX_ERROR_BODY_LENGTH = 300;
// The key name used before keys were stored per provider; only Tavily existed then.
constexpr std::string_view LEGACY_KEY_NAME = "apiKey";

QString qs(std::string_view view) {
  return QString::fromUtf8(view.data(), static_cast<qsizetype>(view.size()));
}

WebSearchSettings settings() {
  auto *services = ServiceRegistry::instance();
  if (!services || !services->rootItemManager()) return {};
  return readPreferences<AiPreferences>(
             services->rootItemManager()->getProviderPreferenceValues(qs(EXTENSION_ID)))
      .webSearch;
}

LocalStorageService *storage() {
  auto *services = ServiceRegistry::instance();
  return services ? services->localStorage() : nullptr;
}

std::string apiKey(const WebSearchProviderInfo &provider) {
  auto *store = storage();
  if (!store) return {};
  auto key = store->getItem(qs(WebSearchService::API_KEY_SCOPE), qs(provider.id)).toString();
  if (key.isEmpty() && provider.provider == WebSearchProvider::Tavily)
    key = store->getItem(qs(WebSearchService::API_KEY_SCOPE), qs(LEGACY_KEY_NAME)).toString();
  return key.toStdString();
}

QFuture<SearchResult> fail(std::string message) {
  return QtFuture::makeReadyValueFuture<SearchResult>(std::unexpected(std::move(message)));
}

// Not http::Client: it logs request URLs and puts them in errors, and SerpApi takes its key in the URL.
QFuture<Reply> send(QNetworkRequest request, std::optional<QByteArray> body = std::nullopt) {
  request.setTransferTimeout(REQUEST_TIMEOUT);
  request.setRawHeader("Accept", "application/json");
  if (body) request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

  auto *manager = http::networkManager();
  auto *reply = body ? manager->post(request, *body) : manager->get(request);

  QPromise<Reply> promise;
  auto future = promise.future();
  QObject::connect(reply, &QNetworkReply::finished, [promise = std::move(promise), reply]() mutable {
    const auto data = reply->readAll();
    const auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() == QNetworkReply::NoError) {
      promise.addResult(data);
    } else if (status != 0) {
      const auto detail = QString::fromUtf8(data).simplified().left(MAX_ERROR_BODY_LENGTH).toStdString();
      promise.addResult(std::unexpected(std::format("HTTP {}: {}", status, detail)));
    } else {
      const auto *name = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(reply->error());
      promise.addResult(std::unexpected(std::format("network error ({})", name ? name : "unknown")));
    }
    promise.finish();
    reply->deleteLater();
  });
  return future;
}

template <typename T> std::expected<T, std::string> parse(const QByteArray &data) {
  T value;
  const std::string json = data.toStdString();
  if (const auto error = glz::read<glz::opts{.error_on_unknown_keys = false}>(value, json))
    return std::unexpected(std::format("unexpected response: {}", glz::format_error(error, json)));
  return value;
}

template <typename T> std::optional<QByteArray> toJson(const T &value) {
  std::string json;
  if (glz::write_json(value, json)) return std::nullopt;
  return QByteArray::fromStdString(json);
}

std::string clip(std::string_view text) {
  return QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size()))
      .left(MAX_CONTENT_LENGTH)
      .toStdString();
}

// Brave marks matched words with <strong>.
std::string stripTags(std::string_view text) {
  static const QRegularExpression TAG(QStringLiteral("<[^>]*>"));
  return QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size())).remove(TAG).toStdString();
}

int resultCount(WebSearchDepth depth) {
  switch (depth) {
  case WebSearchDepth::Quick:
    return 3;
  case WebSearchDepth::Standard:
    return 5;
  case WebSearchDepth::Deep:
    return 10;
  }
  return 5;
}

QNetworkRequest makeRequest(const QString &url,
                            std::initializer_list<std::pair<QByteArray, QByteArray>> headers,
                            const QUrlQuery &query = {}) {
  QUrl target(url);
  if (!query.isEmpty()) target.setQuery(query);
  QNetworkRequest request(target);
  for (const auto &[name, value] : headers)
    request.setRawHeader(name, value);
  return request;
}

template <typename Response, typename Convert>
QFuture<SearchResult> fetch(std::string_view provider, QFuture<Reply> reply, std::size_t limit,
                            Convert convert) {
  return reply.then([provider = std::string(provider), limit, convert](Reply result) -> SearchResult {
    if (!result) return std::unexpected(std::format("{} search failed: {}", provider, result.error()));
    auto response = parse<Response>(*result);
    if (!response) return std::unexpected(std::format("{} search failed: {}", provider, response.error()));

    auto items = convert(std::move(*response));
    if (items.size() > limit) items.resize(limit);
    for (auto &item : items)
      item.content = clip(item.content);
    return items;
  });
}

// Tavily: https://docs.tavily.com
struct TavilyRequest {
  std::string api_key;
  std::string query;
  std::string search_depth;
  int max_results = 5;
  bool include_answer = false;
};

struct TavilyResponse {
  struct Result {
    std::string title;
    std::string url;
    std::string content;
  };
  std::vector<Result> results;
};

QFuture<SearchResult> searchTavily(std::string query, std::string key, WebSearchDepth depth) {
  const auto searchDepth = depth == WebSearchDepth::Quick  ? "fast"
                           : depth == WebSearchDepth::Deep ? "advanced"
                                                           : "basic";
  const auto count = resultCount(depth);
  auto body = toJson(TavilyRequest{.api_key = std::move(key),
                                   .query = std::move(query),
                                   .search_depth = searchDepth,
                                   .max_results = count});
  if (!body) return fail("Tavily search failed: could not build the request");

  return fetch<TavilyResponse>(
      "Tavily", send(makeRequest(QStringLiteral("https://api.tavily.com/search"), {}), std::move(body)),
      count, [](TavilyResponse response) {
        return response.results | std::views::transform([](auto &item) {
                 return WebSearchResult{std::move(item.title), std::move(item.url), std::move(item.content)};
               }) |
               std::ranges::to<std::vector>();
      });
}

// Exa: https://exa.ai/docs/reference/search
struct ExaRequest {
  struct Contents {
    struct Text {
      int maxCharacters = static_cast<int>(MAX_CONTENT_LENGTH);
    };
    Text text;
  };
  std::string query;
  std::string type;
  int numResults = 5;
  Contents contents;
};

struct ExaResponse {
  struct Result {
    std::optional<std::string> title;
    std::string url;
    std::optional<std::string> text;
  };
  std::vector<Result> results;
};

QFuture<SearchResult> searchExa(std::string query, const std::string &key, WebSearchDepth depth) {
  const auto count = resultCount(depth);
  auto body = toJson(ExaRequest{.query = std::move(query),
                                .type = depth == WebSearchDepth::Deep ? "deep-lite" : "auto",
                                .numResults = count});
  if (!body) return fail("Exa search failed: could not build the request");

  return fetch<ExaResponse>("Exa",
                            send(makeRequest(QStringLiteral("https://api.exa.ai/search"),
                                             {{"x-api-key", QByteArray::fromStdString(key)}}),
                                 std::move(body)),
                            count, [](ExaResponse response) {
                              return response.results | std::views::transform([](auto &item) {
                                       return WebSearchResult{item.title.value_or(item.url),
                                                              std::move(item.url), item.text.value_or("")};
                                     }) |
                                     std::ranges::to<std::vector>();
                            });
}

// Serper: https://serper.dev
struct SerperRequest {
  std::string q;
  int num = 5;
};

struct SerperResponse {
  struct Result {
    std::string title;
    std::string link;
    std::optional<std::string> snippet;
  };
  std::vector<Result> organic;
};

QFuture<SearchResult> searchSerper(std::string query, const std::string &key, WebSearchDepth depth) {
  const auto count = resultCount(depth);
  auto body = toJson(SerperRequest{.q = std::move(query), .num = count});
  if (!body) return fail("Serper search failed: could not build the request");

  return fetch<SerperResponse>("Serper",
                               send(makeRequest(QStringLiteral("https://google.serper.dev/search"),
                                                {{"X-API-KEY", QByteArray::fromStdString(key)}}),
                                    std::move(body)),
                               count, [](SerperResponse response) {
                                 return response.organic | std::views::transform([](auto &item) {
                                          return WebSearchResult{std::move(item.title), std::move(item.link),
                                                                 item.snippet.value_or("")};
                                        }) |
                                        std::ranges::to<std::vector>();
                               });
}

// Brave Search: https://api-dashboard.search.brave.com/app/documentation/web-search
struct BraveResponse {
  struct Web {
    struct Result {
      std::string title;
      std::string url;
      std::optional<std::string> description;
      std::optional<std::vector<std::string>> extra_snippets;
    };
    std::vector<Result> results;
  };
  std::optional<Web> web;
};

QFuture<SearchResult> searchBrave(const std::string &query, const std::string &key, WebSearchDepth depth) {
  const auto count = resultCount(depth);
  QUrlQuery params;
  params.addQueryItem(QStringLiteral("q"), QString::fromStdString(query));
  params.addQueryItem(QStringLiteral("count"), QString::number(count));
  params.addQueryItem(QStringLiteral("extra_snippets"), QStringLiteral("true"));

  return fetch<BraveResponse>(
      "Brave Search",
      send(makeRequest(QStringLiteral("https://api.search.brave.com/res/v1/web/search"),
                       {{"X-Subscription-Token", QByteArray::fromStdString(key)}}, params)),
      count, [](BraveResponse response) {
        std::vector<WebSearchResult> items;
        if (!response.web) return items;
        items.reserve(response.web->results.size());
        for (auto &item : response.web->results) {
          auto content = stripTags(item.description.value_or(""));
          for (const auto &snippet : item.extra_snippets.value_or(std::vector<std::string>{}))
            content += "\n" + stripTags(snippet);
          items.emplace_back(WebSearchResult{stripTags(item.title), std::move(item.url), std::move(content)});
        }
        return items;
      });
}

// TinyFish: https://docs.tinyfish.ai/search-api/reference
struct TinyFishResponse {
  struct Result {
    std::string title;
    std::string url;
    std::optional<std::string> snippet;
  };
  std::vector<Result> results;
};

QFuture<SearchResult> searchTinyFish(const std::string &query, const std::string &key, WebSearchDepth depth) {
  QUrlQuery params;
  params.addQueryItem(QStringLiteral("query"), QString::fromStdString(query));

  return fetch<TinyFishResponse>("TinyFish",
                                 send(makeRequest(QStringLiteral("https://api.search.tinyfish.ai"),
                                                  {{"X-API-Key", QByteArray::fromStdString(key)}}, params)),
                                 resultCount(depth), [](TinyFishResponse response) {
                                   return response.results | std::views::transform([](auto &item) {
                                            return WebSearchResult{std::move(item.title), std::move(item.url),
                                                                   item.snippet.value_or("")};
                                          }) |
                                          std::ranges::to<std::vector>();
                                 });
}

// SerpApi: https://serpapi.com/search-api
struct SerpApiResponse {
  struct Result {
    std::string title;
    std::string link;
    std::optional<std::string> snippet;
  };
  std::vector<Result> organic_results;
};

QFuture<SearchResult> searchSerpApi(const std::string &query, const std::string &key, WebSearchDepth depth) {
  QUrlQuery params;
  params.addQueryItem(QStringLiteral("engine"), QStringLiteral("google"));
  params.addQueryItem(QStringLiteral("q"), QString::fromStdString(query));
  params.addQueryItem(QStringLiteral("api_key"), QString::fromStdString(key));

  return fetch<SerpApiResponse>("SerpApi",
                                send(makeRequest(QStringLiteral("https://serpapi.com/search"), {}, params)),
                                resultCount(depth), [](SerpApiResponse response) {
                                  return response.organic_results | std::views::transform([](auto &item) {
                                           return WebSearchResult{std::move(item.title), std::move(item.link),
                                                                  item.snippet.value_or("")};
                                         }) |
                                         std::ranges::to<std::vector>();
                                });
}

} // namespace web_search_detail

using namespace web_search_detail;

const WebSearchProviderInfo *WebSearchService::findProvider(std::string_view id) {
  const auto it = std::ranges::find(PROVIDERS, id, &WebSearchProviderInfo::id);
  return it != PROVIDERS.end() ? &*it : nullptr;
}

std::optional<WebSearchDepth> WebSearchService::parseDepth(std::string_view id) {
  const auto it = std::ranges::find(DEPTHS, id, &WebSearchDepthInfo::id);
  return it != DEPTHS.end() ? std::optional(it->depth) : std::nullopt;
}

std::string_view WebSearchService::depthId(WebSearchDepth depth) {
  return std::ranges::find(DEPTHS, depth, &WebSearchDepthInfo::depth)->id;
}

bool WebSearchService::enabled() { return settings().enabled; }

bool WebSearchService::configured() { return !apiKey(provider()).empty(); }

const WebSearchProviderInfo &WebSearchService::provider() {
  if (const auto *info = findProvider(settings().provider)) return *info;
  if (const auto *info = findProvider(WebSearchSettings{}.provider)) return *info;
  return PROVIDERS.front();
}

WebSearchDepth WebSearchService::depth() {
  const auto &levels = provider().depths;
  if (levels.empty()) return WebSearchDepth::Standard;

  const auto chosen = parseDepth(settings().depth);
  if (chosen && std::ranges::contains(levels, *chosen)) return *chosen;
  if (std::ranges::contains(levels, WebSearchDepth::Standard)) return WebSearchDepth::Standard;
  return levels.front();
}

void WebSearchService::setApiKey(std::string_view key) {
  if (auto *store = storage()) store->setItem(qs(API_KEY_SCOPE), qs(provider().id), qs(key));
}

QFuture<SearchResult> WebSearchService::search(std::string query) {
  if (!enabled()) return fail("Web search is disabled");
  return run(std::move(query));
}

QFuture<SearchResult> WebSearchService::test() { return run("Vicinae launcher"); }

QFuture<SearchResult> WebSearchService::run(std::string query) {
  const auto &info = provider();
  auto key = apiKey(info);
  if (key.empty()) return fail(std::format("No API key is saved for {}", info.name));

  const auto level = depth();
  qInfo().noquote() << "Web search:" << qs(info.name) << "depth" << qs(depthId(level));

  switch (info.provider) {
  case WebSearchProvider::Tavily:
    return searchTavily(std::move(query), std::move(key), level);
  case WebSearchProvider::Exa:
    return searchExa(std::move(query), key, level);
  case WebSearchProvider::Serper:
    return searchSerper(std::move(query), key, level);
  case WebSearchProvider::Brave:
    return searchBrave(query, key, level);
  case WebSearchProvider::TinyFish:
    return searchTinyFish(query, key, level);
  case WebSearchProvider::SerpApi:
    return searchSerpApi(query, key, level);
  }
  return fail(std::format("{} is not supported", info.name));
}

} // namespace AI

#include <CoreServices/CoreServices.h>
#include <QtConcurrent/QtConcurrentRun>
#include <algorithm>
#include <climits>
#include <filesystem>
#include <initializer_list>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#include <common/file-category.hpp>
#include "fuzzy/fzf.hpp"
#include "spotlight-file-indexer.hpp"

namespace {

constexpr int MIN_CANDIDATE_LIMIT = 250;
constexpr int CANDIDATE_LIMIT_MULTIPLIER = 20;

int candidateLimitFor(int limit) {
  if (limit <= 0) { return 0; }
  return std::max(MIN_CANDIDATE_LIMIT, limit * CANDIDATE_LIMIT_MULTIPLIER);
}

vicinae::FileCategory categoryForPath(const std::filesystem::path &path) {
  if (vicinae::normalizedExtension(path) == "app") { return vicinae::FileCategory::Application; }

  std::error_code ec;
  return vicinae::fileCategoryFor(path, std::filesystem::is_directory(path, ec));
}

std::string anyOf(std::initializer_list<std::string_view> predicates) {
  auto predicate = predicates | std::views::join_with(std::string_view{" || "}) | std::ranges::to<std::string>();

  return "(" + predicate + ")";
}

std::optional<std::string> categoryPredicate(vicinae::FileCategory category) {
  switch (category) {
  case vicinae::FileCategory::Directory:
    return R"(kMDItemContentTypeTree == "public.folder")";
  case vicinae::FileCategory::Image:
    return R"(kMDItemContentTypeTree == "public.image")";
  case vicinae::FileCategory::Video:
    return R"(kMDItemContentTypeTree == "public.movie")";
  case vicinae::FileCategory::Audio:
    return R"(kMDItemContentTypeTree == "public.audio")";
  case vicinae::FileCategory::Document:
    return anyOf({R"(kMDItemContentTypeTree == "public.text")", R"(kMDItemContentTypeTree == "public.rtf")",
                  R"(kMDItemContentTypeTree == "com.adobe.pdf")",
                  R"(kMDItemContentTypeTree == "public.presentation")",
                  R"(kMDItemContentTypeTree == "public.spreadsheet")",
                  R"(kMDItemContentTypeTree == "public.comma-separated-values-text")",
                  R"(kMDItemContentTypeTree == "com.microsoft.word.doc")",
                  R"(kMDItemContentTypeTree == "com.microsoft.excel.xls")",
                  R"(kMDItemContentTypeTree == "com.microsoft.powerpoint.ppt")",
                  R"(kMDItemContentTypeTree == "org.openxmlformats.wordprocessingml.document")",
                  R"(kMDItemContentTypeTree == "org.openxmlformats.spreadsheetml.sheet")",
                  R"(kMDItemContentTypeTree == "org.openxmlformats.presentationml.presentation")",
                  R"(kMDItemContentTypeTree == "org.oasis-open.opendocument.text")",
                  R"(kMDItemContentTypeTree == "org.oasis-open.opendocument.spreadsheet")",
                  R"(kMDItemContentTypeTree == "org.oasis-open.opendocument.presentation")",
                  R"(kMDItemContentTypeTree == "com.apple.iwork.pages.pages")",
                  R"(kMDItemContentTypeTree == "com.apple.iwork.numbers.numbers")",
                  R"(kMDItemContentTypeTree == "com.apple.iwork.keynote.key")",
                  R"(kMDItemContentTypeTree == "org.idpf.epub-container")"});
  case vicinae::FileCategory::Archive:
    return R"(kMDItemContentTypeTree == "public.archive")";
  case vicinae::FileCategory::Application:
    return anyOf({R"(kMDItemContentTypeTree == "com.apple.application")",
                  R"(kMDItemContentTypeTree == "com.apple.application-bundle")"});
  case vicinae::FileCategory::Other:
    return std::nullopt;
  }

  return std::nullopt;
}

/**
 * Builds a Spotlight predicate matching each whitespace-separated term as a
 * case/diacritic-insensitive substring of the file name. This purposely mirrors
 * the Linux FTS5 prefix matching: it is only a candidate generator, the real
 * ranking is done afterwards by the fuzzy matcher.
 */
std::string buildPredicate(std::string_view query) {
  std::string predicate;

  for (size_t i = 0; i < query.size();) {
    while (i < query.size() && query[i] == ' ') {
      ++i;
    }

    size_t const start = i;

    while (i < query.size() && query[i] != ' ') {
      ++i;
    }

    if (i == start) { break; }

    std::string_view const word = query.substr(start, i - start);
    std::string escaped;

    escaped.reserve(word.size());

    for (char c : word) {
      if (c == '"' || c == '\\' || c == '*') { escaped.push_back('\\'); }
      escaped.push_back(c);
    }

    if (!predicate.empty()) { predicate += " || "; }
    predicate += "kMDItemFSName == \"*" + escaped + "*\"cd";
  }

  return predicate;
}

std::string buildPredicate(std::string_view query, const IndexerQueryParams &params) {
  std::string predicate = buildPredicate(query);

  if (predicate.empty() || !params.category) { return predicate; }

  auto categoryFilter = categoryPredicate(*params.category);
  if (!categoryFilter) { return predicate; }

  return "(" + predicate + ") && (" + *categoryFilter + ")";
}

std::vector<IndexerFileResult> runQuery(const std::string &query, const IndexerQueryParams &params) {
  std::vector<IndexerFileResult> results;
  int const candidateLimit = candidateLimitFor(params.limit);

  if (candidateLimit == 0) { return results; }

  std::string const predicate = buildPredicate(query, params);

  if (predicate.empty()) { return results; }

  CFStringRef queryString =
      CFStringCreateWithCString(kCFAllocatorDefault, predicate.c_str(), kCFStringEncodingUTF8);

  if (!queryString) { return results; }

  MDQueryRef mdQuery = MDQueryCreate(kCFAllocatorDefault, queryString, nullptr, nullptr);
  CFRelease(queryString);

  if (!mdQuery) { return results; }

  MDQuerySetMaxCount(mdQuery, candidateLimit);

  if (!MDQueryExecute(mdQuery, kMDQuerySynchronous)) {
    CFRelease(mdQuery);
    return results;
  }

  struct Scored {
    std::filesystem::path path;
    int score = 0;
    vicinae::FileCategory category = vicinae::FileCategory::Other;
  };

  CFIndex const count = MDQueryGetResultCount(mdQuery);
  const auto &matcher = fzf::threadLocalMatcher();
  std::vector<Scored> scored;

  scored.reserve(count);

  for (CFIndex i = 0; i < count; ++i) {
    auto item = (MDItemRef)MDQueryGetResultAtIndex(mdQuery, i);

    if (!item) { continue; }

    auto pathRef = (CFStringRef)MDItemCopyAttribute(item, kMDItemPath);

    if (!pathRef) { continue; }

    char buf[PATH_MAX];

    if (CFStringGetCString(pathRef, buf, sizeof(buf), kCFStringEncodingUTF8)) {
      std::filesystem::path path(buf);
      auto category = categoryForPath(path);

      if (params.category && *params.category != category) {
        CFRelease(pathRef);
        continue;
      }

      int const fuzzyScore = matcher.fuzzy_match_v2_score_query(path.filename().string(), query);

      if (fuzzyScore > 0) {
        scored.emplace_back(Scored{.path = std::move(path), .score = fuzzyScore, .category = category});
      }
    }

    CFRelease(pathRef);
  }

  CFRelease(mdQuery);

  std::ranges::stable_sort(scored, [](const Scored &a, const Scored &b) {
    if (a.score != b.score) { return a.score > b.score; }
    return a.path < b.path;
  });

  int const limit = std::max(0, params.limit);
  size_t const end = std::min(static_cast<size_t>(limit), scored.size());

  results.reserve(end);

  for (size_t i = 0; i < end; ++i) {
    results.emplace_back(IndexerFileResult{.path = std::move(scored[i].path),
                                           .rank = static_cast<double>(scored[i].score),
                                           .category = scored[i].category});
  }

  return results;
}

} // namespace

QFuture<std::vector<IndexerFileResult>> SpotlightFileIndexer::queryAsync(std::string_view query,
                                                                         const IndexerQueryParams &params) {
  return QtConcurrent::run([params, q = std::string(query)]() {
    @autoreleasepool {
      return runQuery(q, params);
    }
  });
}

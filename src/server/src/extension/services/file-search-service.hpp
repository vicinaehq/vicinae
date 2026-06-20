#pragma once
#include <ranges>
#include <utility>
#include "generated/tsapi.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-service.hpp"

class ExtFileSearchService : public tsapi::AbstractFileSearch {
public:
  ExtFileSearchService(tsapi::RpcTransport &transport, FileService &indexer)
      : AbstractFileSearch(transport), m_indexer(indexer) {}

  tsapi::Result<std::vector<tsapi::FileInfo>>::Future search(std::string q,
                                                             tsapi::FileSearchOptions opts) override {
    IndexerQueryParams params{
        .limit = static_cast<int>(opts.limit),
        .category =
            opts.filters.category.transform([](tsapi::FileSearchCategory c) { return mapCategory(c); }),
    };

    return m_indexer.queryAsync(std::move(q), std::move(params))
        .then(
            [](std::vector<IndexerFileResult> results) -> tsapi::Result<std::vector<tsapi::FileInfo>>::Type {
              const auto toFileInfo = [](auto &&result) {
                return tsapi::FileInfo{.path = result.path, .category = mapCategory(result.category)};
              };
              return results | std::views::transform(toFileInfo) | std::ranges::to<std::vector>();
            });
  }

private:
  static vicinae::FileCategory mapCategory(tsapi::FileSearchCategory category) {
    using In = tsapi::FileSearchCategory;
    using Out = vicinae::FileCategory;

    switch (category) {
    case In::Other:
      return Out::Other;
    case In::Application:
      return Out::Application;
    case In::Archive:
      return Out::Archive;
    case In::Audio:
      return Out::Audio;
    case In::Directory:
      return Out::Directory;
    case In::Document:
      return Out::Document;
    case In::Image:
      return Out::Image;
    case In::Video:
      return Out::Video;
    }

    std::unreachable();
  }

  static tsapi::FileSearchCategory mapCategory(vicinae::FileCategory category) {
    using In = vicinae::FileCategory;
    using Out = tsapi::FileSearchCategory;

    switch (category) {
    case In::Other:
      return Out::Other;
    case In::Application:
      return Out::Application;
    case In::Archive:
      return Out::Archive;
    case In::Audio:
      return Out::Audio;
    case In::Directory:
      return Out::Directory;
    case In::Document:
      return Out::Document;
    case In::Image:
      return Out::Image;
    case In::Video:
      return Out::Video;
    }

    std::unreachable();
  }

  FileService &m_indexer;
};

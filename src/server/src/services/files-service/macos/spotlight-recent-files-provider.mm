#include "spotlight-recent-files-provider.hpp"
#include <CoreServices/CoreServices.h>
#include <QtConcurrent/QtConcurrentRun>
#include <climits>
#include <string>
#include <common/file-category.hpp>

namespace fs = std::filesystem;

namespace {

constexpr auto LAST_USED = "kMDItemLastUsedDate > $time.today(-365)";
constexpr auto FOLDER = "kMDItemContentTypeTree == \"public.folder\"";
constexpr auto APP_BUNDLE = "kMDItemContentTypeTree == \"com.apple.application-bundle\"";

/**
 * Finder, file panels and system daemons all refresh a folder's last-used date, so folders and
 * bundles only make sense when they are explicitly asked for.
 */
std::string predicateFor(const RecentFilesParams &params) {
  using vicinae::FileCategory;

  if (params.category == FileCategory::Directory) return std::string(LAST_USED) + " && " + FOLDER;
  if (params.category == FileCategory::Application) return std::string(LAST_USED) + " && " + APP_BUNDLE;
  return std::string(LAST_USED) + " && !(" + FOLDER + ") && !(" + APP_BUNDLE + ")";
}

CFArrayRef scopeFor(const RecentFilesParams &params) {
  if (params.category == vicinae::FileCategory::Application) {
    CFStringRef scopes[] = {kMDQueryScopeHome, CFSTR("/Applications")};
    return CFArrayCreate(kCFAllocatorDefault, (const void **)scopes, 2, &kCFTypeArrayCallBacks);
  }

  CFStringRef scopes[] = {kMDQueryScopeHome};
  return CFArrayCreate(kCFAllocatorDefault, (const void **)scopes, 1, &kCFTypeArrayCallBacks);
}

bool matchesCategory(const fs::path &path, const RecentFilesParams &params) {
  if (!params.category) return true;
  if (vicinae::normalizedExtension(path) == "app") {
    return *params.category == vicinae::FileCategory::Application;
  }
  std::error_code ec;
  return vicinae::fileCategoryFor(path, fs::is_directory(path, ec)) == *params.category;
}

std::vector<fs::path> listRecent(const RecentFilesParams &params) {
  std::vector<fs::path> result;
  if (params.limit <= 0) return result;

  auto const predicate = predicateFor(params);
  CFStringRef queryString =
      CFStringCreateWithCString(kCFAllocatorDefault, predicate.c_str(), kCFStringEncodingUTF8);
  if (!queryString) return result;

  CFStringRef sortKeys[] = {kMDItemLastUsedDate};
  CFArrayRef sortOrder =
      CFArrayCreate(kCFAllocatorDefault, (const void **)sortKeys, 1, &kCFTypeArrayCallBacks);
  MDQueryRef query = MDQueryCreate(kCFAllocatorDefault, queryString, nullptr, sortOrder);
  CFRelease(queryString);
  CFRelease(sortOrder);

  if (!query) return result;

  MDQuerySetSortOptionFlagsForAttribute(query, kMDItemLastUsedDate, kMDQueryReverseSortOrderFlag);

  CFArrayRef scope = scopeFor(params);
  MDQuerySetSearchScope(query, scope, 0);
  CFRelease(scope);

  if (!MDQueryExecute(query, kMDQuerySynchronous)) {
    CFRelease(query);
    return result;
  }

  CFIndex const count = MDQueryGetResultCount(query);
  result.reserve(std::min<CFIndex>(count, params.limit));

  for (CFIndex i = 0; i < count && std::ssize(result) < params.limit; ++i) {
    auto item = (MDItemRef)MDQueryGetResultAtIndex(query, i);
    if (!item) continue;

    auto pathRef = (CFStringRef)MDItemCopyAttribute(item, kMDItemPath);
    if (!pathRef) continue;

    char buf[PATH_MAX];
    if (CFStringGetCString(pathRef, buf, sizeof(buf), kCFStringEncodingUTF8)) {
      fs::path path(buf);
      if (matchesCategory(path, params)) result.emplace_back(std::move(path));
    }
    CFRelease(pathRef);
  }

  CFRelease(query);
  return result;
}

} // namespace

QFuture<std::vector<fs::path>> SpotlightRecentFilesProvider::listAsync(const RecentFilesParams &params) {
  return QtConcurrent::run([params] {
    @autoreleasepool {
      return listRecent(params);
    }
  });
}

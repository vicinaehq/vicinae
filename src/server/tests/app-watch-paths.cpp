#include <QTemporaryDir>
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

#include "../src/services/app-service/app-watch-paths.hpp"

TEST_CASE("application watches follow a replaced profile symlink") {
  namespace fs = std::filesystem;
  QTemporaryDir temp;
  REQUIRE(temp.isValid());
  const fs::path root = temp.path().toStdString();
  const auto profiles = root / "profiles";
  const auto first = root / "store/first/share/applications";
  const auto second = root / "store/second/share/applications";
  fs::create_directories(first);
  fs::create_directories(second);
  fs::create_directories(profiles);
  fs::create_directory_symlink(root / "store/first", profiles / "profile");
  fs::create_directory_symlink("profiles/profile", root / "user");

  const auto path = root / "user/share/applications";
  auto before = appWatchPaths::directories({path});
  CHECK(std::ranges::find(before, root) != before.end());
  CHECK(std::ranges::find(before, profiles) != before.end());
  CHECK(std::ranges::find(before, first) != before.end());

  fs::create_directory_symlink(root / "store/second", profiles / "next");
  fs::rename(profiles / "next", profiles / "profile");
  auto after = appWatchPaths::directories({path});
  CHECK(std::ranges::find(after, profiles) != after.end());
  CHECK(std::ranges::find(after, second) != after.end());
  CHECK(std::ranges::find(after, first) == after.end());

  fs::remove(profiles / "profile");
  auto missing = appWatchPaths::directories({path});
  CHECK(std::ranges::find(missing, profiles) != missing.end());
}

#include "services/app-service/macos/mac-app-database.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace fs = std::filesystem;

// The production ImageURL implementation lives in url.cpp and drags in the whole rendering
// stack (theme, painter, glyph), which this test binary does not link. These definitions
// cover the small surface actually used by the code under test.
ImageURL::ImageURL() { _isValid = true; }

ImageURLType ImageURL::type() const { return _type; }

const QString &ImageURL::name() const { return _name; }

void ImageURL::setType(ImageURLType type) { _type = type; }

void ImageURL::setName(const QString &name) { _name = name; }

ImageURL ImageURL::macBundle(const std::filesystem::path &bundlePath) {
  ImageURL url;
  url.setType(ImageURLType::MacBundle);
  url.setName(QString::fromStdString(bundlePath.string()));
  return url;
}

namespace {

constexpr std::string_view FAKE_BUNDLE_ID = "dev.vicinae.test.FakeApp";

QString fakeBundleId() { return QString::fromStdString(std::string(FAKE_BUNDLE_ID)); }

void createBundle(const fs::path &bundle) {
  fs::create_directories(bundle / "Contents" / "MacOS");

  {
    std::ofstream ofs(bundle / "Contents" / "Info.plist");
    ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
           "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
        << "<plist version=\"1.0\">\n<dict>\n"
        << "<key>CFBundleIdentifier</key><string>" << FAKE_BUNDLE_ID << "</string>\n"
        << "<key>CFBundleName</key><string>FakeApp</string>\n"
        << "<key>CFBundleExecutable</key><string>Fake</string>\n"
        << "<key>CFBundlePackageType</key><string>APPL</string>\n"
        << "</dict>\n</plist>\n";
  }

  fs::path const executable = bundle / "Contents" / "MacOS" / "Fake";
  {
    std::ofstream ofs(executable);
    ofs << "#!/bin/sh\n";
  }
  std::error_code ec;
  fs::permissions(executable, fs::perms::owner_exec, fs::perm_options::add, ec);
}

fs::path makeTempRoot() {
  auto const timestamp = std::chrono::system_clock::now().time_since_epoch().count();
  auto const base = fs::temp_directory_path() / ("vicinae-mac-app-test-" + std::to_string(timestamp));

  for (int attempt = 0; attempt != 100; ++attempt) {
    auto candidate = base;
    if (attempt != 0) { candidate += "-" + std::to_string(attempt); }

    std::error_code ec;
    if (fs::create_directory(candidate, ec)) { return candidate; }
  }

  throw std::runtime_error{"failed to create mac app test root"};
}

struct AppScanEnv {
  fs::path root;

  AppScanEnv() {
    root = makeTempRoot();

    // Real bundle lives under `store`; `scan` only exposes it through a symlinked
    // directory, so it is only reachable when directory symlinks are followed.
    fs::path const store = root / "store" / "Apps";
    createBundle(store / "Fake.app");
    fs::create_directories(root / "scan");
    std::error_code ec;
    fs::create_directory_symlink(fs::relative(store, root / "scan"), root / "scan" / "link", ec);
    if (ec) throw std::runtime_error{"failed to create test symlink"};
  }

  ~AppScanEnv() {
    std::error_code ec;
    fs::remove_all(root, ec);
  }
};

class TestAppDatabase : public MacAppDatabase {
public:
  std::vector<fs::path> roots;

  std::vector<fs::path> searchPaths() const override { return roots; }
};

} // namespace

TEST_CASE("mac app database discovers apps through symlinked directories") {
  AppScanEnv env;
  TestAppDatabase db;
  db.roots = {env.root / "scan"};

  REQUIRE(db.scan());

  auto app = db.findById(fakeBundleId());
  REQUIRE(app);
  REQUIRE(app->path().filename() == "Fake.app");
}

TEST_CASE("mac app database scan terminates on symlink loops") {
  AppScanEnv env;
  std::error_code ec;
  fs::create_directory_symlink(".", env.root / "scan" / "loop", ec);
  REQUIRE_FALSE(ec);

  TestAppDatabase db;
  db.roots = {env.root / "scan"};

  REQUIRE(db.scan());
  REQUIRE(db.findById(fakeBundleId()));
}

TEST_CASE("mac application icon url resolves symlinked bundles to their canonical path") {
  AppScanEnv env;
  fs::path const symlinkedPath = env.root / "scan" / "link" / "Fake.app";

  auto app = MacApplication::fromBundle(symlinkedPath);
  REQUIRE(app);
  REQUIRE(app->path() == symlinkedPath);

  auto url = app->iconUrl();
  REQUIRE(url.type() == ImageURLType::MacBundle);
  REQUIRE(url.name().toStdString() == fs::canonical(symlinkedPath).string());
}

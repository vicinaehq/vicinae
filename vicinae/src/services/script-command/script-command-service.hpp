#pragma once
#include "builtin_icon.hpp"
#include "script-command.hpp"
#include "services/emoji-service/emoji.hpp"
#include "ui/image/url.hpp"
#include "utils.hpp"
#include "vicinae.hpp"
#include "xdgpp/env/env.hpp"
#include <cstdint>
#include <filesystem>
#include <format>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <QtConcurrent/QtConcurrent>
#include <qmimedata.h>
#include <qmimedatabase.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <ranges>
#include <stack>

class ScriptMetadataStore {
  struct RunMetadata {
    std::int64_t lastRunAt;
    std::string output;
  };

  struct Data {
    std::unordered_map<std::string, RunMetadata> inlineRuns;
  };

public:
  ScriptMetadataStore(const std::filesystem::path &path = Omnicast::dataDir() / "script-metadata.json")
      : m_path(path) {
    std::error_code ec;

    if (!std::filesystem::is_regular_file(path, ec)) { syncWithDisk(); }

    auto result = loadDataFromDisk();

    if (!result) {
      qWarning() << "Failed to load script metadata from" << m_path << result.error();
      return;
    }

    m_data = *result;
  }

  void saveRun(std::string_view scriptId, std::string_view line) {
    m_data.inlineRuns[std::string{scriptId}] =
        RunMetadata{.lastRunAt = QDateTime::currentSecsSinceEpoch(), .output = std::string{line}};

    syncWithDisk();
  }

  std::optional<RunMetadata> lastRun(const std::string &id) const {
    if (const auto it = m_data.inlineRuns.find(id); it != m_data.inlineRuns.end()) { return it->second; }
    return {};
  }

private:
  std::optional<std::string> syncWithDisk() {
    std::string buf;

    if (const auto error = glz::write_file_json(m_data, m_path.c_str(), buf)) {
      return glz::format_error(error);
    }

    return {};
  }

  std::expected<Data, std::string> loadDataFromDisk() {
    Data data;
    std::string buf;
    if (const auto error = glz::read_file_json(data, m_path.c_str(), buf)) {
      return std::unexpected(glz::format_error(error));
    }
    return data;
  }

  Data m_data;
  std::filesystem::path m_path;
};

class ScriptCommandFile {
public:
  static std::expected<ScriptCommandFile, std::string> fromFile(const std::filesystem::path &path,
                                                                const std::string &id) {
    if (!std::filesystem::is_regular_file(path)) {
      return std::unexpected(std::format("{} is not a valid file", path.c_str()));
    }

    auto parsed = script_command::ScriptCommand::fromFile(path);

    if (!parsed) return std::unexpected(parsed.error());

    ScriptCommandFile file;

    file.m_data = std::move(parsed.value());
    file.m_path = path;
    file.m_id = id;

    return file;
  }

  std::optional<std::string> reload() {
    auto parsed = script_command::ScriptCommand::fromFile(m_path);

    if (!parsed) return parsed.error();

    m_data = parsed.value();
    return std::nullopt;
  }

  std::string packageName() const {
    if (m_data.mode == script_command::OutputMode::Inline) {
      // FIXME: we probably shouldn't do that here
      const ScriptMetadataStore store;

      if (const auto run = store.lastRun(m_id)) { return run->output; }

      return "No data";
    }

    return m_data.packageName.value_or(m_path.parent_path().filename());
  }

  std::vector<QString> createCommandLine(std::span<const QString> args = {}) const {
    std::vector<QString> cmdline;

    cmdline.reserve(m_data.exec.size() + m_data.arguments.size() + 1);

    for (const std::string &s : m_data.exec) {
      cmdline.emplace_back(s.c_str());
    }

    cmdline.emplace_back(m_path.c_str());

    for (const auto &[arg, value] : std::views::zip(m_data.arguments, args)) {
      if (arg.percentEncoded) {
        cmdline.emplace_back(QUrl::toPercentEncoding(value));
      } else {
        cmdline.emplace_back(value);
      }
    }

    return cmdline;
  }

  ImageURL icon() const {
    if (!m_data.icon) return ImageURL::emoji("🤖");
    std::error_code ec;

    if (StaticEmojiDatabase::mapping().contains(m_data.icon.value())) {
      return ImageURL::emoji(m_data.icon.value().c_str());
    }

    if (std::filesystem::is_regular_file(m_data.icon.value(), ec)) {
      return ImageURL::local(QString::fromStdString(m_data.icon.value()));
    }

    const auto relativePath = m_path.parent_path() / m_data.icon.value();

    if (std::filesystem::is_regular_file(relativePath, ec)) {
      return ImageURL::local(QString::fromStdString(relativePath));
    }

    if (const auto url = QUrl(m_data.icon->c_str()); url.isValid()) {
      if (url.scheme() == "https") { return ImageURL::http(url); }
    }

    return ImageURL(BuiltinIcon::Code).setFill(Omnicast::ACCENT_COLOR);
  }

  const script_command::ScriptCommand &data() const { return m_data; }
  const std::filesystem::path &path() const { return m_path; }
  std::string_view id() const { return m_id; }

private:
  std::filesystem::path m_path;
  std::string m_id;
  script_command::ScriptCommand m_data;
};

struct ScriptScanner {
  struct ScannedDirectory {
    std::string id;
    std::filesystem::path path;
    std::uint8_t depth = 0;
  };

  static constexpr const std::uint8_t MAX_DEPTH = 5;

  static std::vector<std::shared_ptr<ScriptCommandFile>>
  scan(const std::vector<std::filesystem::path> &dirs) {
    std::vector<std::shared_ptr<ScriptCommandFile>> scripts;
    std::error_code ec;
    std::stack<ScannedDirectory> dirStack;
    std::unordered_set<std::string> idsSeen;
    QMimeDatabase mimeDb;

    static const auto forbiddenExtensions = {".md", ".svg", ".txt"};

    for (const auto &dir : dirs) {
      dirStack.emplace(ScannedDirectory("", dir));
    }

    while (!dirStack.empty()) {
      const ScannedDirectory dir = std::move(dirStack.top());
      dirStack.pop();

      for (const auto &ent : std::filesystem::directory_iterator(dir.path, ec)) {
        std::string filename = getLastPathComponent(ent.path());

        if (filename.starts_with('.') || filename.contains(".template")) continue;

        std::string id = dir.id;

        if (!id.empty()) id += '.';

        id += filename;

        if (ent.is_directory() && dir.depth + 1 < MAX_DEPTH) {
          dirStack.emplace(ScannedDirectory(id, ent.path(), dir.depth + 1));
          continue;
        }

        if (idsSeen.contains(id)) continue;

        const auto ext = ent.path().extension();

        if (std::ranges::contains(forbiddenExtensions, ext)) { continue; }

        QMimeType mime = mimeDb.mimeTypeForFile(ent.path().c_str());
        bool plainText = mime.inherits("text/plain");

        if (!plainText) continue;

        auto script = ScriptCommandFile::fromFile(ent.path(), id);

        if (!script) {
          qDebug() << "Failed to parse script at" << ent.path().c_str() << script.error().c_str();
          continue;
        }

        scripts.emplace_back(std::make_shared<ScriptCommandFile>(script.value()));
        idsSeen.insert(id);
      }
    }

    return scripts;
  }
};

class ScriptCommandService : public QObject {
  Q_OBJECT

signals:
  void scriptsChanged() const;

public:
  using Watcher = QFutureWatcher<std::vector<std::shared_ptr<ScriptCommandFile>>>;

  ScriptCommandService() {
    triggerScan();
    connect(&m_scanWatcher, &Watcher::finished, this, [this]() {
      m_scripts = std::move(m_scanWatcher.future().takeResult());
      emit scriptsChanged();
    });
  }

  void setCustomScriptPaths(const std::vector<std::filesystem::path> &paths) {
    m_customScriptPaths = paths;
    triggerScan();
  }

  std::vector<std::filesystem::path> defaultScriptDirectories() const {
    return xdgpp::dataDirs() | std::views::transform([](auto &&p) { return p / "vicinae" / "scripts"; }) |
           std::ranges::to<std::vector>();
  }

  std::vector<std::filesystem::path> scriptDirectories() const {
    return std::views::concat(m_customScriptPaths, defaultScriptDirectories()) |
           std::ranges::to<std::vector>();
  }

  const std::vector<std::shared_ptr<ScriptCommandFile>> &scripts() const { return m_scripts; }

  void triggerScan() {
    m_scanWatcher.setFuture(
        QtConcurrent::run([dirs = scriptDirectories()]() { return ScriptScanner::scan(dirs); }));
  }

private:
  std::vector<std::shared_ptr<ScriptCommandFile>> m_scripts;
  std::vector<std::filesystem::path> m_customScriptPaths;
  Watcher m_scanWatcher;
};

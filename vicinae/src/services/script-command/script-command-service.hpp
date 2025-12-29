#pragma once
#include "script-command.hpp"
#include "services/emoji-service/emoji.hpp"
#include "ui/image/url.hpp"
#include "xdgpp/env/env.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <qlogging.h>
#include <ranges>

class ScriptCommandFile {
public:
  static std::expected<ScriptCommandFile, std::string> fromFile(const std::filesystem::path &path) {
    if (!std::filesystem::is_regular_file(path)) {
      return std::unexpected(std::format("{} is not a valid file", path.c_str()));
    }

    std::ifstream ifs(path);
    auto parsed = script_command::ScriptCommand::parse(ifs);

    if (!parsed) return std::unexpected(parsed.error());

    ScriptCommandFile file;

    file.m_data = std::move(parsed.value());
    file.m_path = path;

    return file;
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

    return {};
  }

  const script_command::ScriptCommand &data() const { return m_data; }
  const std::filesystem::path &path() const { return m_path; }

private:
  std::filesystem::path m_path;
  script_command::ScriptCommand m_data;
};

class ScriptCommandService {
public:
  std::vector<ScriptCommandFile> scanAll() const {
    auto scriptDirs =
        xdgpp::dataDirs() | std::views::transform([](auto &&p) { return p / "vicinae" / "scripts"; });
    std::error_code ec;
    std::vector<ScriptCommandFile> scripts;

    for (const std::filesystem::path &script : scriptDirs) {
      for (const auto &ent : std::filesystem::recursive_directory_iterator(script, ec)) {
        auto script = ScriptCommandFile::fromFile(ent.path());

        if (!script) {
          qWarning() << "Failed to parse script at" << ent.path().c_str() << script.error().c_str();
          continue;
        }

        scripts.emplace_back(script.value());
      }
    }

    qDebug() << "got XXX scripts" << scripts.size();

    return scripts;
  }

private:
};

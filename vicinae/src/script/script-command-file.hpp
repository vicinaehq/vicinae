#pragma once
#include <expected>
#include "script-command.hpp"
#include "script-metadata-store.hpp"
#include "services/emoji-service/emoji.hpp"
#include "ui/image/url.hpp"

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

      if (const auto output = store.lastRunData(m_id)) { return output.value(); }

      return "No data";
    }

    return m_data.packageName.value_or(m_path.parent_path().filename());
  }

  std::vector<QString> createCommandLine(std::span<const QString> args) const {
    std::vector<QString> cmdline;

    if (!data().exec.empty()) {
      for (const auto &exec : data().exec) {
        cmdline.emplace_back(exec.c_str());
      }
    }

    cmdline.emplace_back(path().c_str());

    for (const auto &[arg, value] : std::views::zip(data().arguments, args)) {
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

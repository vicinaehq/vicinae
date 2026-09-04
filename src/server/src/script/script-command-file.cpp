#include <array>
#include <ranges>
#include <QDebug>
#include <QStandardPaths>
#include "script/script-command-file.hpp"
#include "glyph/emoji.hpp"
#include "services/script-command/script-command-service.hpp"
#include "service-registry.hpp"
#include "ui/image/url.hpp"

std::expected<ScriptCommandFile, std::string> ScriptCommandFile::fromFile(const std::filesystem::path &path,
                                                                          const std::string &id) {
  if (!std::filesystem::is_regular_file(path)) {
    return std::unexpected(std::format("{} is not a valid file", path.string()));
  }

  auto parsed = script_command::ScriptCommand::fromFile(path);

  if (!parsed) return std::unexpected(parsed.error());

  ScriptCommandFile file;

  file.m_data = std::move(parsed.value());
  file.m_path = path;
  file.m_id = id;

  return file;
}

std::optional<std::string> ScriptCommandFile::reload() {
  auto parsed = script_command::ScriptCommand::fromFile(m_path);

  if (!parsed) return parsed.error();

  m_data = parsed.value();
  return std::nullopt;
}

std::string ScriptCommandFile::packageName() const {
  if (m_data.mode == script_command::OutputMode::Inline) {
    // FIXME: we probably shouldn't do that here
    auto metadata = ServiceRegistry::instance()->scriptDb()->metadata();

    if (const auto output = metadata->lastRunData(m_id)) { return output.value(); }

    return "No data";
  }

  return m_data.packageName.value_or(m_path.parent_path().filename().string());
}

#ifdef Q_OS_WIN
std::vector<QString> ScriptCommandFile::interpreter() const {
  static constexpr auto ALIASES = std::to_array<std::pair<std::string_view, std::string_view>>({
      {"python3", "python"},
      {"pwsh", "powershell"},
      {"sh", "bash"},
  });

  auto argv = script_command::shebangInterpreter(data().shebang);
  if (argv.empty()) argv = script_command::interpreterForExtension(path().extension().string());
  if (argv.empty()) return {};

  const std::string_view name = argv.front();
  const QString wanted = QString::fromUtf8(name.data(), name.size());
  QString program = QStandardPaths::findExecutable(wanted);
  if (program.isEmpty()) {
    const auto alias =
        std::ranges::find(ALIASES, name, &std::pair<std::string_view, std::string_view>::first);
    if (alias != ALIASES.end()) {
      program = QStandardPaths::findExecutable(QString::fromUtf8(alias->second.data(), alias->second.size()));
    }
  }
  if (program.isEmpty()) {
    qWarning() << "No interpreter found for script" << path().c_str() << "(wanted" << wanted << ")";
    program = wanted;
  }

  std::vector<QString> cmdline;
  cmdline.reserve(argv.size());
  cmdline.emplace_back(std::move(program));
  for (const auto &arg : argv | std::views::drop(1)) {
    cmdline.emplace_back(QString::fromStdString(arg));
  }

  return cmdline;
}
#endif

std::vector<QString> ScriptCommandFile::createCommandLine(std::span<const QString> args) const {
  std::vector<QString> cmdline;

  if (!data().exec.empty()) {
    for (const auto &exec : data().exec) {
      cmdline.emplace_back(exec.c_str());
    }
  }
#ifdef Q_OS_WIN
  else {
    cmdline = interpreter();
  }
#endif

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

ImageURL ScriptCommandFile::icon() const {
  if (!m_data.icon) return defaultIcon();

  std::error_code ec;

  if (emoji::isUtf8EncodedEmoji(m_data.icon.value())) { return ImageURL::emoji(m_data.icon.value().c_str()); }

  if (std::filesystem::is_regular_file(m_data.icon.value(), ec)) {
    return ImageURL::local(QString::fromStdString(m_data.icon.value()));
  }

  const auto relativePath = m_path.parent_path() / m_data.icon.value();

  if (std::filesystem::is_regular_file(relativePath, ec)) {
    return ImageURL::local(QString::fromStdString(relativePath.string()));
  }

  if (const auto url = QUrl(m_data.icon->c_str()); url.isValid()) {
    if (url.scheme() == "https") { return ImageURL::http(url); }
  }

  return defaultIcon();
}

#pragma once
#include <expected>
#include "builtin_icon.hpp"
#include "script-command.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"

class ScriptCommandFile {
public:
  static ImageURL defaultIcon() {
    return ImageURL(BuiltinIcon::Code).setBackgroundTint(SemanticColor::Accent);
  }
  static std::expected<ScriptCommandFile, std::string> fromFile(const std::filesystem::path &path,
                                                                const std::string &id);

  std::optional<std::string> reload();
  std::string packageName() const;
  std::vector<QString> createCommandLine(std::span<const QString> args) const;
  ImageURL icon() const;

  const script_command::ScriptCommand &data() const { return m_data; }
  const std::filesystem::path &path() const { return m_path; }
  std::string_view id() const { return m_id; }

private:
  std::filesystem::path m_path;
  std::string m_id;
  script_command::ScriptCommand m_data;
};

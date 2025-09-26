#pragma once
#include <optional>
#include <vector>
#include <string>

namespace xdgpp {
std::vector<std::string> parseExec(const std::string &exec);
std::vector<std::string> parseExec(const std::string &exec, const std::vector<std::string> &uris);

class ExecParser {
public:
  ExecParser(std::string_view name) : m_name(name) {}

  std::vector<std::string> parse(std::string_view exec, const std::vector<std::string> &args = {}) const;
  void setName(std::string_view name) { m_name = name; }
  void setIcon(std::string_view icon) { m_icon = icon; }

private:
  std::string_view m_name;
  std::optional<std::string_view> m_icon;
};

}; // namespace xdgpp

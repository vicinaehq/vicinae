#pragma once
#include <optional>
#include <vector>
#include <string>

namespace xdgpp {
class ExecParser {
public:
  ExecParser(std::string_view name) : m_name(name) {}

  std::vector<std::string> parse(std::string_view exec, const std::vector<std::string> &args = {}) const;

  /**
   * If no field code was expanded to the set of provided
   * URIs, append all the uris at the end of the command line, one argument
   * per URI.
   * Note that if a single URI field such as %f or %u was expanded this won't
   * append the remaining uris at the end of the command line: these will be lost.
   */
  void setForceAppend(bool value = true) { m_forceAppend = value; }
  void setName(std::string_view name) { m_name = name; }
  void setIcon(std::string_view icon) { m_icon = icon; }

private:
  bool m_forceAppend = false;
  std::string_view m_name;
  std::optional<std::string_view> m_icon;
};

}; // namespace xdgpp

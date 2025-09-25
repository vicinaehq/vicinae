#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace xdgpp {
class DesktopEntryValueType {
public:
  DesktopEntryValueType(std::string_view view);

  std::string asString() const;
  bool asBoolean() const;
  double asNumber() const;
  std::vector<std::string> asStringList();

private:
  static char getEscapeChar(char c);

  std::string m_value;
};
} // namespace xdgpp

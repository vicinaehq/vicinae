#include "value.hpp"

namespace xdgpp {

DesktopEntryValueType::DesktopEntryValueType(std::string_view view) : m_value(view) {}

bool DesktopEntryValueType::asBoolean() const { return m_value == "true"; }

double DesktopEntryValueType::asNumber() const { return std::stod(std::string(m_value)); }

std::string DesktopEntryValueType::asString() const {
  std::string str;
  bool escaped = false;

  for (const auto &c : m_value) {
    if (escaped) {
      char ch = getEscapeChar(c);
      if (ch) { str += ch; }
      escaped = false;
      continue;
    }

    if (c == '\\') {
      escaped = true;
      continue;
    }

    str += c;
  }

  return str;
}

std::vector<std::string> DesktopEntryValueType::asStringList() {
  std::vector<std::string> lst;
  std::string part;
  bool escaped = false;

  for (char c : m_value) {
    if (escaped) {
      if (c == ';')
        part += ';';
      else
        part += std::string("\\") + c;
      escaped = false;
      continue;
    }

    if (c == '\\') {
      escaped = true;
      continue;
    }

    if (c == ';') {
      lst.emplace_back(DesktopEntryValueType(part).asString());
      part.clear();
      continue;
    }

    part += c;
  }

  if (!part.empty()) { lst.emplace_back(part); }

  return lst;
}

char DesktopEntryValueType::getEscapeChar(char c) {
  switch (c) {
  case 's':
    return ' ';
  case 'n':
    return '\n';
  case 't':
    return '\t';
  case 'r':
    return '\r';
  case '\\':
    return '\\';
  }

  return 0;
}
}; // namespace xdgpp

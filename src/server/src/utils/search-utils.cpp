#include "utils/search-utils.hpp"
#include <cctype>

namespace search_utils {

bool hasMathIntent(std::string_view query) {
  while (!query.empty() && std::isspace(static_cast<unsigned char>(query.front()))) {
    query.remove_prefix(1);
  }
  while (!query.empty() && std::isspace(static_cast<unsigned char>(query.back()))) {
    query.remove_suffix(1);
  }

  if (query.size() < 2) return false;
  if (query.starts_with('=')) return true;

  static constexpr std::string_view mathFuncs[] = {"sqrt(", "sin(", "cos(",   "tan(",  "log(",
                                                   "ln(",   "abs(", "round(", "ceil(", "floor("};
  for (auto fn : mathFuncs) {
    if (query.find(fn) != std::string_view::npos) return true;
  }

  bool hasDigit = false;
  bool hasMathOp = false;
  bool hasPercent = false;

  for (size_t i = 0; i < query.size(); ++i) {
    char c = query[i];
    if (std::isdigit(static_cast<unsigned char>(c))) {
      hasDigit = true;
    } else if (c == '*' || c == '/' || c == '^' || c == '(' || c == ')') {
      hasMathOp = true;
    } else if (c == '%') {
      hasPercent = true;
    } else if (c == '+' || c == '-') {
      bool prevOk = (i > 0 && (std::isdigit(static_cast<unsigned char>(query[i - 1])) ||
                               std::isspace(static_cast<unsigned char>(query[i - 1]))));
      bool nextOk = (i + 1 < query.size() && (std::isdigit(static_cast<unsigned char>(query[i + 1])) ||
                                              std::isspace(static_cast<unsigned char>(query[i + 1]))));
      if (prevOk || nextOk) { hasMathOp = true; }
    }
  }

  if (hasDigit && (hasMathOp || hasPercent)) { return true; }

  // Currency or unit conversion: contains digits or currency symbol followed by " to " or " in "
  if (hasDigit || query.starts_with('$') || query.starts_with("\u20AC") || query.starts_with("\u00A3") ||
      query.starts_with("\u00A5")) {
    auto toPos = query.find(" to ");
    if (toPos != std::string_view::npos && toPos > 0 && toPos + 4 < query.size()) { return true; }
    auto inPos = query.find(" in ");
    if (inPos != std::string_view::npos && inPos > 0 && inPos + 4 < query.size()) { return true; }
  }

  return false;
}

} // namespace search_utils

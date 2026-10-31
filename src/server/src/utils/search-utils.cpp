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

std::string extractAcronym(std::string_view text) {
  std::string acronym;
  acronym.reserve(8);

  bool atBoundary = true;
  char prevChar = '\0';

  for (char c : text) {
    if (std::isspace(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '/') {
      atBoundary = true;
      prevChar = c;
      continue;
    }

    if (std::isalnum(static_cast<unsigned char>(c))) {
      bool isCamel =
          std::isupper(static_cast<unsigned char>(c)) && std::islower(static_cast<unsigned char>(prevChar));
      if (atBoundary || isCamel) {
        acronym.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
      }
      atBoundary = false;
    } else {
      atBoundary = true;
    }
    prevChar = c;
  }

  return acronym;
}

double calculateAcronymBonus(std::string_view query, std::string_view title) {
  if (query.size() < 2 || query.size() > 6 || title.empty()) return 0.0;

  std::string qClean;
  qClean.reserve(query.size());
  for (char c : query) {
    if (!std::isspace(static_cast<unsigned char>(c))) {
      qClean.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
  }

  if (qClean.size() < 2) return 0.0;

  std::string acronym = extractAcronym(title);
  if (acronym.empty()) return 0.0;

  if (acronym == qClean) { return 35.0; }

  if (acronym.size() > qClean.size() && acronym.starts_with(qClean)) { return 20.0; }

  return 0.0;
}

} // namespace search_utils

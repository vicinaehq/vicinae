// ai-generated based on the test cases, so please keep cases updated if making changes to this file.
#include "regex-utils.hpp"
#include <cstdlib>

static constexpr bool isCharClass(char c) {
  return c == 'd' || c == 'D' || c == 's' || c == 'S' || c == 'w' || c == 'W';
}

static constexpr bool isAnchor(char c) {
  return c == 'b' || c == 'B' || c == '<' || c == '>' || c == 'A' || c == 'Z';
}

static constexpr bool isSpecialChar(char c) {
  return c == '[' || c == '(' || c == '.' || c == '*' || c == '+' || c == '?' || c == '|' || c == '$' ||
         c == '^' || c == '{';
}

static void addSpaceIfNeeded(std::string &result) {
  if (!result.empty() && result.back() != ' ' && result.back() != '^') { result += ' '; }
}

static void flushCaretIfPending(std::string &result, bool &caretPending) {
  if (caretPending) {
    result += '^';
    caretPending = false;
  }
}

static void addFilteredWord(std::string &filtered, const std::string &word, bool hasCaret,
                            size_t minWordLength) {
  size_t wordLen = word.length() - (hasCaret && !word.empty() ? 1 : 0);
  if (!word.empty() && wordLen >= minWordLength) {
    if (!filtered.empty()) { filtered += ' '; }
    filtered += word;
  }
}

std::string extractStaticCharsFromRegex(std::string_view regex, size_t minWordLength) {
  std::string result;
  bool inBracket = false;
  bool escaped = false;
  bool caretPending = false;
  bool hasAlternation = false;
  int parenDepth = 0;
  bool needSpaceAfterGroup = false;
  bool parenAfterBracket = false;
  int groupWithAlternationDepth = -1;
  size_t groupStartPos = 0;

  for (size_t i = 0; i < regex.length(); ++i) {
    char c = regex[i];

    if (escaped) {
      flushCaretIfPending(result, caretPending);
      if (needSpaceAfterGroup && parenDepth == 0) {
        result += ' ';
        needSpaceAfterGroup = false;
      }

      switch (c) {
      case 't':
        result += '\t';
        break;
      case 'n':
        result += '\n';
        break;
      case 'r':
        result += '\r';
        break;
      case 'v':
        result += '\v';
        break;
      case 'f':
        result += '\f';
        break;
      case 'x':
        if (i + 2 < regex.length()) {
          char hex[3] = {regex[i + 1], regex[i + 2], '\0'};
          result += static_cast<char>(std::strtol(hex, nullptr, 16));
          i += 2;
        }
        break;
      default:
        if (!isCharClass(c) && !isAnchor(c)) { result += c; }
        break;
      }
      escaped = false;
      continue;
    }

    switch (c) {
    case '\\':
      escaped = true;
      caretPending = false;
      continue;
    case '[':
      inBracket = true;
      caretPending = false;
      continue;
    case ']':
      inBracket = false;
      needSpaceAfterGroup = true;
      continue;
    }

    if (inBracket) { continue; }

    switch (c) {
    case '{': {
      size_t closePos = regex.find('}', i);
      if (closePos != std::string_view::npos) {
        caretPending = false;
        needSpaceAfterGroup = false;
        addSpaceIfNeeded(result);
        i = closePos;
        continue;
      }
    }
    case '|':
      if (parenDepth > 0) {
        if (groupWithAlternationDepth == -1) {
          groupWithAlternationDepth = parenDepth;
          result.resize(groupStartPos);
        }
      } else {
        hasAlternation = true;
      }
      continue;
    case '(':
      parenDepth++;
      if (groupWithAlternationDepth == -1) { groupStartPos = result.size(); }
      if (needSpaceAfterGroup) {
        addSpaceIfNeeded(result);
        parenAfterBracket = true;
      }
      needSpaceAfterGroup = false;
      continue;
    case ')':
      if (parenDepth > 0) parenDepth--;
      if (parenDepth == 0 && parenAfterBracket) {
        needSpaceAfterGroup = true;
        parenAfterBracket = false;
      }
      if (groupWithAlternationDepth == parenDepth + 1) { groupWithAlternationDepth = -1; }
      continue;
    case '^':
      if (i + 1 < regex.length() && !isSpecialChar(regex[i + 1]) && regex[i + 1] != '\\') {
        caretPending = true;
      }
      continue;
    case '.':
      caretPending = false;
      needSpaceAfterGroup = false;
      addSpaceIfNeeded(result);
      continue;
    case '?':
      if (i > 0 && (regex[i - 1] == '*' || regex[i - 1] == '+' || regex[i - 1] == '?')) { continue; }
      if (!result.empty()) {
        if (result.back() == ' ') { result.pop_back(); }
        if (!result.empty() && result.back() != '^') { result.pop_back(); }
      }
      caretPending = false;
      needSpaceAfterGroup = false;
      continue;
    case '*':
    case '+':
      caretPending = false;
      needSpaceAfterGroup = false;
      addSpaceIfNeeded(result);
      continue;
    case '$':
      caretPending = false;
      needSpaceAfterGroup = false;
      continue;
    }

    if (groupWithAlternationDepth > 0 && parenDepth >= groupWithAlternationDepth) { continue; }

    if (needSpaceAfterGroup && parenDepth == 0) { addSpaceIfNeeded(result); }
    if (parenDepth == 0) { needSpaceAfterGroup = false; }

    flushCaretIfPending(result, caretPending);
    result += c;
  }

  if (hasAlternation) { return ""; }

  std::string cleanResult;
  for (char c : result) {
    auto uc = static_cast<unsigned char>(c);
    if (std::isalnum(uc) || c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f' ||
        c == '^') {
      cleanResult += c;
    } else if (!std::isspace(uc) && !cleanResult.empty() && cleanResult.back() != ' ') {
      cleanResult += ' ';
    }
  }

  while (!cleanResult.empty() && cleanResult.back() == ' ') {
    cleanResult.pop_back();
  }

  if (minWordLength == 0) { return cleanResult; }

  std::string filtered, currentWord;
  bool hasCaret = false;

  for (char c : cleanResult) {
    if (c == ' ') {
      addFilteredWord(filtered, currentWord, hasCaret, minWordLength);
      currentWord.clear();
      hasCaret = false;
    } else {
      if (c == '^' && currentWord.empty()) { hasCaret = true; }
      currentWord += c;
    }
  }

  addFilteredWord(filtered, currentWord, hasCaret, minWordLength);
  return filtered;
}

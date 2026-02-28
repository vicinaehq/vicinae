#include "exec.hpp"

static bool isQuoteChar(char c) { return c == '"' || c == '\''; }

namespace xdgpp {

std::vector<std::string> ExecParser::parse(std::string_view data,
                                           const std::vector<std::string> &uris) const {

  std::vector<std::string> args;
  enum class State { Reset, FieldCode, Escaped, Quote, QuotedEscaped } state = State::Reset;
  size_t i = 0;
  const size_t uriIdx = 0;
  std::string part;
  bool uriExpanded = false;
  char quoteChar = 0; // the current quotation char

  while (i < data.size()) {
    const char ch = data[i];

    switch (state) {
    case State::Reset: {
      if (isQuoteChar(ch)) {
        state = State::Quote;
        quoteChar = ch;
      } else if (ch == '%') {
        state = State::FieldCode;
      } else if (ch == '\\') {
        state = State::Escaped;
      } else if (std::isspace(ch)) {
        if (!part.empty()) {
          args.emplace_back(part);
          part.clear();
        }
      } else {
        part += ch;
      }
      break;
    }
    case State::FieldCode:
      if (ch == '%') {
        part += '%';
        state = State::Reset;
        break;
      }

      switch (ch) {
      case 'f':
      case 'u':
        uriExpanded = true;
        if (!uris.empty()) args.emplace_back(uris.at(0));
        break;
      case 'F':
      case 'U':
        uriExpanded = true;
        args.insert(args.end(), uris.begin(), uris.end());
        break;
      case 'i':
        if (m_icon) {
          args.emplace_back("--icon");
          args.emplace_back(*m_icon);
        }
        break;
      case 'c':
        args.emplace_back(m_name);
        break;
      default:
        break;
      }

      state = State::Reset;
      break;

    case State::Escaped:
      part += ch;
      state = State::Reset;
      break;
    case State::Quote: {
      if (ch == '\\') {
        state = State::QuotedEscaped;
      } else if (ch == quoteChar) {
        state = State::Reset;
      } else {
        part += ch;
      }
      break;
    }
    case State::QuotedEscaped:
      part += ch;
      state = State::Quote;
      break;
    }

    ++i;
  }

  if (!part.empty()) { args.emplace_back(part); }
  if (!uriExpanded && m_forceAppend) { args.insert(args.end(), uris.begin(), uris.end()); }

  return args;
}
}; // namespace xdgpp

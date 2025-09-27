#include "exec.hpp"

static bool isQuoteChar(char c) { return c == '"' || c == '\''; }

namespace xdgpp {

std::vector<std::string> ExecParser::parse(std::string_view data,
                                           const std::vector<std::string> &uris) const {

  std::vector<std::string> args;
  enum State { Reset, FieldCode, Escaped, Quote, QuotedEscaped } state = Reset;
  size_t i = 0;
  size_t uriIdx = 0;
  std::string part;
  bool uriExpanded = false;
  char quoteChar = 0; // the current quotation char

  while (i < data.size()) {
    char ch = data[i];

    switch (state) {
    case Reset: {
      if (isQuoteChar(ch)) {
        state = Quote;
        quoteChar = ch;
      } else if (ch == '%') {
        state = FieldCode;
      } else if (ch == '\\') {
        state = Escaped;
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
    case FieldCode:
      if (ch == '%') {
        part += '%';
        state = Reset;
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
      }

      state = Reset;
      break;

    case Escaped:
      part += ch;
      state = Reset;
      break;
    case Quote: {
      if (ch == '\\') {
        state = QuotedEscaped;
      } else if (ch == quoteChar) {
        state = Reset;
      } else {
        part += ch;
      }
      break;
    }
    case QuotedEscaped:
      part += ch;
      state = Quote;
      break;
    }

    ++i;
  }

  if (!part.empty()) { args.emplace_back(part); }
  if (!uriExpanded && m_forceAppend) { args.insert(args.end(), uris.begin(), uris.end()); }

  return args;
}
}; // namespace xdgpp

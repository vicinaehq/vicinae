#include "script-output-tokenizer.hpp"
#include "ui/omni-painter/omni-painter.hpp"

ScriptOutputTokenizer::ScriptOutputTokenizer(QStringView str) : m_data(str) {}

std::optional<ScriptOutputTokenizer::Token> ScriptOutputTokenizer::next() {
  static const auto urlSchemes = {QStringLiteral("http://"), QStringLiteral("https://")};

  if (m_cursor >= m_data.size()) return {};

  size_t beginIdx = m_cursor;
  Token tok;

  std::vector<uint8_t> colorCodes;

  while (m_cursor < m_data.size()) {
    QChar ch = m_data.at(m_cursor);
    QStringView rem = m_data.sliced(m_cursor);

    switch (m_state) {
    case State::Normal: {
      if (ch == '\033') {
        if (tok.fmt) return tok;
        m_state = State::Escape;
      } else {
        bool maybeUrl = std::ranges::any_of(urlSchemes, [&](auto s) { return rem.startsWith(s); });

        if (maybeUrl) {
          m_state = Url;
          return tok;
        }

        tok.text.append(ch);
      }
      break;
    }
    case State::Url: {
      if (isValidUrlChar(ch)) {
        tok.text.append(ch);
      } else {
        m_state = State::Normal;
        tok.url = true;
        return tok;
      }
      break;
    }
    case State::Escape: {
      if (ch == '[') {
        if (!tok.text.isEmpty()) { return tok; }
        colorCodes.clear();
        colorCodes.emplace_back(0);
        m_state = State::Color;
      }
      break;
    }
    case State::Color: {
      if (ch.isNumber()) {
        char n = ch.toLatin1() - 48;
        colorCodes.back() = colorCodes.back() * 10 + n;
      } else if (ch == ';') {
        colorCodes.emplace_back(0);
      } else if (ch == 'm') {
        tok.fmt = parseColor(colorCodes);
        m_state = Normal;
      }
      break;
    }
    }

    ++m_cursor;
  }

  return tok;
}

size_t ScriptOutputTokenizer::cursor() const { return m_cursor; }

void ScriptOutputTokenizer::setCursor(size_t cursor) { m_cursor = cursor; }

ScriptOutputTokenizer::Format ScriptOutputTokenizer::parseColor(const std::vector<std::uint8_t> &codes) {
  Format fmt;

  for (const auto code : codes) {
    if (code >= 30 && code <= 37) {
      fmt.fg = parseFgColor(code);
    } else if (code >= 40 && code <= 47) {
      fmt.bg = parseFgColor(code - 10);
    } else if (code == 0) {
      fmt.reset = true;
    } else if (code == 1) {
      // should brighten next foreground color, we ignore this for now
    } else if (code >= 90 && code <= 97) {
      fmt.fg = parseFgColor(code - 60);
    }
  }

  return fmt;
}

QColor ScriptOutputTokenizer::parseBgColor(int code) {
  return OmniPainter::resolveColor(getStandardColor(code - 10));
}

bool ScriptOutputTokenizer::isValidUrlChar(QChar c) {
  return c.isPrint() && !c.isSpace() && c != '"' && c != '\'' && c != ')' && c != '(';
}

QColor ScriptOutputTokenizer::parseFgColor(std::uint8_t code) {
  return OmniPainter::resolveColor(getStandardColor(code));
}

ColorLike ScriptOutputTokenizer::getStandardColor(std::uint8_t code) {
  switch (code) {
  case 0:
    return QColor(Qt::transparent);
  case 30:
    return QColor(Qt::black);
  case 31:
    return SemanticColor::Red;
  case 32:
    return SemanticColor::Green;
  case 33:
    return SemanticColor::Yellow;
  case 34:
    return SemanticColor::Blue;
  case 35:
    return SemanticColor::Magenta;
  case 36:
    return SemanticColor::Cyan;
  case 97:
    return QColor(Qt::white);
  default:
    return SemanticColor::TextPrimary;
  }
}

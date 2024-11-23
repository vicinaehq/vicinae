#include "calculator.hpp"
#include <iostream>

static const std::string_view hexCharset = "0123456789ABCDEF";
static const std::string_view decCharset = "0123456789";
static const std::string_view octalCharset = "01234567";

Tokenizer::Tokenizer(std::string_view input) noexcept
    : input(input), current(next()) {}

std::optional<Token> Tokenizer::peak() const { return current; }

std::optional<Token> Tokenizer::consume() {
  auto tmp = current;
  current = next();
  return tmp;
}

void Tokenizer::reset() {
  cursor = 0;
  state = START;
}

std::optional<Token> Tokenizer::next() {
  size_t start = cursor, end = cursor;
  size_t i = cursor;
  double number = 0;
  double fraction = 0;
  double exponent = 0;
  std::string_view baseCharset = decCharset;

  for (; i <= input.size(); ++i) {
    char ch = i < input.size() ? input.at(i) : 0;
    std::string_view view{input.begin() + start, input.begin() + end};

    switch (state) {
    case START:
      if (std::isdigit(ch)) {
        if (ch == '0')
          state = NUMBER_LITERAL;
        else {
          state = NUMBER;
          --i;
        }
      } else if (std::isblank(ch)) {
        state = WHITESPACE;
        ++end;
      } else if (std::ispunct(ch)) {
        state = OPERATOR;
        ++end;
      } else if (std::isalpha(ch)) {
        state = STRING;
        ++end;
      }
      break;

    case NUMBER_EXPONENT:
      if (ch == '-' || ch == '+') {
        if (ch == '-')
          exponent *= -1;
        break;
      }
      if (auto pos = baseCharset.find(ch); pos != std::string::npos) {
        exponent = exponent * baseCharset.size() + pos;
        break;
      }

      --i;
      state = NUMBER;
      break;

    case NUMBER_FRACTION:
      if (auto pos = baseCharset.find(ch); pos != std::string::npos) {
        fraction = fraction * baseCharset.size() + pos;
        break;
      }

      while (fraction >= 1)
        fraction /= baseCharset.size();

      --i;
      state = NUMBER;
      break;

    case NUMBER_LITERAL:
      switch (tolower(ch)) {
      case 'x':
        baseCharset = hexCharset;
        break;
      case 'b':
        baseCharset = "01";
        break;
      case '.':
        --i;
        break; // not octal
      default:
        baseCharset = octalCharset;
        break;
      }
      state = NUMBER;
      start = end;
      break;

    case NUMBER:
      if (tolower(ch) == 'e') {
        state = NUMBER_EXPONENT;
        break;
      }

      if (ch == '.') {
        state = NUMBER_FRACTION;
        break;
      }

      if (auto pos = baseCharset.find(ch); pos != std::string::npos) {
        number = number * baseCharset.size() + pos;
        break;
      }

      state = START;
      cursor = i;
      return commitToken({view, (number + fraction) * std::pow(10, exponent),
                          Token::Type::NUMBER});

    case OPERATOR:
      state = START;
      cursor = i;
      return commitToken({view, view, Token::Type::OPERATOR});
    case STRING:
      if (std::isalnum(ch))
        ++end;
      else {
        state = START;
        cursor = i;

        if (auto it = variables.find(view); it != variables.end()) {
          return commitToken({view, it->second, Token::Type::NUMBER});
        }

        return commitToken({view, view, Token::Type::STRING});
      }
      break;

    case WHITESPACE:
      if (!std::isblank(ch)) {
        start = end;
        state = START;
        --i;
      } else
        ++end;
      break;

    default:
      break;
    }
  }

  return std::nullopt;
}

#include "calculator.hpp"
#include <cctype>
#include <iostream>
#include <unordered_map>

struct Operator {};

// clang-format off
static const std::unordered_map<std::string_view, OperatorType> opMap = {
	{"+", OperatorType::PLUS},
	{"-", OperatorType::MINUS},
	{"*", OperatorType::STAR},
	{"/", OperatorType::DIV},
	{"%", OperatorType::MOD},
	{"^", OperatorType::CARET},
	{">>", OperatorType::LSHIFT},
	{"<<", OperatorType::RSHIFT},
	{"~", OperatorType::TILDE},
	{"in", OperatorType::IN},
};
// clang-format on

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
  double exponentSign = 1;
  std::string_view baseCharset = decCharset;

  for (;;) {
    char ch = i < input.size() ? input.at(i) : 0;
    std::string_view view{input.begin() + start, input.begin() + end};

    switch (state) {
    case START:
      if (!ch)
        return std::nullopt;
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
      } else {
        state = STRING;
        ++end;
      }
      break;

    case NUMBER_EXPONENT:
      if (ch == '-' || ch == '+') {
        if (ch == '-')
          exponentSign *= -1;
        ++end;
        break;
      }
      if (auto pos = baseCharset.find(ch); pos != std::string::npos) {
        exponent = exponent * baseCharset.size() + pos;
        ++end;
        break;
      }

      exponent *= exponentSign;
      --i;
      state = NUMBER;
      break;

    case NUMBER_FRACTION:
      if (auto pos = baseCharset.find(ch); pos != std::string::npos) {
        fraction = fraction * baseCharset.size() + pos;
        ++end;
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
      if (std::ispunct(ch)) {
        ++end;
        break;
      }

      cursor = i;
      state = START;
      start = end;

      if (view == "(")
        return commitToken({view, view, Token::Type::LPAREN});
      if (view == ")")
        return commitToken({view, view, Token::Type::RPAREN});
      if (auto it = opMap.find(view); it != opMap.end()) {
        return commitToken({view, view, Token::Type::OPERATOR});
      }

      // unknown operator is tokenized as a string
      return commitToken({view, view, Token::Type::STRING});

    case STRING:
      if (std::isalnum(ch)) {
        ++end;
        break;
      }

      state = START;
      cursor = i;

      if (auto it = opMap.find(view); it != opMap.end()) {
        return commitToken({view, view, Token::Type::OPERATOR});
      }

      std::cout << "stoken=" << view << std::endl;

      return commitToken({view, view, Token::Type::STRING});

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

    ++i;
  }

  return std::nullopt;
}

#include "calculator.hpp"
#include <iostream>

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
  int sign = 1;

  for (; i != input.size(); ++i) {
    char ch = input.at(i);
    std::string_view view{input.begin() + start, input.begin() + end};

    switch (state) {
    case START:
      /*
if ((ch == '+' || ch == '-') && (!prev || prev->isUnaryPredictor())) {
sign = ch == '-' ? -sign : sign;
state = SIGN;
++end;
std::cout << "new sign:" << sign << std::endl;
}
    */

      if (std::isdigit(ch)) {
        state = NUMBER;
        ++end;
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

    case SIGN:
      if (ch == '+' || ch == '-') {
        sign = ch == '-' ? -sign : sign;
        ++end;
      } else {
        start = end;
        state = START;
        --i;
      }
      break;

    case OPERATOR:
      state = START;
      cursor = i;
      return commitToken({view, view, Token::Type::OPERATOR});
    case NUMBER:
      if (std::isdigit(ch) || ch == '.')
        ++end;
      else {
        state = START;
        cursor = i;
        double v = std::stod(view.data()) * sign;

        return commitToken({view, v, Token::Type::NUMBER});
      }
      break;
    case STRING:
      if (std::isalpha(ch))
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

  std::string_view s{input.begin() + start, input.begin() + end};

  cursor = i;

  std::optional<Token> token = std::nullopt;

  switch (state) {
  case NUMBER:
    std::cout << "sign=" << sign << std::endl;
    token = {s, std::stod(s.data()) * sign, Token::Type::NUMBER};
    break;
  case STRING:
    token = {s, s, Token::Type::STRING};
    break;
  case OPERATOR:
    token = {s, s, Token::Type::OPERATOR};
    break;
  default:
    break;
  }

  state = START;

  return token;
}

#pragma once
#include <cctype>
#include <format>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

enum class TokenType {
  Identifier,
  Service,
  Method,
  Enum,
  Struct,
  Event,
  Colon,
  Arrow,
  LParen,
  RParen,
  LBrace,
  RBrace,
  LBracket,
  RBracket,
  Comma,
  Semicolon,

  TypeString,
  TypeNumber,
};

static const std::unordered_map<std::string_view, TokenType> TYPES{
    {"string", TokenType::TypeString},
    {"number", TokenType::TypeNumber},
};

struct Token {
  TokenType type;
  std::string_view data;

  std::string_view typeName() const {
    using T = TokenType;

    switch (type) {
    case T::Identifier:
      return "Identifer";
    case T::Service:
      return "Service";
    case T::Method:
      return "Method";
    case T::Enum:
      return "Enum";
    case T::Struct:
      return "Type";
    case T::Event:
      return "Event";
    case T::Colon:
      return "Colon";
    case T::Arrow:
      return "Arrow";
    case T::LParen:
      return "LParen";
    case T::RParen:
      return "RParen";
    case T::LBrace:
      return "LBrace";
    case T::RBrace:
      return "RBrace";
    case T::LBracket:
      return "LBracket";
    case T::RBracket:
      return "RBracket";
    case T::Comma:
      return "Comma";
    case T::Semicolon:
      return "Semicolon";
    case T::TypeString:
      return "String";
    case T::TypeNumber:
      return "Number";
    }
  }

  bool isType() const { return TYPES.contains(data); }
};

class Lexer {
public:
  struct Cursor {
    int line = 0;
    int column = 0;
  };
  explicit Lexer(std::string_view data) : m_data(data) {}

  static Token tryParseToken(std::string_view word) {
    using T = TokenType;

    if (word == "struct") { return Token(T::Struct); }
    if (word == "enum") { return Token(T::Enum); }
    if (word == "service") { return Token(T::Service); }
    if (word == "event") { return Token(T::Event); }
    if (word == "fn") { return Token(T::Method); }
    if (word == "=>") { return Token(T::Arrow); }
    if (word == "(") { return Token(T::LParen); }
    if (word == ")") { return Token(T::RParen); }
    if (word == "{") { return Token(T::LBrace); }
    if (word == "}") { return Token(T::RBrace); }
    if (word == ":") { return Token(T::Colon); }
    if (word == ",") { return Token(T::Comma); }
    if (word == ";") { return Token(T::Semicolon); }

    if (auto it = TYPES.find(word); it != TYPES.end()) { return Token(it->second); }

    return Token(T::Identifier);

    return {};
  }

  std::optional<Token> peak() { return m_current; }

  std::string_view data() const { return m_data; }

  Cursor pos() const {
    Cursor cs;

    for (int i = 0; i != cursor && i < m_data.size(); ++i) {
      char c = m_data[i];
      if (c == '\n') {
        ++cs.line;
        cs.column = 0;
      } else {
        ++cs.column;
      }
    }

    return cs;
  }

  std::optional<Token> getNext() {
    m_current = getNextImpl();
    return m_current;
  }

  std::optional<Token> getNextImpl() {
    enum { Reset, Operator, Word, ForwardSlash, SingleLineComment } state = Reset;
    size_t start = 0;

    while (cursor < m_data.size()) {
      std::string_view view = m_data.substr(start, cursor - start);
      char c = m_data.at(cursor);

      switch (state) {
      case Reset: {
        if (std::isalnum(c)) {
          state = Word;
          start = cursor;
          break;
        }

        else if (c == '/') {
          state = ForwardSlash;
          break;
        } else if (!std::isspace(c)) {
          if (c == '{') {
            ++cursor;
            return Token(TokenType::LBrace, view);
          }
          if (c == '}') {
            ++cursor;
            return Token(TokenType::RBrace, view);
          }
          if (c == '(') {
            ++cursor;
            return Token(TokenType::LParen, view);
          }
          if (c == ')') {
            ++cursor;
            return Token(TokenType::RParen, view);
          }
          if (c == '[') {
            ++cursor;
            return Token(TokenType::LBracket, view);
          }
          if (c == ']') {
            ++cursor;
            return Token(TokenType::RBracket, view);
          }
          state = Operator;
          start = cursor;
          break;
        }

        break;
      }
      case ForwardSlash:
        if (c == '/') { state = SingleLineComment; }
        break;
      case SingleLineComment:
        if (c == '\n') { state = Reset; }
        break;
      case Operator: {
        if (std::isalnum(c) || std::isspace(c)) {
          if (start != cursor) {
            auto tok = tryParseToken(view);
            tok.data = view;
            return tok;
          }
        }
        break;
      }
      case Word: {
        if (!std::isalnum(c)) {
          if (start != cursor) {
            auto tok = tryParseToken(view);
            tok.data = view;
            return tok;
          }
        }
        break;
      }
      }

      ++cursor;
    }

    return {};
  }

  Token getNextOfTypeOrThrow(TokenType type, std::string_view reason = "unexpected token") {
    auto tok = getNext();
    if (!tok) throw std::runtime_error("No more token");
    if (tok->type != type)
      throw std::runtime_error(std::format("{} (got token {})", reason, tok->typeName()));
    return tok.value();
  }

private:
  std::optional<Token> m_current;
  std::string_view m_data;
  size_t cursor = 0;
};

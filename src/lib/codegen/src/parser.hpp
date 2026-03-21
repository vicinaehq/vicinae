#pragma once
#include "lexer.hpp"
#include <format>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

struct TypeStruct;

enum class DataType {
  String,
  Number,
};

struct EnumValue {
  std::string_view name;
  std::vector<std::string_view> values;
};

struct TypeValue {
  std::variant<DataType, TypeStruct *, EnumValue> data;
  bool isArray = false;
};

struct MethodParameter {
  std::string_view name;
  TypeValue type;
};

struct Method {
  std::string_view name;
  std::vector<MethodParameter> params;
  TypeValue returnType;
};

struct Event {
  std::string_view name;
  std::vector<MethodParameter> params;
};

struct Service {
  std::string_view name;
  std::vector<Method> methods;
  std::vector<Event> events;
};

struct StructField {
  std::string_view name;
  TypeValue type;
};

struct TypeStruct {
  std::string_view name;
  std::vector<StructField> fields;
};

struct Tree {
  std::vector<std::unique_ptr<Service>> services;
  std::vector<std::unique_ptr<TypeStruct>> structs;
  std::vector<EnumValue> enums;
};

class Parser {
public:
  static Tree parseTree(std::string_view data) { return Parser{data}.parse(); }

  Parser(std::string_view data) : m_lexer(data) {}

  Tree parse() {
    m_tree.structs.reserve(16);
    m_lexer.getNext();

    while (auto tok = m_lexer.peak()) {
      if (tok->type == TokenType::Struct) {
        auto s = parseStruct();
        std::cout << "added type" << s->name;
        m_tree.structs.emplace_back(s);
      }
      if (tok->type == TokenType::Service) { m_tree.services.emplace_back(parseService()); }
      if (tok->type == TokenType::Semicolon) {
        m_lexer.getNext();
        continue;
      }
      if (tok->type == TokenType::Enum) {
        auto ev = parseEnum();
        m_tree.enums.emplace_back(ev);
      }
    }

    return std::move(m_tree);
  }

  std::optional<TypeValue> resolveType(std::string_view name) {
    for (const auto &s : m_tree.structs) {
      if (s->name == name) { return TypeValue{s.get()}; }
    }
    for (const auto &s : m_tree.enums) {
      if (s.name == name) { return TypeValue{s}; }
    }
    return std::nullopt;
  }

  TypeValue parseTypeValue() {
    auto typeTok = m_lexer.peak();

    if (typeTok->type == TokenType::Identifier) {
      auto type = resolveType(typeTok->data);
      if (!type) { throw std::runtime_error(std::format("{} is not a valid type", typeTok->data)); }
      m_lexer.getNext();
      return type.value();
    }

    m_lexer.getNext();
    return TypeValue{DataType::String};
  }

  TypeValue parseType() {
    auto type = parseTypeValue();

    if (auto tok = m_lexer.peak(); tok->type == TokenType::LBracket) {
      m_lexer.getNext();
      if (auto tok2 = m_lexer.peak(); tok2->type == TokenType::RBracket) {
        type.isArray = true;
        m_lexer.getNext();
      }
    }

    return type;
  }

  Service *parseService() {
    assertGetNext(TokenType::Service, "Expected service token");

    auto service = new Service;
    service->name = assertGetNext(TokenType::Identifier, "expected identifier after service keyword").data;
    assertGetNext(TokenType::LBrace, "expected lbrace to define service");

    while (peakUnless(TokenType::RBrace)) {
      auto tok = m_lexer.peak();

      if (tok->type == TokenType::Method) {
        service->methods.emplace_back(parseMethod());
      } else if (tok->type == TokenType::Event) {
        service->events.emplace_back(parseEvent());
      } else {
        throw std::runtime_error(R"(Expected "fn" or "event" keyword in service)");
      }
    }

    assertGetNext(TokenType::RBrace, "rbrace expected to close service");

    return service;
  }

  std::vector<MethodParameter> parseParameterList() {
    std::vector<MethodParameter> params;

    assertGetNext(TokenType::LParen, "expected ( to start method parameters");

    while (auto tok = m_lexer.peak()) {
      MethodParameter param;
      if (tok->type == TokenType::RParen) { break; }
      if (tok->type == TokenType::Comma) {
        m_lexer.getNext();
        continue;
      }

      param.name = assertGetNext(TokenType::Identifier).data;
      assertGetNext(TokenType::Colon, "expected colon");
      param.type = parseType();

      params.emplace_back(param);
    }

    assertGetNext(TokenType::RParen, "rparen at the end of parameter list was expected");

    return params;
  }

  // assert current tok is of type and then go next
  Token assertGetNext(TokenType type, const std::string &reason = "assertPeak failed") {
    auto tok = m_lexer.peak();
    if (!tok || tok->type != type) { throw std::runtime_error(reason); }
    m_lexer.getNext();
    return tok.value();
  }

  std::optional<Token> peakUnless(TokenType type) {
    auto tok = m_lexer.peak();
    if (tok->type == type) return std::nullopt;
    return tok;
  }

  Method parseMethod() {
    Method method{};

    assertGetNext(TokenType::Method, "expected fn keyword before method");
    method.name = assertGetNext(TokenType::Identifier, "Expected identifier after \"fn\"").data;
    method.params = this->parseParameterList();

    assertGetNext(TokenType::Arrow, "Expected => <return_type> for service method");
    method.returnType = parseType();
    assertGetNext(TokenType::Semicolon, "expected semicolon at the end of service method");

    return method;
  }

  Event parseEvent() {
    Event event{};

    assertGetNext(TokenType::Event, "Expected event");
    event.name = assertGetNext(TokenType::Identifier, "Expected identifier after \"event\"").data;
    event.params = parseParameterList();
    assertGetNext(TokenType::Semicolon, "expected semicolon at the end of event method");

    return event;
  }

  EnumValue parseEnum() {
    EnumValue ev;

    assertGetNext(TokenType::Enum, "expected enum kw");
    ev.name = assertGetNext(TokenType::Identifier, "expected identifier after enum keyword").data;
    assertGetNext(TokenType::LBrace, "expected lbrace");

    while (auto tok = peakUnless(TokenType::RBrace)) {
      if (tok->type == TokenType::Comma) {
        m_lexer.getNext();
        continue;
      }
      if (tok->type != TokenType::Identifier) { throw std::runtime_error("Expected identifier in enum"); }

      ev.values.emplace_back(tok->data);
      m_lexer.getNext();
    }

    assertGetNext(TokenType::RBrace);

    return ev;
  }

  TypeStruct *parseStruct() {
    auto type = new TypeStruct;
    assertGetNext(TokenType::Struct, "Expected struct");
    type->name = assertGetNext(TokenType::Identifier, "expected identifier after struct keyword").data;
    assertGetNext(TokenType::LBrace, "expected lbrace after struct name");

    while (peakUnless(TokenType::RBrace)) {
      StructField field;

      field.name = assertGetNext(TokenType::Identifier).data;
      assertGetNext(TokenType::Colon, "expected colon after struct field name");
      field.type = parseType();

      assertGetNext(TokenType::Semicolon, "expected semicolon at the end of struct field");

      type->fields.emplace_back(field);
    }

    assertGetNext(TokenType::RBrace);

    return type;
  }

private:
  Tree m_tree;
  Lexer m_lexer;
};

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

struct Service {
  std::string_view name;
  std::vector<Method> methods;
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

    while (auto tok = m_lexer.getNext()) {
      if (tok->type == TokenType::Struct) {
        auto s = parseStruct();
        std::cout << "added type" << s->name;
        m_tree.structs.emplace_back(s);
      }
      if (tok->type == TokenType::Service) { m_tree.services.emplace_back(parseService()); }
      if (tok->type == TokenType::Semicolon) continue;
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
    auto typeTok = m_lexer.getNext();
    if (typeTok->type == TokenType::Identifier) {
      auto type = resolveType(typeTok->data);
      if (!type) { throw std::runtime_error(std::format("{} is not a valid type", typeTok->data)); }
      return type.value();
    }

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
    auto service = new Service;
    service->name =
        m_lexer.getNextOfTypeOrThrow(TokenType::Identifier, "expected identifier after service keyword").data;
    m_lexer.getNextOfTypeOrThrow(TokenType::LBrace, "expected lbrace");

    while (auto tok = m_lexer.getNext()) {
      if (tok->type == TokenType::RBrace) { break; }
      if (tok->type != TokenType::Method) {
        throw std::runtime_error("Expected \"fn\" keyboard before service method");
      }

      tok = m_lexer.getNextOfTypeOrThrow(TokenType::Identifier, "Expected identifier after \"fn\"");

      Method method;

      method.name = tok->data;
      m_lexer.getNextOfTypeOrThrow(TokenType::LParen, "expected ( to start method parameters");

      while (auto tok = m_lexer.getNext()) {

        MethodParameter param;
        if (tok->type == TokenType::RParen) { break; }
        if (tok->type == TokenType::Comma) { continue; }

        param.name = tok->data;
        m_lexer.getNextOfTypeOrThrow(TokenType::Colon, "expected colon");
        param.type = parseType();

        method.params.emplace_back(param);
      }

      m_lexer.getNextOfTypeOrThrow(TokenType::Arrow, "Expected => <return_type> for service method");
      method.returnType = parseType();
      m_lexer.getNextOfTypeOrThrow(TokenType::Semicolon, "expected semicolon at the end of service method");

      service->methods.emplace_back(method);
    }

    return service;
  }

  EnumValue parseEnum() {
    EnumValue ev;

    ev.name =
        m_lexer.getNextOfTypeOrThrow(TokenType::Identifier, "expected identifier after enum keyword").data;
    m_lexer.getNextOfTypeOrThrow(TokenType::LBrace, "expected lbrace");

    while (auto tok = m_lexer.getNext()) {
      if (tok->type == TokenType::RBrace) { break; }
      if (tok->type == TokenType::Comma) { continue; }
      if (tok->type != TokenType::Identifier) { throw std::runtime_error("Expected identifier in enum"); }

      ev.values.emplace_back(tok->data);
    }

    return ev;
  }

  TypeStruct *parseStruct() {
    auto type = new TypeStruct;
    type->name =
        m_lexer.getNextOfTypeOrThrow(TokenType::Identifier, "expected identifier after struct keyword").data;
    m_lexer.getNextOfTypeOrThrow(TokenType::LBrace, "expected lbrace");

    while (auto tok = m_lexer.getNext()) {
      if (tok->type == TokenType::RBrace) { break; }
      StructField field;

      field.name = tok->data;
      m_lexer.getNextOfTypeOrThrow(TokenType::Colon, "expected colon");
      field.type = parseType();

      m_lexer.getNextOfTypeOrThrow(TokenType::Semicolon, "expected semicolon at the end of struct field");

      type->fields.emplace_back(field);
    }

    return type;
  }

private:
  Tree m_tree;
  Lexer m_lexer;
};

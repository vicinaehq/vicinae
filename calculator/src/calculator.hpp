#pragma once

#include <cctype>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <strings.h>
#include <variant>
#include <vector>

// clang-format off
struct Unit {
  enum Type { 
	  Seconds, Minutes, Hours, Days, Years, 
	  Grams, Kilograms,
	  Ounces, Pounds, TonsUS,
	  Millimeters, Centimeters, Meters, Kilometers,
	  Inches, Feet, Yards, Miles

  };
  enum Category { Time, Weight, Length, Currency };

  Type type;
  Category category;
  std::vector<std::string_view> names;
  std::string_view displayName;
  double conversionFactor;
};

struct Function {
	std::string_view name;
	std::function<double(double arg)> fn;
};

static std::vector<Function> functions = {
	{ "sqrt", [](double x) { return std::sqrt(x); } },
	{ "cos", [](double x) { return std::cos(x); } },
	{ "acos", [](double x) { return std::acos(x); } },
	{ "cosh", [](double x) { return std::cosh(x); } },
	{ "cos", [](double x) { return std::cos(x); } },
};

static std::map<std::string_view, double> variables = {
	{"PI", M_PI},
	{"E", M_E}
};

// clang-format on

std::optional<Unit> findUnitByName(std::string_view s);

struct Token {
  using ValueType = std::variant<double, std::string_view>;
  enum Type { NUMBER, STRING, OPERATOR, UNIT, LPAREN, RPAREN };

  std::string_view raw;
  ValueType data;
  Type type;

  bool isNumber() const { return type == NUMBER; };
  bool isString() const { return type == STRING; };
  bool isOperator() const { return type == OPERATOR; };
  bool isOperand() const { return type != OPERATOR; };
  bool isUnaryPredictor() const {
    return type == OPERATOR && *asOperator() != ")";
  };

  const double *asNumber() const {
    if (auto p = std::get_if<double>(&data))
      return p;
    return nullptr;
  }

  const std::string_view *asOperator() const {
    if (auto p = std::get_if<std::string_view>(&data);
        p && type == Type::OPERATOR)
      return p;
    return nullptr;
  }

  const std::string_view *asString() const {
    if (auto p = std::get_if<std::string_view>(&data);
        p && type == Type::STRING)
      return p;
    return nullptr;
  }
};

static std::map<Token::Type, std::string_view> map = {
    {Token::Type::OPERATOR, "OPERATOR"},
    {Token::Type::STRING, "STRING"},
    {Token::Type::NUMBER, "NUMBER"},
};

enum State {
  START,
  NUMBER,
  WHITESPACE,
  OPERATOR,
  STRING,
  SIGN,
};

class Tokenizer {
  std::string_view input;
  State state = START;
  size_t cursor = 0;
  std::optional<Token> current;
  Token commitToken(const Token &token) { return token; }

public:
  operator bool() { return cursor < input.size(); }

  Tokenizer(std::string_view input) noexcept;

  std::optional<Token> peak() const;
  std::optional<Token> consume();
  std::optional<Token> next();
  void reset();
};

static std::map<std::string_view, size_t> precedenceTable = {
    {"in", 2}, {"+", 1}, {"-", 1}, {"/", 2}, {"*", 2}, {"^", 2},
};

class AST {
public:
  class Node;

  struct NumericValue {
    double value;
    std::string unit;
  };

  struct UnaryExpression {
    std::string_view op;
    Node *value;
  };

  struct BinaryExpression {
    std::string_view op;
    Node *lhs;
    Node *rhs;
  };

  struct FunctionCall {
    std::string_view name;
    std::vector<Node *> args;
  };

  struct StringLiteral {
    std::string_view raw;
  };

  struct UnitLiteral {
    Unit unit;
  };

  class Node {
  public:
    using NodeValue = std::variant<FunctionCall, NumericValue, BinaryExpression,
                                   StringLiteral, UnitLiteral, UnaryExpression>;

    template <typename T> T *valueAs() { return std::get_if<T>(&data); }

    NodeValue data;
    Node(const NodeValue &data);
    ~Node();
  };

  Tokenizer &tk;
  Node *root;

  bool isOperator(const Token &tok);

  Node *parseExpression(size_t minPrecedence = 1);
  Node *parseString();
  Node *parseUnaryExpression();
  Node *parseTerm();

  void printNode(Node *root, size_t depth = 0);

public:
  void print() { printNode(root); }

  AST(Tokenizer &tokenizer) noexcept;

  Node *parse();

  ~AST();
};

class Parser {
public:
  Parser();

  struct ComputationResult {
    double value;
    std::optional<Unit> unit;

    ComputationResult(double value) : value(value), unit(std::nullopt) {}
  };

private:
  bool isArithmeticOperator(std::string_view op);
  ComputationResult evaluateBinExpr(AST::BinaryExpression *expr);
  ComputationResult evaluateFunctionCall(const AST::FunctionCall &call);
  ComputationResult evaluateUnaryExpr(const AST::UnaryExpression &unexpr);
  ComputationResult evaluateNode(AST::Node *root);
  std::string preprocess(std::string_view input);

public:
  double evaluate(std::string_view expression);
};

#pragma once

#include <cctype>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <strings.h>
#include <variant>
#include <vector>

#define RETURN_IF_ERR(expr)                                                    \
  ({                                                                           \
    auto _result = (expr);                                                     \
    if (!_result) {                                                            \
      return _result.error();                                                  \
    }                                                                          \
    *_result;                                                                  \
  })

class CalculatorError {
  std::string message;

public:
  CalculatorError(std::string_view message) : message(message) {}

  std::string_view what() const { return message; }
};

class ParseError : public CalculatorError {
public:
  ParseError(std::string_view message) : CalculatorError(message) {}
};

class UsageError : public CalculatorError {
public:
  UsageError(std::string_view message) : CalculatorError(message) {}
};

template <class T, class U> class Result {
  std::variant<T, U> data;

public:
  operator bool() { return std::get_if<T>(&data); }
  const T &operator*() { return *std::get_if<T>(&data); }
  const T &operator->() { return *std::get_if<T>(&data); }
  const U &error() { return *std::get_if<U>(&data); }
  const T &value() { return *std::get_if<T>(&data); }

  Result(const T &p) : data(p) {}
  Result(const U &p) : data(p) {}
};

// clang-format off
struct OutputFormat {
	enum Type { Decimal, Hex, Base64 };

	Type type;
	std::vector<std::string_view> names;
	std::string_view displayName;
};

struct Unit {
  enum Type { 
	  Milliseconds, Seconds, Minutes, Hours, Days, Years, 
	  Bytes, Kilobytes, Megabytes, Gigabytes, Kibibytes, Mebibytes, Gibibytes,
	  Grams, Kilograms,
	  Ounces, Pounds, TonsUS,
	  Millimeters, Centimeters, Meters, Kilometers,
	  Inches, Feet, Yards, Miles

  };
  enum Category { Time, Weight, Length, Currency, DigitalStorage };

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

static std::map<std::string_view, double> variables = {
	{"PI", M_PI},
	{"E", M_E}
};

// clang-format on

std::optional<std::reference_wrapper<const Unit>>
findUnitByName(std::string_view s);
std::optional<std::reference_wrapper<OutputFormat>>
findFormatByName(std::string_view name);

// unary or binary
enum OperatorType {
  PLUS,
  MINUS,
  STAR,
  DIV,
  MOD,
  CARET,
  LSHIFT,
  RSHIFT,
  TILDE,
  IN
};

struct Token {
  using ValueType = std::variant<double, std::string_view, OperatorType>;
  enum Type {
    NUMBER,
    STRING,
    OPERATOR,
    UNIT,
    LPAREN,
    RPAREN,
  };

  std::string_view raw;
  ValueType data;
  Type type;

  bool isNumber() const { return type == NUMBER; };
  bool isString() const { return type == STRING; };
  bool isOperator() const { return type == OPERATOR; };
  bool isOperand() const { return type != OPERATOR; };

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
  NUMBER_LITERAL,
  NUMBER_EXPONENT,
  NUMBER_FRACTION,
  WHITESPACE,
  OPERATOR,
  STRING,
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
    {"as", 2}, {"in", 1}, {"+", 1}, {"-", 1}, {"/", 2}, {"*", 2}, {"^", 2},
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
    const Unit &unit;
  };

  struct FormatLiteral {
    const OutputFormat &format;
  };

  class Node {
  public:
    using NodeValue = std::variant<FunctionCall, NumericValue, BinaryExpression,
                                   StringLiteral, UnitLiteral, UnaryExpression,
                                   FormatLiteral>;

    template <typename T> T *valueAs() { return std::get_if<T>(&data); }

    NodeValue data;
    Node(const NodeValue &data);
    ~Node();
  };

  Tokenizer &tk;
  Node *root;

  bool isOperator(const Token &tok);

  Result<Node *, CalculatorError> parseExpression(size_t minPrecedence = 1);
  Result<Node *, CalculatorError> parseString();
  Result<Node *, CalculatorError> parseUnaryExpression();
  Result<Node *, CalculatorError> parseTerm();

  void printNode(Node *root, size_t depth = 0);

public:
  void print() { printNode(root); }

  AST(Tokenizer &tokenizer) noexcept;

  Result<Node *, CalculatorError> parse();

  ~AST();
};

class Parser {
public:
  Parser();

  struct Number {
    double value;
    std::optional<Unit> unit;
    std::optional<std::reference_wrapper<const OutputFormat>> format;

    Number(double value)
        : value(value), unit(std::nullopt), format(std::nullopt) {}
  };

private:
  bool isArithmeticOperator(std::string_view op);
  Result<Number, CalculatorError> evaluateBinExpr(AST::BinaryExpression *expr);
  Result<Number, CalculatorError>
  evaluateFunctionCall(const AST::FunctionCall &call);
  Result<Number, CalculatorError>
  evaluateUnaryExpr(const AST::UnaryExpression &unexpr);
  Result<Number, CalculatorError> evaluateNode(AST::Node *root);
  std::string preprocess(std::string_view input);

public:
  Result<Number, CalculatorError> evaluate(std::string_view expression);
};

std::ostream &operator<<(std::ostream &lhs, const Parser::Number &num);

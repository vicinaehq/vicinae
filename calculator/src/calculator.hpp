#pragma once

#include <cctype>
#include <climits>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <strings.h>
#include <variant>
#include <vector>

struct Unit {
  enum Type { Seconds, Minutes, Hours, Days, Years, Grams, Kilograms };
  enum Category { Time, Weight };

  Type type;
  Category category;
  std::vector<std::string_view> names;
};

// clang-format off
static std::vector<Unit> units = {
	{
		.type = Unit::Type::Seconds,
		.category = Unit::Category::Time,
		.names = {  "second", "seconds", "secs" },
	},
	{
		.type = Unit::Type::Minutes,
		.category = Unit::Category::Time,
		.names = {  "minute", "minutes", "mins" },
	},
	{
		.type = Unit::Type::Hours,
		.category = Unit::Category::Time,
		.names = {  "hour", "hours", "hrs" },
	},
	{
		.type = Unit::Type::Days,
		.category = Unit::Category::Time,
		.names = {  "day", "days" },
	},
	{
		.type = Unit::Type::Years,
		.category = Unit::Category::Time,
		.names = {  "year", "years" },
	},
	{
		.type = Unit::Type::Grams,
		.category = Unit::Category::Weight,
		.names = {  "g", "gram", "grams" },
	},
};

static std::optional<Unit> findUnitByName(std::string_view s) {
	for (const auto& unit: units) {
		for (const auto& name: unit.names) {
			if (strncasecmp(name.data(), s.data(), name.size()) == 0) return unit;
		}
	}

	return std::nullopt;
}

struct Token {
  using ValueType = std::variant<double, std::string_view>;
  enum Type { NUMBER, STRING, OPERATOR };

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

static std::ostream &operator<<(std::ostream &lhs, const Token &rhs) {
  std::string_view type = "UNKOWN";

  if (auto it = map.find(rhs.type); it != map.end())
    type = it->second;

  std::string formattedValue;

  if (auto dblp = std::get_if<double>(&rhs.data)) {
    formattedValue = std::to_string(*dblp);
  } else if (auto sv = std::get_if<std::string_view>(&rhs.data)) {
    formattedValue = *sv;
  }

  lhs << type << "(" << formattedValue << ")";

  return lhs;
}

enum State {
  START,
  NUMBER,
  WHITESPACE,
  OPERATOR,
  STRING,
  SIGN,
};

static std::string serializedPI = std::to_string(M_PI);
static std::string serializedE = std::to_string(M_E);

static std::vector<std::pair<std::string_view, std::string_view>>
    replacementTable = {
        {"to the power of", "^"}, {"divided by", "/"}, {"multiplied by", "*"},
        {"mul by", "*"},          {"div by", "/"},     {"div", "/"},
        {"times", "*"},           {"mul", "*"},        {"plus", "+"},
        {"minus", "-"},           {"add", "+"},        {"sub", "-"},
        {"power", "^"},           {"pow", "^"},        {"by", "*"},
};

static std::string preprocess(std::string_view input) {
  std::string final;
  size_t i = 0;

  while (i < input.size()) {
    bool replaced = false;
    for (const auto &[target, replacement] : replacementTable) {
      // replacement cannot fit
      if (input.size() - i < target.size())
        continue;

      if (strncasecmp(input.data() + i, target.data(), target.size()) == 0) {
        final += replacement;
        i += target.size();
        replaced = true;
        break;
      }
    }

    if (!replaced)
      final.push_back(input.at(i++));
  }

  return final;
}

class Tokenizer {
  std::optional<Token> prev;
  std::string_view input;
  State state = START;
  size_t cursor = 0;
  std::optional<Token> current;

public:
  operator bool() { return cursor < input.size(); }

  Tokenizer(std::string_view input) : input(input) { current = next(); }

  std::optional<Token> peak() { return current; }
  std::optional<Token> consume() {
    auto old = current;
    current = next();
    return old;
  }

  void reset() {
    cursor = 0;
    state = START;
  }

  std::vector<Token> collect() {
    std::vector<Token> toks;

    while (auto tok = next())
      toks.push_back(*tok);

    return toks;
  }

  Token commitToken(const Token &token) {
    prev = token;
    return token;
  }

  std::optional<Token> next() {
    size_t start = cursor, end = cursor;
    size_t i = cursor;
    int sign = 1;

    for (; i != input.size(); ++i) {
      char ch = input.at(i);
      std::string_view view{input.begin() + start, input.begin() + end};

      switch (state) {
      case START:
        if ((ch == '+' || ch == '-') && (!prev || prev->isUnaryPredictor())) {
          sign = ch == '-' ? -sign : sign;
          state = SIGN;
          ++end;
          std::cout << "new sign:" << sign << std::endl;
        } else if (std::isdigit(ch)) {
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
};

static double evaluateRPN(const std::vector<Token> tokens) {
  std::stack<double> values;

  for (const auto &tok : tokens) {
    std::cout << "output=" << tok << std::endl;
    if (auto p = tok.asNumber()) {
      values.push(*p);
      continue;
    }

    if (values.size() < 2)
      return NAN;

    auto op = tok.asOperator();

    if (!op)
      return NAN;

    double rhs = values.top();
    values.pop();
    double lhs = values.top();
    values.pop();

    if (*op == "+")
      values.push(lhs + rhs);
    if (*op == "-")
      values.push(lhs - rhs);
    if (*op == "/")
      values.push(lhs / rhs);
    if (*op == "*")
      values.push(lhs * rhs);
    if (*op == "^")
      values.push(std::pow(lhs, rhs));
  }

  if (values.empty())
    return NAN;

  return values.top();
}

struct NumericValue {
  double value;
  std::string unit;
};

struct FunctionCall;
struct NumericValue;
struct BinaryExpression;
struct StringLiteral;

using Node = std::variant<FunctionCall, NumericValue, BinaryExpression, StringLiteral, Unit>;

struct BinaryExpression {
  std::string_view op;
 Node *lhs;
 Node
      *rhs;
};

struct FunctionCall {
  std::string_view name;
  std::vector<Node*>
      args;
};

struct StringLiteral {
  std::string_view raw;
};

static std::map<std::string_view, size_t> precedenceTable = {
    {"in", 1}, {"+", 1}, {"-", 1}, {"/", 2}, {"*", 2}, {"^", 3},
};

struct AST {
  Tokenizer &tk;
  Token current;

  std::vector<Node *> program;

  bool isOperator(const Token &tok) {
    if (tok.isOperator()) {
      return true;
    }

    if (auto s = tok.asString()) {
      if (*s == "in" || *s == "to" || *s == "into")
        return true;
    }

    return false;
  }

  Node *parseExpression(size_t minPrecedence = 1) {
    auto left = parseTerm();

    while (tk.peak() && tk.peak()->raw != ")") {
      auto op = tk.peak();

	  std::string_view finalOp = "";

	  bool shouldConsume = false;

	  if (findUnitByName(op->raw)) {
		  finalOp = "in";
	  } else if (isOperator(*op)) {
		  finalOp = op->raw;
		  shouldConsume = true;
	  }

	  if (finalOp.empty()) break ;

      auto precedence = precedenceTable[finalOp];

      if (precedence < minPrecedence) {
        break;
      }

	  if (shouldConsume) tk.consume();

      Node *right = parseExpression(precedence + 1);

      left = new Node(BinaryExpression(finalOp, left, right));
    }

    return left;
  }

  Node *parseTerm() {
    if (!tk.peak())
      return nullptr;

    if (tk.peak()->isNumber()) {
      double num = *tk.peak()->asNumber();

      tk.consume();

      if (tk.peak() && tk.peak()->isString()) {
        std::string s(*tk.peak()->asString());
        tk.consume();

        return new Node(NumericValue{num, s});
      }

	  tk.consume();

      return new Node(NumericValue{num});
    }

    if (tk.peak()->isString()) {
      auto name = tk.consume();

	  if (auto unit = findUnitByName(name->raw)) {
		  return new Node{*unit};
	  }

      // function if next token is call
      if (tk.peak()->raw == "(") {
        FunctionCall call;

        call.name = name->raw;

        tk.consume();
        while (tk.peak() && tk.peak()->raw != ")") {
          auto arg = parseExpression();

          call.args.push_back(arg);

          if (tk.peak()->raw != ",")
            break;
          tk.consume();
        }

        if (tk.peak() && tk.peak()->raw != ")") {
          throw std::runtime_error("expected ) at the end of function call");
        }

        tk.consume();

        return new Node{call};
      }

      return new Node{StringLiteral{name->raw}};
    }

    if (tk.peak()->raw == "(") {
      tk.consume();
      auto expr = parseExpression();
      tk.consume();

      return expr;
    }

    return nullptr;
  }

  void printNode(Node *root, size_t depth = 0) {
    std::string padding;

    for (size_t i = 0; i != depth; ++i)
      padding += '\t';

    if (auto binexpr = std::get_if<BinaryExpression>(root)) {
      std::cout << padding << "BinaryExpression: operator " << binexpr->op
                << std::endl;
      printNode(binexpr->lhs, depth + 1);
      printNode(binexpr->rhs, depth + 1);
    }
    if (auto num = std::get_if<NumericValue>(root)) {
      std::cout << padding << "NumericValue (" << num->unit << ") => "
                << num->value << std::endl;
    }
    if (auto fn = std::get_if<FunctionCall>(root)) {

      std::cout << padding << "FunctionCall " << fn->name << "(\n";
      for (size_t i = 0; i != fn->args.size(); ++i) {
        printNode(fn->args.at(i), depth + 1);
      }

      std::cout << ")" << std::endl;
    }
    if (auto conv = std::get_if<StringLiteral>(root)) {
      std::cout << padding << "String: " << conv->raw << std::endl;
    }
    if (auto unit = std::get_if<Unit>(root)) {
      std::cout << padding << "Unit: " << unit->type << std::endl;
	}
  }

  void print() {
    for (const auto &node : program) {
      printNode(node);
    }
  }

  AST(Tokenizer &tokenizer) : tk(tokenizer) {
    while (tk.peak()) {
      std::cout << "peak " << *tk.peak() << std::endl;
      program.push_back(parseExpression());
    }
  }
};

class Parser {
public:
  Parser() {}

  struct ComputationResult {
	  double value;
	  std::optional<Unit> unit;

	  ComputationResult(double value): value(value), unit(std::nullopt) {
	  }
  };

  bool isArithmeticOperator(std::string_view op) {
	  return op == "+" || op == "-" || op == "/" || op == "*" || op == "^";
  }

  ComputationResult evaluateBinExpr(BinaryExpression* expr) {
	  if (isArithmeticOperator(expr->op)) {
		  auto lhs = evaluateNode(expr->lhs);
		  auto rhs = evaluateNode(expr->rhs);

		  if (lhs.unit) {
			  std::cout << "lhs unit ";
		  }

		  if (rhs.unit) {
			  std::cout << "rhs unit ";
		  }

		  if (lhs.unit && rhs.unit) {
			  if (lhs.unit->category != rhs.unit->category) 
			  	throw std::runtime_error("Operation on incompatible units");
			  std::cout << "converting " << lhs.unit->type << " to " << rhs.unit->type << std::endl;
		  }

		  // todo: unit aware computations
		  if (expr->op == "+") return lhs.value + rhs.value;
		  if (expr->op == "-") return lhs.value - rhs.value;
		  if (expr->op == "*") return lhs.value * rhs.value;
		  if (expr->op == "/") return lhs.value / rhs.value;
	  }

	  if (expr->op == "in") {
		  auto lhs = evaluateNode(expr->lhs);
		  auto rhs = std::get_if<Unit>(expr->rhs);
		  auto res = ComputationResult(lhs);

		  res.unit = *rhs;

		  return res;
	  }

	  throw std::runtime_error("Unknown operator " + std::string{expr->op});
  }

  ComputationResult evaluateNode(Node* root)  {
	  if (auto binexpr = std::get_if<BinaryExpression>(root)) {
		  return evaluateBinExpr(binexpr);
	  }
	  if (auto num = std::get_if<NumericValue>(root)) {
		  ComputationResult comp(num->value);

		  if (!num->unit.empty())
			  comp.unit = findUnitByName(num->unit);

		  return comp;
	  }

	  return 0;
  }

  double evaluate(std::string_view expression) {
    std::string preprocessed = preprocess(expression);
    Tokenizer tokenizer(preprocessed);
    AST ast(tokenizer);

	Node* root = ast.program.at(0);
	

    ast.print();
	std::cout << "result=" << evaluateNode(root).value << std::endl;

    /*

std::stack<Token> operators;
std::vector<Token> output;
std::map<std::string_view, int> precedenceTable = {
    {"+", 1}, {"-", 1}, {"/", 2}, {"*", 2}, {"^", 2},
};

while (auto tok = tokenizer.next()) {
  if (tok->isNumber()) {
    output.push_back(*tok);
  } else if (tok->isOperator() && *tok->asOperator() == "(") {
    operators.push(*tok);
  } else if (tok->isOperator() && *tok->asOperator() == ")") {
    while (!operators.empty() && operators.top().raw != "(") {
      output.push_back(operators.top());
      operators.pop();
    }

    operators.pop();
  } else {
    while (!operators.empty() && precedenceTable[operators.top().raw] >=
                                     precedenceTable[tok->raw]) {
      output.push_back(operators.top());
      operators.pop();
    }

    operators.push(*tok);
  }
}

while (!operators.empty()) {
  output.push_back(operators.top());
  operators.pop();
}

double result = evaluateRPN(output);

std::cout << "result: " << result << std::endl;

tokenizer.reset();
    */

    return 0;
  }
};

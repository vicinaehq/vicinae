#include "calculator.hpp"
#include <cctype>
#include <format>
#include <iostream>
#include <strings.h>

static std::vector<Function> functions = {
    {"sqrt", [](double x) { return std::sqrt(x); }},
    {"cos", [](double x) { return std::cos(x); }},
    {"acos", [](double x) { return std::acos(x); }},
    {"cosh", [](double x) { return std::cosh(x); }},
    {"cos", [](double x) { return std::cos(x); }},
    {"round", [](double x) { return std::round(x); }},
    {"ceil", [](double x) { return std::ceil(x); }},
};

static std::optional<Function> findFunctionByName(std::string_view name) {
  for (const auto &fn : functions) {
    if (fn.name == name)
      return fn;
  }

  return std::nullopt;
}

Parser::Parser() {}

bool Parser::isArithmeticOperator(std::string_view op) {
  return op == "+" || op == "-" || op == "/" || op == "*" || op == "^";
}

Result<Parser::Number, CalculatorError>
Parser::evaluateBinExpr(AST::BinaryExpression *expr) {
  if (isArithmeticOperator(expr->op)) {
    auto lhs = RETURN_IF_ERR(evaluateNode(expr->lhs));
    auto rhs = RETURN_IF_ERR(evaluateNode(expr->rhs));

    /*
std::cout << lhs.value;

if (lhs.unit) {
  std::cerr << "(" << lhs.unit->displayName << ")";
}

std::cerr << " " << expr->op << rhs.value;

if (rhs.unit) {
  std::cerr << "(" << rhs.unit->displayName << ")";
}

std::cerr << std::endl;
    */

    Number res(0);

    res.unit = lhs.unit ? lhs.unit : rhs.unit;

    auto scaledRhs = rhs.value;

    if (lhs.unit && rhs.unit) {
      if (lhs.unit->category != rhs.unit->category)
        return UsageError("Incompatible units");

      scaledRhs =
          rhs.value * (rhs.unit->conversionFactor / lhs.unit->conversionFactor);
    }

    // todo: unit aware computations
    if (expr->op == "+")
      res.value = lhs.value + scaledRhs;
    if (expr->op == "-")
      res.value = lhs.value - scaledRhs;
    if (expr->op == "*")
      res.value = lhs.value * scaledRhs;
    if (expr->op == "/")
      res.value = lhs.value / scaledRhs;
    if (expr->op == "^")
      res.value = std::pow(lhs.value, scaledRhs);
    if (expr->op == "%")
      res.value = static_cast<long>(lhs.value) % static_cast<long>(scaledRhs);

    return res;
  }

  if (expr->op == "in" || expr->op == "as") {
    auto lhs = RETURN_IF_ERR(evaluateNode(expr->lhs));
    Number res(lhs.value);

    if (auto format = expr->rhs->valueAs<AST::FormatLiteral>()) {
      res.format = format->format;

      return res;
    }

    auto rhs = expr->rhs->valueAs<AST::UnitLiteral>();

    if (!rhs) {
      return ParseError(
          "unit literal expected at the right hand side of in operator");
    }

    res.unit = rhs->unit;

    if (lhs.unit) {
      if (lhs.unit->category != rhs->unit.category) {
        return UsageError("Incompatible units");
      }

      res.value *= (lhs.unit->conversionFactor / rhs->unit.conversionFactor);
    }

    if (auto unit = expr->lhs->valueAs<AST::UnitLiteral>()) {
      res.value = (unit->unit.conversionFactor / rhs->unit.conversionFactor);
    }

    std::cerr << "set unit to " << res.unit->displayName << std::endl;

    return res;
  }

  return ParseError("Unknown operator " + std::string{expr->op});
}

Result<Parser::Number, CalculatorError>
Parser::evaluateFunctionCall(const AST::FunctionCall &call) {
  auto func = findFunctionByName(call.name);

  if (!func)
    return ParseError(std::string("Function ") + call.name.data() +
                      " is not available");

  if (call.args.empty())
    return ParseError("No arguments for function");

  auto rhs = RETURN_IF_ERR(evaluateNode(call.args.at(0)));
  Number res(func->fn(rhs.value));

  res.unit = rhs.unit;

  return res;
}

Result<Parser::Number, CalculatorError>
Parser::evaluateUnaryExpr(const AST::UnaryExpression &unexpr) {
  if (unexpr.op == "+")
    return evaluateNode(unexpr.value);

  if (unexpr.op == "-") {
    Number res(RETURN_IF_ERR(evaluateNode(unexpr.value)));

    res.value *= -1;

    return res;
  }

  if (unexpr.op == "~") {
    Number n = RETURN_IF_ERR(evaluateNode(unexpr.value));

    n.value = ~static_cast<unsigned long>(n.value);

    return n;
  }

  return ParseError(std::string("unsupported unary expr ") +
                    std::string(unexpr.op));
}

Result<Parser::Number, CalculatorError> Parser::evaluateNode(AST::Node *root) {
  if (!root)
    return Number(0);

  if (auto binexpr = root->valueAs<AST::BinaryExpression>()) {
    return evaluateBinExpr(binexpr);
  }

  if (auto unexpr = root->valueAs<AST::UnaryExpression>()) {
    return evaluateUnaryExpr(*unexpr);
  }

  if (auto num = root->valueAs<AST::NumericValue>()) {
    Number comp(num->value);

    if (!num->unit.empty())
      comp.unit = findUnitByName(num->unit);

    return comp;
  }

  if (auto fn = root->valueAs<AST::FunctionCall>()) {
    return evaluateFunctionCall(*fn);
  }

  if (auto unit = root->valueAs<AST::UnitLiteral>()) {
    Number result(1);

    result.unit = unit->unit;

    return result;
  }

  return ParseError("Cannot evaluate node");
}

// clang-format off
static std::vector<std::pair<std::string_view, std::string_view>>
    subsitutionMap{
        {"plus", "+"},
        {"minus", "-"},
        {"divided by", "/"},
        {"multiplied by", "*"},
        {"times", "*"},
        {"power", "^"},
		{"**", "^"},
        {"to the power of", "^"},
		{"% of", "/100 *"},
		{"percent of", "/100 *"},
    };
// clang-format on 
	
std::string Parser::preprocess(std::string_view view) {
  std::string s;
  size_t i = 0;

  while (i < view.size()) {
    bool replaced = false;
    std::string_view sub{view.begin() + i, view.end()};

      for (const auto& [k, v] : subsitutionMap) {
		 if (sub.size() < k.size()) continue ;

        if (strncasecmp(k.data(), sub.data(), k.size()) == 0) {
          s += v;
          i += k.size();
          replaced = true;
		  break ;
        }
      }

    if (!replaced) {
      s += view.at(i);
      i++;
    }
  }

  return s;
}

Result<Parser::Number, CalculatorError> Parser::evaluate(std::string_view expression) {
  std::string preprocessed = preprocess(expression);
  Tokenizer tokenizer(preprocessed);
  AST ast(tokenizer);


  auto root = RETURN_IF_ERR(ast.parse());

  ast.print();

  return evaluateNode(root);
}

std::ostream& operator<<(std::ostream& lhs, const Parser::Number& num) {
	lhs << num.value;

	if (num.unit)
		lhs << " " << num.unit->displayName;

	return lhs;
}

#include "calculator.hpp"
#include <cctype>
#include <iostream>
#include <strings.h>

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

Parser::ComputationResult Parser::evaluateBinExpr(AST::BinaryExpression *expr) {
  if (isArithmeticOperator(expr->op)) {
    auto lhs = evaluateNode(expr->lhs);
    auto rhs = evaluateNode(expr->rhs);

    std::cout << lhs.value;

    if (lhs.unit) {
      std::cout << "(" << lhs.unit->displayName << ")";
    }

    std::cout << " " << expr->op << rhs.value;

    if (rhs.unit) {
      std::cout << "(" << rhs.unit->displayName << ")";
    }

    std::cout << std::endl;

    ComputationResult res(0);

    res.unit = lhs.unit ? lhs.unit : rhs.unit;

    auto scaledRhs = rhs.value;

    if (lhs.unit && rhs.unit) {
      if (lhs.unit->category != rhs.unit->category)
        throw std::runtime_error(
            std::string("Operation on incompatible units: ") +
            lhs.unit->displayName.data() + " with " +
            rhs.unit->displayName.data());

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

    return res;
  }

  if (expr->op == "in") {
    auto lhs = evaluateNode(expr->lhs);

    auto rhs = expr->rhs->valueAs<AST::UnitLiteral>();

    if (!rhs) {
      throw std::runtime_error(
          "unit literal expected at the right hand side of in operator");
    }

    ComputationResult res(lhs.value);

    res.unit = rhs->unit;

    if (lhs.unit) {
      if (lhs.unit->category != rhs->unit.category)
        throw std::runtime_error(
            std::string("Operation on incompatible units: ") +
            lhs.unit->displayName.data() + " with " +
            rhs->unit.displayName.data());

      res.value *= (lhs.unit->conversionFactor / rhs->unit.conversionFactor);
    }

    if (auto unit = expr->lhs->valueAs<AST::UnitLiteral>()) {
      res.value = (unit->unit.conversionFactor / rhs->unit.conversionFactor);
    }

    std::cout << "set unit to " << res.unit->displayName << std::endl;

    return res;
  }

  throw std::runtime_error("Unknown operator " + std::string{expr->op});
}

Parser::ComputationResult
Parser::evaluateFunctionCall(const AST::FunctionCall &call) {
  auto func = findFunctionByName(call.name);

  if (!func)
    throw std::runtime_error(std::string("Function ") + call.name.data() +
                             " is not available");

  if (call.args.empty())
    return 0;

  auto rhs = evaluateNode(call.args.at(0));

  auto ret = func->fn(rhs.value);

  ComputationResult res(ret);

  res.unit = rhs.unit;

  return res;
}

Parser::ComputationResult
Parser::evaluateUnaryExpr(const AST::UnaryExpression &unexpr) {
  if (unexpr.op == "+") {
    return evaluateNode(unexpr.value);
  }
  if (unexpr.op == "-") {
    ComputationResult res(evaluateNode(unexpr.value));

    res.value *= -1;

    return res;
  }

  throw std::runtime_error(std::string("unsupported unary expr ") +
                           unexpr.op.data());
}

Parser::ComputationResult Parser::evaluateNode(AST::Node *root) {
  if (!root)
    return ComputationResult(0);

  if (auto binexpr = root->valueAs<AST::BinaryExpression>()) {
    return evaluateBinExpr(binexpr);
  }

  if (auto unexpr = root->valueAs<AST::UnaryExpression>()) {
    return evaluateUnaryExpr(*unexpr);
  }

  if (auto num = root->valueAs<AST::NumericValue>()) {
    ComputationResult comp(num->value);

    if (!num->unit.empty())
      comp.unit = findUnitByName(num->unit);

    return comp;
  }

  if (auto fn = root->valueAs<AST::FunctionCall>()) {
    return evaluateFunctionCall(*fn);
  }

  if (auto unit = root->valueAs<AST::UnitLiteral>()) {
    ComputationResult result(1);

    result.unit = unit->unit;

    return result;
  }

  return 0;
}

// clang-format off
static std::vector<std::pair<std::string_view, std::string_view>>
    subsitutionMap{
        {"plus", "+"},
        {"minus", "-"},
        {"divided by", "/"},
		{"div by", "/"},
		{"div", "/"},
        {"multiplied by", "*"},
        {"mul by", "*"},
        {"mul", "*"},
        {"times", "*"},
        {"power", "^"},
        {"to the power of", "^"},
    };
// clang-format on 
	
std::string Parser::preprocess(std::string_view view) {
  std::string s;
  size_t i = 0;
  unsigned char prev = 0;

  while (i < view.size()) {
    bool replaced = false;
    std::string_view sub{view.begin() + i, view.end()};

    if (!prev || std::isblank(prev)) {
      for (const auto& [k, v] : subsitutionMap) {
		 if (sub.size() < k.size()) continue ;

		 bool isNextOk = sub.size() == k.size() || std::isblank(sub.at(k.size()));

		 if (!isNextOk) continue ;

        if (strncasecmp(k.data(), sub.data(), k.size()) == 0) {
          s += v;
          i += k.size();
          replaced = true;
		  break ;
        }
      }
    }

    if (!replaced) {
      s += view.at(i);
      prev = view.at(i);
      i++;
    }
  }

  return s;
}

double Parser::evaluate(std::string_view expression) {
  std::string preprocessed = preprocess(expression);
  Tokenizer tokenizer(preprocessed);
  std::cout << "prepro=" << preprocessed << std::endl;
  AST ast(tokenizer);

  AST::Node *root = ast.parse();
  ast.print();

  auto node = evaluateNode(root);

  std::cout << "result=" << node.value;

  if (node.unit) {
    std::cout << " (" << node.unit->displayName << ")";
  }

  std::cout << std::endl;

  return 0;
}

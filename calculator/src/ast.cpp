#include "calculator.hpp"
#include <iostream>
#include <stdexcept>

static std::string padding(size_t n) {
  size_t indent_size = 2;
  std::string padding;

  for (size_t i = 0; i != n; ++i) {
    for (size_t i = 0; i != indent_size; ++i)
      padding.push_back(' ');
  }

  return padding;
}

AST::Node::Node(const NodeValue &data) : data(data) {}

AST::Node::~Node() {
  if (auto binexpr = valueAs<BinaryExpression>()) {
    delete binexpr->lhs;
    delete binexpr->rhs;
  } else if (auto unexpr = valueAs<UnaryExpression>()) {
    delete unexpr->value;
  }
}

AST::AST(Tokenizer &tk) noexcept : tk(tk), root(nullptr) {}

AST::~AST() {
  if (root)
    delete root;
}

AST::Node *AST::parse() { return root = parseExpression(); }

AST::Node *AST::parseExpression(size_t minPrecedence) {
  auto left = parseTerm();

  while (tk.peak() && tk.peak()->raw != ")") {
    auto op = tk.peak();

    auto unit = findUnitByName(op->raw);
    std::string_view finalOp = unit ? "in" : op->raw;

    if (finalOp.empty())
      throw std::runtime_error("Empty operator");

    auto precedence = precedenceTable[finalOp];

    // if the "in" operator is used explictly make it left associative
    if (finalOp == "in" && !unit) {
      std::cout << "left associative" << std::endl;
      precedence = 1;
    }

    if (precedence < minPrecedence)
      break;

    if (!unit)
      tk.consume();

    Node *right = parseExpression(precedence + 1);

    left = new Node(BinaryExpression(finalOp, left, right));
  }

  return left;
}

AST::Node *AST::parseString() {
  auto name = tk.consume();

  if (const auto unit = findUnitByName(name->raw))
    return new Node(UnitLiteral{*unit});

  if (auto format = findFormatByName(name->raw)) {
    return new Node{FormatLiteral{*format}};
  }

  if (tk.peak()->raw != "(")
    throw std::runtime_error(std::string{"Unexpected token: "} +
                             name->asString()->data());

  FunctionCall call{.name = name->raw};

  tk.consume();

  while (tk.peak() && tk.peak()->raw != ")") {
    auto arg = parseExpression();

    call.args.push_back(arg);

    if (tk.peak()->raw != ",")
      break;
    tk.consume();
  }

  if (auto tok = tk.consume(); tok && tok->raw == ")")
    return new Node{call};

  std::cerr << "expected ) at the end of function call";

  return nullptr;
}

AST::Node *AST::parseUnaryExpression() {
  auto op = tk.consume();

  return new Node{UnaryExpression{op->raw, parseTerm()}};
}

AST::Node *AST::parseTerm() {
  if (!tk.peak())
    return nullptr;

  if (tk.peak()->isNumber()) {
    double num = *tk.peak()->asNumber();

    tk.consume();

    return new Node(NumericValue{num});
  }

  if (tk.peak()->raw == "(") {
    tk.consume();
    auto expr = parseExpression();
    tk.consume();

    return expr;
  }

  if (tk.peak()->isOperator()) {
    return parseUnaryExpression();
  }

  if (tk.peak()->isString())
    return parseString();

  return nullptr;
}

bool AST::isOperator(const Token &tok) {
  if (tok.isOperator()) {
    return true;
  }

  if (auto s = tok.asString()) {
    if (*s == "in" || *s == "to" || *s == "into")
      return true;
  }

  return false;
}

void AST::printNode(Node *root, size_t depth) {
  if (!root)
    return;

  if (auto binexpr = root->valueAs<BinaryExpression>()) {
    std::cerr << padding(depth) << "operator " << binexpr->op << "(" << "\n";
    printNode(binexpr->lhs, depth + 1);
    printNode(binexpr->rhs, depth + 1);
    std::cerr << padding(depth) << ")" << "\n";
  }
  if (auto num = root->valueAs<NumericValue>()) {
    std::cerr << padding(depth) << "Number(" << num->value << ")\n";
  }
  if (auto fn = root->valueAs<FunctionCall>()) {

    std::cerr << padding(depth) << "FunctionCall " << fn->name << "(\n";
    for (size_t i = 0; i != fn->args.size(); ++i) {
      printNode(fn->args.at(i), depth + 1);
    }

    std::cerr << ")" << std::endl;
  }
  if (auto conv = root->valueAs<StringLiteral>()) {
    std::cerr << padding(depth) << "String: " << conv->raw << std::endl;
  }
  if (auto unit = root->valueAs<UnitLiteral>()) {
    std::cerr << padding(depth) << "Unit: " << unit->unit.displayName
              << std::endl;
  }
  if (auto output = root->valueAs<FormatLiteral>()) {
    std::cerr << padding(depth)
              << "Output Format: " << output->format.displayName << std::endl;
  }
  if (auto unexpr = root->valueAs<UnaryExpression>()) {
    std::cerr << padding(depth) << "unary " << unexpr->op << "(" << "\n";
    printNode(unexpr->value, depth + 1);
    std::cerr << padding(depth) << ")" << "\n";
  }
}

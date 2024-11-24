#include "calculator.hpp"
#include <iostream>

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

Result<AST::Node *, CalculatorError> AST::parse() {
  return root = RETURN_IF_ERR(parseExpression());
}

Result<AST::Node *, CalculatorError>
AST::parseExpression(size_t minPrecedence) {
  auto left = RETURN_IF_ERR(parseTerm());

  static std::string_view lastOp = "";

  while (tk.peak() && tk.peak()->type != Token::RPAREN) {
    auto op = tk.peak();

    auto unit = findUnitByName(op->raw);
    std::string_view finalOp = unit ? "as" : op->raw;

    if (!unit && op->type != Token::OPERATOR) {
      return ParseError(std::string("Expected operator but got ") +
                        std::string(op->raw));
    }

    if (finalOp.empty())
      return ParseError("Empty operator");

    auto precedence = precedenceTable[finalOp];

    if (lastOp == "in" || lastOp == "as")
      precedence = 1;

    if (precedence < minPrecedence)
      break;

    if (!unit)
      tk.consume();

    lastOp = finalOp;

    Node *right = RETURN_IF_ERR(parseExpression(precedence + 1));

    left = new Node(BinaryExpression(finalOp, left, right));
  }

  return left;
}

Result<AST::Node *, CalculatorError> AST::parseString() {
  auto name = tk.consume();

  if (const auto unit = findUnitByName(name->raw))
    return new Node(UnitLiteral{*unit});

  if (auto format = findFormatByName(name->raw)) {
    return new Node{FormatLiteral{*format}};
  }

  if (tk.peak()->type != Token::LPAREN)
    return ParseError(std::string("Unexpected token: ") +
                      std::string(name->raw));

  FunctionCall call{.name = name->raw};

  tk.consume();

  while (tk.peak() && tk.peak()->type != Token::RPAREN) {
    auto arg = RETURN_IF_ERR(parseExpression());

    call.args.push_back(arg);

    if (tk.peak()->raw != ",")
      break;
    tk.consume();
  }

  if (auto tok = tk.consume(); tok && tok->type == Token::RPAREN)
    return new Node{call};

  std::cerr << "expected ) at the end of function call";

  return nullptr;
}

Result<AST::Node *, CalculatorError> AST::parseUnaryExpression() {
  auto op = tk.consume();

  // todo: we might want to check if op is a valid unary operator

  auto term = RETURN_IF_ERR(parseTerm());

  return new Node{UnaryExpression{op->raw, term}};
}

Result<AST::Node *, CalculatorError> AST::parseTerm() {
  if (!tk.peak())
    return nullptr;

  if (tk.peak()->isNumber()) {
    double num = *tk.peak()->asNumber();

    tk.consume();

    return new Node(NumericValue{num});
  }

  if (tk.peak()->type == Token::LPAREN) {
    tk.consume();
    auto expr = parseExpression();
    tk.consume();

    return expr;
  }

  if (tk.peak()->type == Token::OPERATOR) {
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

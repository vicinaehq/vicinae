#pragma once
#include "../parser.hpp"

class AbstractCodeGenerator {
public:
  virtual ~AbstractCodeGenerator() = default;
  virtual std::string name() const = 0;
  virtual std::string generate(const Tree &ast) = 0;
};

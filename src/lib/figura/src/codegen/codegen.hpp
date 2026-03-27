#pragma once
#include "../parser.hpp"

class AbstractCodeGenerator {
public:
  virtual ~AbstractCodeGenerator() = default;
  virtual std::string name() const = 0;
  virtual std::string generateClient(const Tree &ast) = 0;
  virtual std::string generateServer(const Tree &ast) = 0;
};

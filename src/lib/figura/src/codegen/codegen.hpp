#pragma once
#include "../parser.hpp"
#include <filesystem>

struct CodegenOptions {
  std::filesystem::path file;
  std::optional<std::string> generationNamespace;
};

class AbstractCodeGenerator {
public:
  virtual ~AbstractCodeGenerator() = default;
  virtual std::string name() const = 0;
  virtual std::string generateClient(const Tree &ast, const CodegenOptions &opts = {}) = 0;
  virtual std::string generateServer(const Tree &ast, const CodegenOptions &opts = {}) = 0;
};

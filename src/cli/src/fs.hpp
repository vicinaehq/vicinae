#pragma once
#include "cli.hpp"

class FileSearchCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "fs"; }
  std::string description() const override { return "File search related commands"; }
  void setup(CLI::App *app) override {}

  FileSearchCommand();
};

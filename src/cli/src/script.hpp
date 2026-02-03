#pragma once
#include "cli.hpp"
#include "script-command-generator.hpp"
#include "script-command.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

class ScriptTemplateCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "template"; }
  std::string description() const override { return "Generate a new script template"; }

  void setup(CLI::App *app) override {
    app->add_option("-t,--title", m_title, "Title for the script")->required();
    app->add_option("-l,--lang", m_lang, "Language for the script")->default_val("bash");
    app->add_option("-m,--mode", m_mode, "Output mode (fullOutput, compact, inline, silent, terminal)")
        ->default_val("fullOutput");
  }

  bool run(CLI::App *app) override {
    auto lang = ScriptCommandGenerator::parseLanguage(m_lang);
    if (!lang) {
      std::ostringstream oss;
      oss << "Invalid language: " << m_lang << "\n\nSupported languages: ";
      auto langs = ScriptCommandGenerator::supportedLanguages();
      for (size_t i = 0; i < langs.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << langs[i];
      }
      throw CLI::ValidationError("--lang", oss.str());
    }

    auto mode = script_command::parseOutputMode(m_mode);
    if (!mode) {
      throw CLI::ValidationError("--mode",
                                 "Invalid output mode: " + m_mode +
                                     "\n\nSupported modes: fullOutput, compact, inline, silent, terminal");
    }

    auto script = ScriptCommandGenerator::generate(m_title, *lang, *mode);

    std::cout << script << std::endl;
    return true;
  }

private:
  std::string m_title;
  std::string m_lang;
  std::string m_mode;
};

class ScriptCheckCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "check"; }
  std::string description() const override { return "Validate a script command file"; }

  void setup(CLI::App *app) override {
    app->add_option("file", m_filePath, "Path to the script file to validate")->required();
  }

  bool run(CLI::App *app) override {
    if (!std::filesystem::exists(m_filePath)) {
      std::cerr << "Error: File not found: " << m_filePath << std::endl;
      return false;
    }

    auto result = script_command::ScriptCommand::fromFile(m_filePath);

    if (!result) {
      std::cerr << "Error: " << result.error() << std::endl;
      return false;
    }

    return true;
  }

private:
  std::string m_filePath;
};

class ScriptCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "script"; }
  std::string description() const override { return "Script command utilities"; }

  ScriptCommand() {
    registerCommand<ScriptTemplateCommand>();
    registerCommand<ScriptCheckCommand>();
  }
};

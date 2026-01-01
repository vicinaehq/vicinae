#pragma once
#include "cli.hpp"
#include "script-command-generator.hpp"
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
    app->add_option("-m,--mode", m_mode, "Output mode (full, compact, inline, silent, terminal)")
        ->default_val("full");
  }

  void run(CLI::App *app) override {
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

    auto mode = parseOutputMode(m_mode);
    if (!mode) {
      throw CLI::ValidationError("--mode",
                                 "Invalid output mode: " + m_mode +
                                     "\n\nSupported modes: full, compact, inline, silent, terminal");
    }

    auto script = ScriptCommandGenerator::generate(m_title, *lang, *mode);

    std::cout << script << std::endl;
  }

private:
  std::string m_title;
  std::string m_lang;
  std::string m_mode;

  std::optional<script_command::OutputMode> parseOutputMode(const std::string &mode) {
    static const std::unordered_map<std::string, script_command::OutputMode> modeMap = {
        {"full", script_command::OutputMode::Full},         {"compact", script_command::OutputMode::Compact},
        {"inline", script_command::OutputMode::Inline},     {"silent", script_command::OutputMode::Silent},
        {"terminal", script_command::OutputMode::Terminal},
    };

    auto it = modeMap.find(mode);
    if (it == modeMap.end()) { return std::nullopt; }
    return it->second;
  }
};

class ScriptCheckCommand : public AbstractCommandLineCommand {
public:
  std::string id() const override { return "check"; }
  std::string description() const override { return "Validate a script command file"; }

  void setup(CLI::App *app) override {
    app->add_option("file", m_filePath, "Path to the script file to validate")->required();
  }

  void run(CLI::App *app) override {
    if (!std::filesystem::exists(m_filePath)) {
      std::cerr << "Error: File not found: " << m_filePath << std::endl;
      exit(1);
    }

    auto result = script_command::ScriptCommand::fromFile(m_filePath);

    if (!result) {
      std::cerr << "Error: " << result.error() << std::endl;
      exit(1);
    }

    exit(0);
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

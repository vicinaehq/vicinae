#pragma once
#include "client.hpp"
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "table.hpp"

#ifndef OMNICAST_VERSION
#define OMNICAST_VERSION "unknown"
#endif

namespace Color {
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

// Bold/bright versions
const std::string BOLD_RED = "\033[1;31m";
const std::string BOLD_GREEN = "\033[1;32m";
const std::string BOLD_YELLOW = "\033[1;33m";

// Background colors
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
const std::string BG_YELLOW = "\033[43m";
} // namespace Color

struct Flag {
  std::string longOpt;
  char shortOpt;
  std::string description;
};

class CommandContext {
  const CommandContext *_parent = nullptr;
  std::vector<std::unique_ptr<CommandContext>> _subcommands;
  std::vector<Flag> _flags;
  std::string _name;
  std::string _description;
  std::vector<std::string> _aliases;
  std::vector<std::string> _positionals;

  std::string fullStringName() const;

public:
  void printHelp(std::ostream &os = std::cerr) const;

  virtual void execute(const std::vector<std::string> &args) const;

  void setFlag(const Flag &flag);
  void addAlias(const std::string &alias);
  const std::vector<std::string> &aliases() const;
  void setParent(const CommandContext &parent);
  void addPositional(const std::string &name);

  void start(const std::vector<std::string> &args) const;

  void registerCommand(std::unique_ptr<CommandContext> cmd);
  const std::string &name() const;
  const std::string &description() const;

public:
  CommandContext(const std::string &name, const std::string &description = "");
};

class CmdCommand : public CommandContext {
  class PushCommand : public CommandContext {
    void execute(const std::vector<std::string> &args) const override {
      std::cout << "argument: " << args.at(0);
      auto res = CommandClient::oneshot("command.push", Proto::Array{args.at(0)});
    }

  public:
    PushCommand() : CommandContext("push", "Push a command on top of the navigation stack") {
      addPositional("command_id");
    }
  };

  class PopCommand : public CommandContext {
  public:
    PopCommand() : CommandContext("pop", "Pop the command at the top of the navigation stack") {
      setFlag({
          .longOpt = "all",
          .shortOpt = 'a',
          .description = "Pop all commands, not only the topmost one",
      });
    }
  };

  class ListCommand : public CommandContext {
    void execute(const std::vector<std::string> &args) const override {
      auto result = CommandClient::oneshot("command.list");
      auto commands = result.value().asArray();
      TableFormatter table;

      table.addColumn("ID", 30, Alignment::LEFT);
      table.addColumn("Name", 30, Alignment::LEFT);

      for (const auto &cmd : commands) {
        auto dict = cmd.asDict();
        auto id = dict["id"].asString();
        auto name = dict["name"].asString();

        table.addRow({id, name});
      }

      table.render();
    }

  public:
    ListCommand() : CommandContext("list", "List available commands") {}
  };

  // void execute(const std::vector<std::string> &args) const override { std::cout << "execute"; }

public:
  CmdCommand() : CommandContext("command", "Interact with omnicast commands") {
    addAlias("cmd");
    registerCommand(std::make_unique<PushCommand>());
    registerCommand(std::make_unique<PopCommand>());
    registerCommand(std::make_unique<ListCommand>());
  }
};

class ToogleCommand : public CommandContext {
  void execute(const std::vector<std::string> &args) const override {
    auto res = CommandClient::oneshot("toggle");

    std::cout << (res ? "OK" : "KO") << std::endl;
  }

public:
  ToogleCommand() : CommandContext("toggle", "Toggle omnicast window") {}
};

class ExtensionCommand : public CommandContext {
  class DevelopCommand : public CommandContext {
    void execute(const std::vector<std::string> &args) const override {
      // load designated extension bundle in dev mode
      // auto res = CommandClient::oneshot("clipboard.store", Proto::Array{data, Proto::Dict{}});
    }

  public:
    DevelopCommand() : CommandContext("develop", "Start an extension in development mode") {
      addAlias("dev");
      addPositional("path_to_extension");
    }
  };

  class BuildCommand : public CommandContext {
    void execute(const std::vector<std::string> &args) const override {
      std::filesystem::path extensionDir(args.at(0));

      std::cout << "Building" << extensionDir;
      // TODO: call build tool
    }

  public:
    BuildCommand() : CommandContext("build", "Build an extension for production") {
      addAlias("build");
      addPositional("path_to_extension");
      setFlag({.longOpt = "output",
               .shortOpt = 'o',
               .description = "Specify an alternative build output directory"});
    }
  };

public:
  ExtensionCommand() : CommandContext("extension", "Interact with omnicast extensions") {
    addAlias("ext");
    registerCommand(std::make_unique<DevelopCommand>());
    registerCommand(std::make_unique<BuildCommand>());
  }
};

class PingCommand : public CommandContext {
  void execute(const std::vector<std::string> &args) const override {
    auto reply = CommandClient::oneshot("ping");

    std::cout << "response" << reply.value().asString();
  }

public:
  PingCommand() : CommandContext("ping", "Ping omnicast daemon") {}
};

class VersionCommand : public CommandContext {
  void execute(const std::vector<std::string> &args) const override {
    std::cout << OMNICAST_VERSION << std::endl;
  }

public:
  VersionCommand() : CommandContext("version", "Print omnicast version") { addAlias("ver"); }
};

class AppCommand : public CommandContext {
  void execute(const std::vector<std::string> &args) const override { std::cout << "appuh!" << std::endl; }

public:
  AppCommand() : CommandContext("app", "Interact with the app database") { addAlias("apps"); }
};

class Omnictl : public CommandContext {
  void execute(const std::vector<std::string> &args) const override { printHelp(); }

public:
  Omnictl() : CommandContext("omnictl") {}
};

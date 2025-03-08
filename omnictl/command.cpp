#include "command.hpp"
#include <algorithm>
#include <exception>
#include <iomanip>

std::string Command::fullStringName() const {
  std::string s = "";

  if (_parent) { s += _parent->fullStringName(); }

  if (!s.empty()) s += " ";

  return s += name();
}

void Command::registerCommand(std::unique_ptr<Command> cmd) {
  cmd->setParent(*this);
  _subcommands.push_back(std::move(cmd));
}

void Command::execute(const std::vector<std::string> &args) const { printHelp(); }

void Command::start(const std::vector<std::string> &args) const {
  auto pos = _positionals;
  auto &scmds = _subcommands;

  if (args.size() < pos.size()) {
    printHelp();
    return;
  }

  if (args.size() > pos.size()) {
    auto &name = args.at(pos.size());

    for (const auto &cmd : scmds) {
      auto &aliases = cmd->aliases();

      if (cmd->name() == name || std::find(aliases.begin(), aliases.end(), name) != aliases.end()) {
        cmd->start({args.begin() + 1, args.end()});
        return;
      }
    }

    printHelp(std::cerr);
    return;
  }

  try {
    execute(args);
  } catch (const std::exception &e) {
    std::cout << Color::RED << "Uncaught error: " << Color::WHITE << e.what() << std::endl;
    exit(1);
  }
}

void Command::setFlag(const Flag &flag) { _flags.push_back(flag); }
void Command::addAlias(const std::string &alias) { _aliases.push_back(alias); }
const std::vector<std::string> &Command::aliases() const { return _aliases; }
void Command::setParent(const Command &parent) { _parent = &parent; }
void Command::addPositional(const std::string &name) { _positionals.push_back(name); }

void Command::printHelp(std::ostream &os) const {
  os << Color::YELLOW << "USAGE:\n" << Color::WHITE;
  os << std::string(4, ' ') << fullStringName();

  for (const auto &pos : _positionals) {
    os << " <" << pos << ">";
  }

  if (!_subcommands.empty()) { os << " <command>"; }

  os << " [args] [flags]";

  os << "\n";

  if (auto desc = description(); !desc.empty()) {
    os << "\n" << Color::YELLOW << "DESCRIPTION:\n" << Color::WHITE;
    os << std::string(4, ' ') << desc << "\n";
  }

  size_t maxLength = 0;

  for (auto &cmd : _subcommands) {
    maxLength = std::max(maxLength, cmd->name().size());
  }

  for (auto &flag : _flags) {
    size_t flagLength = 0;

    if (!flag.longOpt.empty()) { flagLength += flag.longOpt.size() + 2; }
    if (flag.shortOpt) { flagLength += 2; }
    if (flag.shortOpt && !flag.longOpt.empty()) { flagLength += 1; }

    maxLength = std::max(maxLength, flagLength);
  }

  if (!_subcommands.empty()) {
    os << "\n" << Color::YELLOW << "SUBCOMMANDS:\n";
    for (const auto &subcommand : _subcommands) {
      os << std::string(4, ' ') << Color::RED << std::left << std::setw(maxLength + 5) << subcommand->name()
         << Color::WHITE << subcommand->description() << "\n";
    }
  }

  if (!_flags.empty()) {
    os << "\n" << Color::YELLOW << "OPTIONS:\n" << Color::WHITE;

    for (const auto &flag : _flags) {
      os << std::string(4, ' ') << Color::RED;

      std::string flagString;

      if (flag.shortOpt) {
        flagString = flagString + "-" + flag.shortOpt;
      } else {
        flagString += "  ";
      }

      if (!flag.longOpt.empty()) {
        if (flag.shortOpt) { flagString += ","; }
        flagString = flagString + "--" + flag.longOpt;
      }
      os << std::left << std::setw(maxLength + 5) << flagString << Color::WHITE << flag.description << "\n";
    }
  }
}

const std::string &Command::name() const { return _name; }

const std::string &Command::description() const { return _description; }

Command::Command(const std::string &name, const std::string &description)
    : _name(name), _description(description) {
  setFlag(Flag{.longOpt = "help", .shortOpt = 'c', .description = "Print this help message"});
}

#include <functional>
#include <iostream>
#include "client.hpp"
#include "proto.hpp"

struct CommandLineArgument {
  std::string name;
  std::function<void(const std::vector<std::string> &)> fn;
  std::vector<CommandLineArgument> subcommands;
};

void ping(const std::vector<std::string> &argv) {
  std::string socketPath = "/tmp/spellcastd.sock";
  CommandClient client;

  if (!client.connect(socketPath)) {
    std::cout << "Failed to connect to daemon at " << socketPath << ". Is omnicast running?";
    return;
  }

  auto reply = client.request("ping", Proto::Array{});

  if (auto err = std::get_if<CommandError>(&reply)) {
    std::cout << "daemon error" << err->message;
    return;
  }

  auto &variant = std::get<Proto::Variant>(reply);

  std::cout << "answer " << variant.asString("NOUPE!");
}

void open1(const std::vector<std::string> &argv) { std::cout << "open root" << std::endl; }
void openCommand(const std::vector<std::string> &argv) {
  std::cout << "open command" << argv.at(0) << std::endl;
}

// clang-format off
std::vector<CommandLineArgument> args = {
    CommandLineArgument{.name = "ping", .fn = &ping},
    CommandLineArgument{
		.name = "open", 
		.fn = &open1,
		.subcommands = {
			CommandLineArgument{
				.name = "<command-id>",
				.fn = &openCommand
			}
		}
	},
    CommandLineArgument{.name = "commands"},
    CommandLineArgument{.name = "clipboard"},
};
// clang-format on

int main(int argc, char **argv) {
  std::vector<std::string> positionals;
  std::vector<std::string> flags;

  for (int i = 1; i != argc; ++i) {
    std::string arg(argv[i]);

    if (arg.starts_with("-")) {
      flags.push_back(arg);
    } else {
      positionals.push_back(arg);
    }
  }

  const std::vector<CommandLineArgument> *currentArg = &args;
  std::vector<std::string> positionalArgs;

  for (int i = 0; i < positionals.size(); ++i) {
    auto &positional = positionals.at(i);

    for (const auto &arg : *currentArg) {
      bool isArg = arg.name.starts_with("<") || arg.name.starts_with("[");

      if (arg.name == positional || isArg) {
        if (isArg) { positionalArgs.push_back(positional); }
        if (i == positionals.size() - 1 && arg.fn) {
          arg.fn(positionalArgs);
          return 0;
        }

        currentArg = &arg.subcommands;
        break;
      }
    }
  }

  std::cout << "no path";
}

#include "command.hpp"

int main(int argc, char **argv) {
  auto cli = std::make_unique<Omnictl>();

  cli->registerCommand(std::make_unique<CmdCommand>());
  cli->registerCommand(std::make_unique<ExtensionCommand>());
  cli->registerCommand(std::make_unique<ToogleCommand>());
  cli->registerCommand(std::make_unique<PingCommand>());
  cli->registerCommand(std::make_unique<VersionCommand>());
  cli->registerCommand(std::make_unique<AppCommand>());

  std::vector<std::string> args;

  for (int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  cli->start(args);
}

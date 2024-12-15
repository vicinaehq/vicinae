#include "command-database.hpp"
#include "commands/calculator-history/calculator-history.hpp"
#include "commands/create-quicklink/create-quicklink.hpp"
#include "commands/manage-quicklinks/quicklink-manager.hpp"
#include "common.hpp"
#include <memory>

CommandDatabase::CommandDatabase() {
  commands.push_back(CommandInfo(
      "Calculator history", "pcbcalculator", "Calculator", false,
      "search calculator history",
      std::make_shared<BasicCommandFactory<CalculatorHistoryCommand>>()));
  commands.push_back(CommandInfo(
      "Browse quicklinks", "link", "Quicklink", false, "browse quicklink",
      std::make_shared<BasicCommandFactory<QuickLinkManagerCommand>>()));

  commands.push_back(CommandInfo(
      "Create quicklink", "link", "Quicklink", false, "create quicklink",
      std::make_shared<BasicCommandFactory<CreateQuickLinkCommand>>()));
}

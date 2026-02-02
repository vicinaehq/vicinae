#pragma once
#include <qstring.h>

struct CommandArgument {
  enum Type { Text, Password, Dropdown };

  struct DropdownData {
    QString title;
    QString value;
  };

  QString name;
  Type type;
  QString placeholder;
  bool required;
  std::optional<std::vector<DropdownData>> data;
};

using ArgumentList = std::vector<CommandArgument>;

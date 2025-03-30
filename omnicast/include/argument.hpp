#pragma once
#include <qstring.h>

enum ArgumentType { ArgumentText, ArgumentPassword, ArgumentDropdown };

struct Argument {
  QString name;
  ArgumentType type;
  QString placeholder;
  bool required;
};

using ArgumentList = std::vector<Argument>;

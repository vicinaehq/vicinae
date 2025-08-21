#pragma once
#include <qstring.h>

namespace Contributor {
struct Contributor {
  QString login;
  QString resource;
  size_t contribs;
};

std::vector<Contributor> getList();
}; // namespace Contributor

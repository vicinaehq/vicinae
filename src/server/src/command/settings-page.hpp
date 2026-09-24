#pragma once

#include <string>
#include <QString>

struct SettingsPage {
  std::string id;
  QString title;
  QString description;
  QString component;
};

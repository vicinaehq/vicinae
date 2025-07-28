#pragma once
#include <QDir>
#include <qdir.h>
#include <qtenvironmentvariables.h>
#include <string>
#include <vector>

struct ConfigExtension {
  std::string name;
  bool enabled;
  std::vector<std::string> exec;
};

struct Config {
  static QString dirPath() {
    return QDir::cleanPath(QDir::homePath() + QDir::separator() + ".config" + QDir::separator() + "omni");
  }

  static QDir dir() { return dirPath(); }
};

#pragma once
#include <qprocess.h>
#include <ranges>
#include "script-command-file.hpp"

class ScriptProcess : public QProcess {
public:
  ScriptProcess(const ScriptCommandFile &script, const std::vector<QString> &args) {
    const auto cmdline = script.createCommandLine(args);
    QStringList argv;

    for (const auto &[idx, arg] : cmdline | std::views::enumerate) {
      if (idx == 0) {
        setProgram(arg);
      } else {
        argv << arg;
      }
    }

    const auto cwd = script.data().currentDirectoryPath.value_or(script.path().parent_path());

    setWorkingDirectory(cwd.c_str());
    setArguments(argv);
  }
};

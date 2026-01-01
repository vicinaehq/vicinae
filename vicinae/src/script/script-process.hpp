#pragma once
#include "script-command-file.hpp"

class ScriptProcess : public QProcess {
public:
  ScriptProcess(const ScriptCommandFile &script, const std::vector<QString> &args) {
    QStringList argv;

    if (!script.data().exec.empty()) {
      for (const auto &[idx, exec] : script.data().exec | std::views::enumerate) {
        if (idx == 0) {
          setProgram(exec.c_str());
        } else {
          argv << exec.c_str();
        }
      }
      argv << script.path().c_str();
    } else {
      setProgram(script.path().c_str());
    }

    for (const auto &[arg, value] : std::views::zip(script.data().arguments, args)) {
      if (arg.percentEncoded) {
        argv << QUrl::toPercentEncoding(value);
      } else {
        argv << value;
      }
    }

    const auto cwd = script.data().currentDirectoryPath.value_or(script.path().parent_path());

    setWorkingDirectory(cwd.c_str());
    setArguments(argv);
  }
};

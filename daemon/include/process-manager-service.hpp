#pragma once
#include <qdiriterator.h>
#include <qfiledevice.h>
#include <unistd.h>

struct ProcessInfo {
  pid_t pid;
  QString comm;
};

class ProcessManagerService {
  QDir procDir;

public:
  QList<ProcessInfo> list() {
    QList<ProcessInfo> procList;

    for (const auto &name : procDir.entryList()) {
      bool isNum = false;
      auto pid = name.toInt(&isNum);

      if (!isNum)
        continue;

      ProcessInfo info{.pid = pid};
      auto processPath = procDir.absoluteFilePath(name);
      {
        QFile commFile(processPath + QDir::separator() + "comm");

        if (!commFile.open(QIODevice::ReadOnly | QIODevice::Text))
          continue;

        info.comm = commFile.readAll().trimmed();
      }

      procList << info;
    }

    return procList;
  }

  ProcessManagerService() : procDir("/proc") {}
};

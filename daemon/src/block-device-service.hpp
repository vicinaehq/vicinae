#pragma once
#include <QStorageInfo>
#include <optional>
#include <qdir.h>

struct SystemMountpoint {
  QString devicePath;
  QString mountpointPath;
  QString type;
};

struct DeviceMetadata {
  bool rotational;
};

struct DeviceData {
  QString mountpoint;
  QString name;
  DeviceMetadata metadata;
};

class BlockDeviceService {

  QList<SystemMountpoint> listDeviceMountpoints() {
    QFileInfo mountInfo("/proc/mounts");
    QFile file(mountInfo.canonicalFilePath());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug() << "failed to open mountpoints";
      return {};
    }

    qDebug() << "opened" << mountInfo.canonicalFilePath();

    QList<SystemMountpoint> mountpoints;
    QTextStream in(&file);

    for (const auto &line : in.readAll().split('\n')) {
      auto parts = line.split(' ');

      if (parts.size() != 6)
        continue;

      if (!parts[0].startsWith("/dev/"))
        continue;

      QString devicePath = parts[0];
      QFileInfo info(devicePath);

      if (info.isSymLink()) {
        devicePath = info.canonicalFilePath();
      }

      getDeviceInfo(devicePath);

      mountpoints << SystemMountpoint{.devicePath = devicePath,
                                      .mountpointPath = parts[1],
                                      .type = parts[2]};
    }

    return mountpoints;
  }

  DeviceMetadata getDeviceInfo(const QString &devicePath) {
    QFileInfo fileInfo(devicePath);

    QString resolvedBlockDir =
        QString("/sys/class/block") + QDir::separator() + fileInfo.fileName();
    QFileInfo blockDirInfo(resolvedBlockDir);

    if (blockDirInfo.isSymLink())
      resolvedBlockDir = blockDirInfo.canonicalFilePath();

    QDir blockDir(resolvedBlockDir);

    if (!blockDir.exists()) {
      qDebug() << "no such blockdir" << blockDir.absolutePath();
      return {};
    }

    auto partitionFile = QFile(blockDir.absoluteFilePath("partition"));

    if (partitionFile.exists()) {
      blockDir.cdUp();
    }

    DeviceMetadata info;
    QDir queue(blockDir.absoluteFilePath("queue"));

    if (queue.exists()) {
      if (auto file = QFile(queue.absoluteFilePath("rotational"));
          file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        info.rotational = file.readAll().toInt() == 1;
        qDebug() << "rotational" << info.rotational;
      }
    }

    return info;
  }

public:
  BlockDeviceService() {}

  QList<SystemMountpoint> deviceMountpoints() {
    return listDeviceMountpoints();
  }

  std::optional<DeviceData> getDeviceFromPath(const QString &path) {
    auto mounts = listDeviceMountpoints();
    std::optional<SystemMountpoint> bestMatch;

    for (const auto &mount : mounts) {
      if (!path.startsWith(mount.mountpointPath))
        continue;
      if (!bestMatch ||
          bestMatch->mountpointPath.size() < mount.mountpointPath.size())
        bestMatch = mount;
    }

    if (!bestMatch)
      return std::nullopt;

    auto metadata = getDeviceInfo(bestMatch->devicePath);

    return DeviceData{.mountpoint = bestMatch->mountpointPath,
                      .name = bestMatch->devicePath,
                      .metadata = metadata};
  }
};

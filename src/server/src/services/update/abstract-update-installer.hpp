#pragma once
#include <QObject>
#include <QString>
#include <filesystem>

class AbstractUpdateInstaller : public QObject {
  Q_OBJECT

signals:
  void stageChanged(const QString &stage);
  void finished();
  void failed(const QString &error);

public:
  using QObject::QObject;

  virtual bool supported() const = 0;
  virtual QString assetName() const = 0;
  virtual void install(const std::filesystem::path &archive, const QString &expectedVersion) = 0;
  virtual void relaunch() = 0;
};

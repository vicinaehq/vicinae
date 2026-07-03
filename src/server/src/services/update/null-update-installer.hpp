#pragma once
#include "abstract-update-installer.hpp"

class NullUpdateInstaller : public AbstractUpdateInstaller {
public:
  using AbstractUpdateInstaller::AbstractUpdateInstaller;

  bool supported() const override { return false; }
  QString assetName() const override { return {}; }

  void install(const std::filesystem::path &, const QString &) override {
    emit failed(QStringLiteral("Self update is not supported on this platform"));
  }

  void relaunch() override {}
};

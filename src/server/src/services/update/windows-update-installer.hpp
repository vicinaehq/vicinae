#pragma once
#include "abstract-update-installer.hpp"
#include <QCoreApplication>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class WindowsUpdateInstaller : public AbstractUpdateInstaller {
  Q_DECLARE_TR_FUNCTIONS(WindowsUpdateInstaller)

public:
  struct Signer {
    std::wstring subject;
    std::vector<unsigned char> thumbprint;
  };

  WindowsUpdateInstaller();

  bool supported() const override { return m_self.has_value(); }
  QString assetName() const override;
  void install(const std::filesystem::path &archive, const QString &expectedVersion) override;
  void relaunch() override;

private:
  void performInstall(const std::filesystem::path &archive, const QString &expectedVersion);
  std::optional<QString> verifySignature(const std::filesystem::path &setup) const;

  std::optional<Signer> m_self;
};

#pragma once
#include "abstract-update-installer.hpp"
#include <optional>
#include <string>
#include <system_error>

class MacosUpdateInstaller : public AbstractUpdateInstaller {
public:
  MacosUpdateInstaller();

  static bool isHomebrewInstall() {
    std::error_code ec;

    for (const char *caskroom : {"/opt/homebrew/Caskroom/vicinae", "/usr/local/Caskroom/vicinae"}) {
      if (std::filesystem::exists(caskroom, ec)) return true;
    }

    return false;
  }

  bool supported() const override { return true; }
  bool canSelfInstall() const override { return m_self.has_value(); }
  QString assetName() const override { return QStringLiteral("Vicinae.dmg"); }
  void install(const std::filesystem::path &archive, const QString &expectedVersion) override;
  void relaunch() override;

private:
  struct SelfInfo {
    std::filesystem::path bundlePath;
    std::string teamId;
  };

  void performInstall(const std::filesystem::path &archive, const QString &expectedVersion);
  std::optional<QString> swapBundle(const std::filesystem::path &stagedApp);
  static void detach(const std::filesystem::path &mountPoint);

  std::optional<SelfInfo> m_self;
};

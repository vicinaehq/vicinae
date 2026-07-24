#pragma once
#include "abstract-update-installer.hpp"
#include "common/types.hpp"
#include "github-release.hpp"
#include "http-client.hpp"
#include "semver.hpp"
#include "ui/action-pannel/action.hpp"
#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

class ToastService;

class UpdateService : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void updateChanged();

public:
  enum class Status { Idle, Checking, UpdateAvailable, Downloading, Installing, Installed, Failed };

  struct AvailableUpdate {
    QString tag;
    QString version;
    QString releaseUrl;
    QString assetUrl;
  };

  UpdateService(ToastService &toast, std::unique_ptr<AbstractUpdateInstaller> installer);

  void checkNow();

  const std::optional<AvailableUpdate> &available() const { return m_available; }
  Status status() const { return m_status; }
  QString currentVersionTag() const;
  bool checksSupported() const { return m_currentVersion.has_value() && m_installer->supported(); }

  void downloadAndInstall();
  void skipAvailableVersion();

private:
  void relaunch();
  struct State {
    std::optional<std::string> skippedVersion;
  };

  void performCheck();
  void handleRelease(const github::Release &release);
  void setStatus(Status status);
  void failInstall(const QString &error);
  void loadState();
  void saveState();

  ToastService &m_toast;
  std::unique_ptr<AbstractUpdateInstaller> m_installer;
  http::Client m_client;
  QTimer m_timer;
  State m_state;
  std::filesystem::path m_statePath;
  std::string m_buf;
  std::optional<vicinae::Semver> m_currentVersion;
  std::optional<AvailableUpdate> m_available;
  Status m_status = Status::Idle;
};

class InstallUpdateAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(InstallUpdateAction)

public:
  InstallUpdateAction();

  void execute(ApplicationContext *ctx) override;
};

class SkipUpdateVersionAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(SkipUpdateVersionAction)

public:
  SkipUpdateVersionAction();

  void execute(ApplicationContext *ctx) override;
};

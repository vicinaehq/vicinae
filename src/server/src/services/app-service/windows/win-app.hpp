#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <filesystem>

// A launchable Windows app, from either the Win32 (shortcut) or packaged (UWP) track.
class WindowsApplication : public AbstractApplication {
public:
  struct Data {
    QString id;
    QString displayName;
    QString program;              // Win32: target exe. Packaged: AppUserModelID.
    std::filesystem::path path;   // Win32: the .lnk. Packaged: install location.
    QString shellParsingName;     // Win32: the .lnk. Packaged: shell:AppsFolder\<AUMID>.
    bool packaged = false;
    bool shellOpen = false;       // opener that defers to the shell's default handler for the target
    bool game = false;
  };

  explicit WindowsApplication(Data data) : m_data(std::move(data)) {}

  QString id() const override { return m_data.id; }
  QString displayName() const override { return m_data.displayName; }
  bool displayable() const override { return true; }
  bool isTerminalApp() const override { return false; }
  QString program() const override { return m_data.program; }
  std::filesystem::path path() const override { return m_data.path; }
  QString description() const override { return {}; }
  QString category() const override { return m_data.game ? QStringLiteral("Game") : QString(); }

  ImageURL iconUrl() const override { return ImageURL::winShellIcon(m_data.shellParsingName); }

  bool isPackaged() const { return m_data.packaged; }
  bool opensViaShell() const { return m_data.shellOpen; }
  const QString &shellParsingName() const { return m_data.shellParsingName; }

private:
  Data m_data;
};

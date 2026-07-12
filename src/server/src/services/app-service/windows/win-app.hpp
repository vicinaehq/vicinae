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
    QString arguments;
    QString workingDirectory;
    std::filesystem::path path;   // Win32: the .lnk. Packaged: install location.
    QString shellParsingName;     // Win32: the .lnk. Packaged: shell:AppsFolder\<AUMID>.
    QString openerExtension;      // set on openers from the assoc-handler enumeration
    QString category;             // "Game", "Link", or empty for a regular application
    bool packaged = false;
    bool shellOpen = false;       // opener that defers to the shell's default handler for the target
  };

  explicit WindowsApplication(Data data) : m_data(std::move(data)) {}

  QString id() const override { return m_data.id; }
  QString displayName() const override { return m_data.displayName; }
  bool displayable() const override { return true; }
  bool isTerminalApp() const override { return false; }
  QString program() const override { return m_data.program; }
  std::filesystem::path path() const override { return m_data.path; }
  QString description() const override { return {}; }
  QString category() const override { return m_data.category; }

  ImageURL iconUrl() const override { return ImageURL::winShellIcon(m_data.shellParsingName); }

  bool isPackaged() const { return m_data.packaged; }
  bool opensViaShell() const { return m_data.shellOpen; }
  const QString &shellParsingName() const { return m_data.shellParsingName; }
  const QString &arguments() const { return m_data.arguments; }
  const QString &workingDirectory() const { return m_data.workingDirectory; }
  const QString &openerExtension() const { return m_data.openerExtension; }

private:
  Data m_data;
};

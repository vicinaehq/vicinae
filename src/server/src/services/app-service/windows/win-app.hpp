#pragma once
#include "common/types.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/image/url.hpp"
#include <QString>
#include <filesystem>
#include <variant>

struct Win32ShortcutApp {
  std::filesystem::path shortcut; // .lnk or .appref-ms
  QString program;                // target exe; empty when only a shell PIDL is stored
  QString arguments;
  QString workingDirectory;
  QString aumid; // from the .lnk property store, when set
};

struct UrlShortcutApp {
  std::filesystem::path shortcut; // the .url file
  QString url;
};

struct Win32ExeApp {
  std::filesystem::path exe;
  QString openerExtension; // assoc string to re-resolve the handler through, when known
};

struct PackagedApp {
  QString aumid;
  std::filesystem::path installLocation;
};

// defers to the shell's default handler for the target
struct ShellOpenApp {
  QString target;
};

using WindowsAppKind = std::variant<Win32ShortcutApp, UrlShortcutApp, Win32ExeApp, PackagedApp, ShellOpenApp>;

class WindowsApplication : public AbstractApplication {
public:
  struct Data {
    QString id;
    QString displayName;
    QString category; // "Game", "Link", or empty
    WindowsAppKind kind;
  };

  explicit WindowsApplication(Data data) : m_data(std::move(data)) {}

  QString id() const override { return m_data.id; }
  QString displayName() const override { return m_data.displayName; }
  bool displayable() const override { return true; }
  bool isTerminalApp() const override { return false; }
  QString description() const override { return {}; }
  QString category() const override { return m_data.category; }

  QString program() const override {
    return match(
        m_data.kind, [](const Win32ShortcutApp &s) { return s.program; },
        [](const UrlShortcutApp &u) { return u.url; },
        [](const Win32ExeApp &e) { return QString::fromStdWString(e.exe.wstring()); },
        [](const PackagedApp &p) { return p.aumid; }, [](const ShellOpenApp &) { return QString(); });
  }

  std::filesystem::path path() const override {
    return match(
        m_data.kind, [](const Win32ShortcutApp &s) { return s.shortcut; },
        [](const UrlShortcutApp &u) { return u.shortcut; }, [](const Win32ExeApp &e) { return e.exe; },
        [](const PackagedApp &p) { return p.installLocation; },
        [](const ShellOpenApp &) { return std::filesystem::path(); });
  }

  QString shellParsingName() const {
    return match(
        m_data.kind, [](const Win32ShortcutApp &s) { return QString::fromStdWString(s.shortcut.wstring()); },
        [](const UrlShortcutApp &u) { return QString::fromStdWString(u.shortcut.wstring()); },
        [](const Win32ExeApp &e) { return QString::fromStdWString(e.exe.wstring()); },
        [](const PackagedApp &p) { return QStringLiteral("shell:AppsFolder\\") + p.aumid; },
        [](const ShellOpenApp &s) { return s.target; });
  }

  ImageURL iconUrl() const override { return ImageURL::winShellIcon(shellParsingName()); }

  const WindowsAppKind &kind() const { return m_data.kind; }
  bool isPackaged() const { return std::holds_alternative<PackagedApp>(m_data.kind); }
  bool opensViaShell() const { return std::holds_alternative<ShellOpenApp>(m_data.kind); }

private:
  Data m_data;
};

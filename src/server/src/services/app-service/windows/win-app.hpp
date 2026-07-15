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
    QString fullyQualifiedName;
    QString category; // "Game", "Link", or empty
    bool elevated = false;
    WindowsAppKind kind;
  };

  explicit WindowsApplication(Data data) : m_data(std::move(data)) {}

  QString id() const override { return m_data.id; }
  QString displayName() const override { return m_data.displayName; }
  QString fullyQualifiedName() const override {
    return m_data.fullyQualifiedName.isEmpty() ? m_data.displayName : m_data.fullyQualifiedName;
  }
  bool displayable() const override { return true; }
  bool isTerminalApp() const override { return false; }
  bool isAction() const override { return m_data.elevated; }
  QString description() const override { return {}; }
  QString category() const override { return m_data.category; }

  std::vector<std::shared_ptr<AbstractApplication>> actions() const override {
    if (m_data.elevated || !supportsElevation()) return {};
    Data data = m_data;
    data.id += QStringLiteral(":runas");
    data.fullyQualifiedName = m_data.displayName + QStringLiteral(": Run as Administrator");
    data.displayName = QStringLiteral("Run as Administrator");
    data.elevated = true;
    return {std::make_shared<WindowsApplication>(std::move(data))};
  }

  // matches the keys produced by the window manager: full exe paths and AUMIDs
  bool matchesWindowClass(const QString &target) const override {
    if (target.isEmpty()) return false;
    auto equals = [&](const QString &value) {
      return !value.isEmpty() && value.compare(target, Qt::CaseInsensitive) == 0;
    };
    return match(
        m_data.kind, [&](const Win32ShortcutApp &s) { return equals(s.aumid) || equals(s.program); },
        [&](const Win32ExeApp &e) { return equals(QString::fromStdWString(e.exe.wstring())); },
        [&](const PackagedApp &p) { return equals(p.aumid); }, [](const auto &) { return false; });
  }

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

  ImageURL iconUrl() const override {
    constexpr int SIID_SHIELD_ID = 77; // SIID_SHIELD
    if (m_data.elevated) return ImageURL::winStockIcon(SIID_SHIELD_ID);
    return ImageURL::winShellIcon(shellParsingName());
  }

  const WindowsAppKind &kind() const { return m_data.kind; }
  bool isPackaged() const { return std::holds_alternative<PackagedApp>(m_data.kind); }
  bool opensViaShell() const { return std::holds_alternative<ShellOpenApp>(m_data.kind); }
  bool elevated() const { return m_data.elevated; }

private:
  bool supportsElevation() const {
    return match(
        m_data.kind, [](const Win32ShortcutApp &) { return true; },
        [](const UrlShortcutApp &) { return false; }, [](const Win32ExeApp &) { return true; },
        [](const PackagedApp &) { return true; }, [](const ShellOpenApp &) { return false; });
  }

  Data m_data;
};

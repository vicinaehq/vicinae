#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include <QString>
#include <filesystem>
#include <memory>

class MacApplication : public AbstractApplication {
public:
  static std::shared_ptr<MacApplication> fromBundle(const std::filesystem::path &bundlePath);

  QString id() const override { return m_id; }
  QString displayName() const override { return m_displayName; }
  bool displayable() const override { return m_displayable; }
  bool isTerminalApp() const override { return false; }
  bool isTerminalEmulator() const override;
  ImageURL iconUrl() const override;
  std::filesystem::path path() const override { return m_bundlePath; }
  QString program() const override { return m_executable; }
  std::optional<QString> windowClass() const override { return m_id; }
  bool matchesWindowClass(const QString &wmClass) const override;
  QString description() const override { return {}; }

  MacApplication(std::filesystem::path bundlePath, QString id, QString displayName, QString executable,
                 bool displayable);

private:
  std::filesystem::path m_bundlePath;
  QString m_id;
  QString m_displayName;
  QString m_executable;
  bool m_displayable;
};

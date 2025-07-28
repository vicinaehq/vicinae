#include "vicinae.hpp"

std::filesystem::path Omnicast::runtimeDir() {
  std::filesystem::path osRundir(
      QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation).toStdString());

  return osRundir / "vicinae";
}

std::filesystem::path Omnicast::dataDir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation).toStdString();
}

std::filesystem::path Omnicast::configDir() {
  return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString();
}

std::filesystem::path Omnicast::commandSocketPath() { return runtimeDir() / "vicinae.sock"; }
std::filesystem::path Omnicast::pidFile() { return runtimeDir() / "vicinae.pid"; }

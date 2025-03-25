#include "omnicast.hpp"

std::filesystem::path Omnicast::runtimeDir() {
  std::filesystem::path osRundir(
      QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation).toStdString());

  return osRundir / "omnicast";
}

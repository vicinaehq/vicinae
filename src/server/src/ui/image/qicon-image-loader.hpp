#pragma once
#include "ui/image/image.hpp"
#include <QFile>

class QIconImageLoader : public AbstractImageLoader {
  QString m_icon;
  std::optional<QString> m_theme;

private:
  QIcon loadIconFromFileSystem(const QString &iconName);

public:
  void render(const RenderConfig &config) override;

  QIconImageLoader(const QString &name, const std::optional<QString> &themeName = "");
};

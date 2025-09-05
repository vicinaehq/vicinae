#pragma once
#include "ui/image/image.hpp"
#include <QFile>
#include <qfuturewatcher.h>

class QIconImageLoader : public AbstractImageLoader {
public:
  void render(const RenderConfig &config) override;

  QIconImageLoader(const QString &name, const std::optional<QString> &themeName = "");

private:
  void iconLoaded(const QIcon &icon);

  QString m_icon;
  std::optional<QString> m_theme;
  QFutureWatcher<QIcon> m_watcher;
  RenderConfig m_config;
};

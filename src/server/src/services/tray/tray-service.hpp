#pragma once
#include <QObject>
#include <QString>
#include <memory>

class TrayService : public QObject {
  Q_OBJECT

public:
  enum class Link { Sponsor, Discord, Follow };
  Q_ENUM(Link)

signals:
  void toggleRequested();
  void openSettingsRequested(const QString &tab);
  void checkForUpdatesRequested();
  void openLinkRequested(Link link);
  void quitRequested();

public:
  explicit TrayService(QObject *parent = nullptr) : QObject(parent) {}
  ~TrayService() override = default;

  virtual void setVersion(const QString &version) = 0;
  virtual void setCheckForUpdatesVisible(bool visible) = 0;
  virtual void setAvailableUpdate(const QString &tag) = 0;
  virtual void show() = 0;
  virtual void hide() = 0;
};

std::unique_ptr<TrayService> createTrayService();

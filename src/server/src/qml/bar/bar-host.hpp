#pragma once
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <memory>
#include <optional>
#include <vector>
#include "bar/bar-tray-model.hpp"
#include "common/context.hpp"
#include "image-url.hpp"
#include "services/media-control/abstract-media-control.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

struct BarWorkspace {
  AbstractWindowManager::WorkspacePtr workspace;
  bool active = false;
  std::size_t windowCount = 0;
  std::vector<ImageUrl> icons;
};

/**
 * Data shared by every bar window: clock, focused window, media, audio, power, tray and the
 * workspace snapshot per-screen models filter from.
 */
class BarHost : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString time READ time NOTIFY clockChanged)
  Q_PROPERTY(QString date READ date NOTIFY clockChanged)
  Q_PROPERTY(QString longDate READ longDate NOTIFY clockChanged)
  Q_PROPERTY(bool hasFocusedWindow READ hasFocusedWindow NOTIFY focusedWindowChanged)
  Q_PROPERTY(QString focusedApp READ focusedApp NOTIFY focusedWindowChanged)
  Q_PROPERTY(QString focusedTitle READ focusedTitle NOTIFY focusedWindowChanged)
  Q_PROPERTY(ImageUrl focusedIcon READ focusedIcon NOTIFY focusedWindowChanged)
  Q_PROPERTY(bool mediaVisible READ mediaVisible NOTIFY mediaChanged)
  Q_PROPERTY(bool mediaPlaying READ mediaPlaying NOTIFY mediaChanged)
  Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaChanged)
  Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY mediaChanged)
  Q_PROPERTY(QString mediaIdentity READ mediaIdentity NOTIFY mediaChanged)
  Q_PROPERTY(bool mediaCanGoNext READ mediaCanGoNext NOTIFY mediaChanged)
  Q_PROPERTY(bool mediaCanGoPrevious READ mediaCanGoPrevious NOTIFY mediaChanged)
  Q_PROPERTY(int volume READ volume NOTIFY audioChanged)
  Q_PROPERTY(bool muted READ muted NOTIFY audioChanged)
  Q_PROPERTY(QVariantList powerActions READ powerActions CONSTANT)
  Q_PROPERTY(bool hasUnreadNews READ hasUnreadNews NOTIFY newsChanged)
  Q_PROPERTY(BarTrayModel *tray READ tray CONSTANT)

signals:
  void clockChanged();
  void focusedWindowChanged();
  void mediaChanged();
  void audioChanged();
  void newsChanged();
  void workspacesChanged();

public:
  explicit BarHost(ApplicationContext &ctx, QObject *parent = nullptr);

  QString time() const { return m_time; }
  QString date() const { return m_date; }
  QString longDate() const { return m_longDate; }

  bool hasFocusedWindow() const { return m_focused.valid; }
  QString focusedApp() const { return m_focused.app; }
  QString focusedTitle() const { return m_focused.title; }
  ImageUrl focusedIcon() const { return m_focused.icon; }

  bool mediaVisible() const { return m_media.has_value(); }
  bool mediaPlaying() const;
  QString mediaTitle() const { return m_media ? m_media->title : QString(); }
  QString mediaArtist() const { return m_media ? m_media->artist : QString(); }
  QString mediaIdentity() const { return m_media ? m_media->identity : QString(); }
  bool mediaCanGoNext() const { return m_media && m_media->canGoNext; }
  bool mediaCanGoPrevious() const { return m_media && m_media->canGoPrevious; }

  int volume() const { return m_volume; }
  bool muted() const { return m_muted; }

  QVariantList powerActions() const { return m_powerActions; }
  bool hasUnreadNews() const;
  BarTrayModel *tray() { return &m_tray; }

  const std::vector<BarWorkspace> &workspaces() const { return m_workspaces; }

  Q_INVOKABLE void toggleLauncher();
  Q_INVOKABLE void openSettings();
  Q_INVOKABLE void focusWorkspace(const QString &id);
  Q_INVOKABLE void mediaPlayPause();
  Q_INVOKABLE void mediaNext();
  Q_INVOKABLE void mediaPrevious();
  Q_INVOKABLE void adjustVolume(int delta);
  Q_INVOKABLE void toggleMute();
  Q_INVOKABLE void refreshAudio();
  Q_INVOKABLE void powerAction(const QString &id);

private:
  struct FocusedWindow {
    bool valid = false;
    QString app;
    QString title;
    ImageUrl icon;
  };

  void tickClock();
  void refreshFocusedWindow();
  void refreshWorkspaces();
  void refreshMedia();
  void buildPowerActions();

  ApplicationContext &m_ctx;
  QTimer m_clockTimer;
  QTimer m_workspaceTimer;
  QTimer m_focusTimer;
  QTimer m_audioTimer;
  QString m_time;
  QString m_date;
  QString m_longDate;
  FocusedWindow m_focused;
  std::optional<MediaPlayer> m_media;
  int m_volume = 0;
  bool m_muted = false;
  QVariantList m_powerActions;
  std::vector<BarWorkspace> m_workspaces;
  BarTrayModel m_tray;
};

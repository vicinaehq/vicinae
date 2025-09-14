#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/hyprland/hypr-listener.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <lib/xkbcommon-utils.hpp>
#include <qapplication.h>
#include <qfuture.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qkeysequence.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <QJsonArray>
#include <QJsonObject>
#include <qpromise.h>
#include <qstringview.h>
#include <qtcoreexports.h>

class HyprlandWindow : public AbstractWindowManager::AbstractWindow {
  QString m_id;
  QString m_title;
  QString m_wmClass;
  int m_workspaceId;
  int m_pid;
  AbstractWindowManager::WindowBounds m_bounds;

public:
  QString id() const override { return m_id; }
  std::optional<int> pid() const override { return m_pid; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }
  std::optional<AbstractWindowManager::WindowBounds> bounds() const override { return m_bounds; }
  std::optional<QString> workspace() const override { return QString::number(m_workspaceId); }
  bool canClose() const override { return true; }

  HyprlandWindow(const QJsonObject &json);
};

class HyprlandWindowManager : public AbstractWindowManager {
public:
  HyprlandWindowManager();

  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;

  WorkspacePtr getActiveWorkspace() const override;

  bool supportsInputForwarding() const override;
  bool sendShortcutSync(const AbstractWindow &window, const KeyboardShortcut &shortcut) override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool isActivatable() const override;
  bool hasWorkspaces() const override;
  WorkspaceList listWorkspaces() const override;

  bool ping() const override;
  void start() override;

  ~HyprlandWindowManager() override = default;

private:
  Hyprland::EventListener m_ev;

  QString stringifyModifiers(QFlags<Qt::KeyboardModifier> mods);
  QString stringifyKey(Qt::Key key) const;
  QString id() const override;
  QString displayName() const override;
};

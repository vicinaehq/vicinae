#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
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
  int m_pid;

public:
  QString id() const override { return m_id; }
  std::optional<int> pid() const override { return m_pid; }
  QString title() const override { return m_title; }
  QString wmClass() const override { return m_wmClass; }

  // Extended AbstractWindow interface - use defaults for now
  std::optional<int> workspace() const override { return std::nullopt; }
  bool canClose() const override { return true; }

  HyprlandWindow(const QJsonObject &json);
};

class HyprlandWindowManager : public AbstractWindowManager {
  QString stringifyModifiers(QFlags<Qt::KeyboardModifier> mods);

  QString stringifyKey(Qt::Key key) const;
  QString id() const override;
  QString displayName() const override;

public:
  WindowList listWindowsSync() const override;
  AbstractWindowManager::WindowPtr getFocusedWindowSync() const override;
  bool supportsInputForwarding() const override;
  bool sendShortcutSync(const AbstractWindow &window, const KeyboardShortcut &shortcut) override;
  void focusWindowSync(const AbstractWindow &window) const override;
  bool closeWindow(const AbstractWindow &window) const override;
  bool isActivatable() const override;

  bool ping() const override;
  void start() const override;

  ~HyprlandWindowManager() override = default;
};

#pragma once
#include "abstract-file-chooser.hpp"
#include <qobject.h>
#include <qtmetamacros.h>

class FileChooser;

class FileChooserService : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)

signals:
  void dialogOpened();
  void dialogClosed();
  void activeChanged();
  void filesSelected(const QStringList &paths);

public:
  explicit FileChooserService(QObject *parent = nullptr);

  /// Returns true if the portal handled it, false if QML fallback is needed.
  Q_INVOKABLE bool openDialog(bool canChooseFiles, bool canChooseDirectories, bool multiple);

  /// Called when the QML fallback dialog completes (accepted or rejected).
  Q_INVOKABLE void notifyFallbackDone();

  void cancel();
  bool isActive() const;

private:
  void finish(const QStringList *paths);

  FileChooser *m_activeChooser = nullptr;
  bool m_fallbackActive = false;
};

#pragma once
#include "ui/qml-engine-scope.hpp"
#include "abstract-file-chooser.hpp"
#include <QUrl>
#include <qobject.h>
#include <qtmetamacros.h>

class FileChooserService : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(FileChooser)
  QML_SINGLETON

public:
  static FileChooserService *create(QQmlEngine *, QJSEngine *);

private:
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

  /// Converts a fallback dialog URL into a native local path.
  Q_INVOKABLE QString toLocalPath(const QUrl &url) const;

  void cancel();
  bool isActive() const;

private:
  void finish(const QStringList *paths);

  AbstractFileChooser *m_activeChooser = nullptr;
  bool m_fallbackActive = false;
};

#pragma once
#include "ui/qml-engine-scope.hpp"
#include "abstract-file-chooser.hpp"
#include <QUrl>
#include <filesystem>
#include <functional>
#include <optional>
#include <qobject.h>
#include <qtmetamacros.h>
#include <string>

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
  /// resumeCaller is false once a save destination was chosen: the caller is done with the launcher.
  void dialogClosed(bool resumeCaller);
  void activeChanged();
  void filesSelected(const QStringList &paths);
  void fallbackSaveRequested(const QUrl &suggestedFile);

public:
  struct SaveOptions {
    std::string suggestedName;
    std::optional<std::filesystem::path> currentFolder;
  };

  /// Receives the chosen destination, or std::nullopt if the dialog was dismissed.
  using SaveCallback = std::function<void(std::optional<std::filesystem::path>)>;

  explicit FileChooserService(QObject *parent);

  void saveFile(const SaveOptions &options, SaveCallback callback);

  /// Returns true if the portal handled it, false if QML fallback is needed.
  Q_INVOKABLE bool openDialog(bool canChooseFiles, bool canChooseDirectories, bool multiple);

  /// Called when the QML fallback dialog completes (accepted or rejected).
  Q_INVOKABLE void notifyFallbackDone();

  Q_INVOKABLE void acceptFallbackSave(const QUrl &file);
  Q_INVOKABLE void rejectFallbackSave();

  /// Converts a fallback dialog URL into a native local path.
  Q_INVOKABLE QString toLocalPath(const QUrl &url) const;

  void cancel();
  bool isActive() const;

private:
  void finish(const QStringList *paths);

  AbstractFileChooser *m_activeChooser = nullptr;
  SaveCallback m_saveCallback;
  bool m_fallbackActive = false;
};

#pragma once
#include "abstract-file-chooser.hpp"
#include <QPointer>
#include <functional>

class FileChooser;

class FileChooserService : public QObject {
  Q_OBJECT

signals:
  void dialogOpened();
  void dialogClosed();

public:
  using ResultCallback = std::function<void(const std::vector<std::filesystem::path> &)>;

  explicit FileChooserService(QObject *parent = nullptr);

  /// Opens a file chooser dialog with the given options.
  /// \param context Safety guard — if this object is destroyed, callbacks won't fire.
  /// \param onChosen Called with selected paths on success.
  /// \returns true if the portal handled it, false if QML fallback is needed.
  bool open(const FileChooserOptions &options, QObject *context, ResultCallback onChosen);

  /// Called by the QML fallback path when the user selects files.
  void reportFallbackResult(const std::vector<std::filesystem::path> &paths);

  /// Called by the QML fallback path when the user cancels.
  void reportFallbackCancelled();

  void cancel();
  bool isActive() const;

private:
  void finish(const std::vector<std::filesystem::path> *paths);

  FileChooser *m_activeChooser = nullptr;
  QPointer<QObject> m_context;
  ResultCallback m_onChosen;
};

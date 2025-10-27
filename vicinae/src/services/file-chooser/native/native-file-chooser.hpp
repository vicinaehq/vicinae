#pragma once
#include "services/file-chooser/abstract-file-chooser.hpp"
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qfiledialog.h>

/**
 * Uses the underlying platform's file chooser or the QT one if on Linux.
 */
class NativeFileChooser : public AbstractFileChooser {
public:
  NativeFileChooser();

  bool openFile() override;
  void setMultipleSelection(bool value) override;
  void setCurrentFolder(const std::filesystem::path &path) override;
  void setMimeTypeFilters(const QStringList &filters) override;

private:
  void handleFiles(const QStringList &files);

  bool m_multiple;
  QFileDialog m_dialog;
};

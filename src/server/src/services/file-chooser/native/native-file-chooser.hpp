#pragma once
#include "services/file-chooser/abstract-file-chooser.hpp"
#include <qobject.h>

class NativeFileChooser : public AbstractFileChooser {
  Q_OBJECT

public:
  NativeFileChooser(QObject *parent = nullptr);

  bool open(const FileChooserOptions &options) override;

private:
  void handleFiles(const QStringList &files);
};

#pragma once
#include "abstract-file-chooser.hpp"
#include <qobject.h>
#include <qtmetamacros.h>

class FileChooser : public QObject {
  Q_OBJECT

signals:
  void filesChosen(const std::vector<std::filesystem::path> &paths);
  void rejected();

public:
  explicit FileChooser(QObject *parent = nullptr);

  void open(const FileChooserOptions &options);

private:
  void connectBackend(AbstractFileChooser *backend);
};

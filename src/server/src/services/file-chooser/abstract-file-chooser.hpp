#pragma once
#include <qobject.h>
#include <qstringliteral.h>
#include <filesystem>
#include <optional>
#include <qtmetamacros.h>
#include <vector>

struct FileChooserOptions {
  bool canChooseFiles = true;
  bool canChooseDirectories = false;
  bool allowMultipleSelection = false;
  bool showHiddenFiles = false;
  std::optional<std::filesystem::path> currentFolder;
};

class AbstractFileChooser : public QObject {
  Q_OBJECT

signals:
  void filesChosen(const std::vector<std::filesystem::path> &paths);
  void rejected() const;

public:
  AbstractFileChooser(QObject *parent = nullptr) : QObject(parent) {}

  virtual bool open(const FileChooserOptions &options) = 0;
};

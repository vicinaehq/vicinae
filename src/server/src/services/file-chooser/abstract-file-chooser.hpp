#pragma once
#include <qobject.h>
#include <qstringliteral.h>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <qtmetamacros.h>
#include <string>
#include <vector>

enum class FileChooserMode : std::uint8_t { Open, Save };

struct FileChooserOptions {
  FileChooserMode mode = FileChooserMode::Open;
  bool canChooseFiles = true;
  bool canChooseDirectories = false;
  bool allowMultipleSelection = false;
  bool showHiddenFiles = false;
  std::optional<std::filesystem::path> currentFolder;
  /// Save mode only: file name prefilled in the dialog.
  std::optional<std::string> suggestedName;
};

class AbstractFileChooser : public QObject {
  Q_OBJECT

signals:
  void filesChosen(const std::vector<std::filesystem::path> &paths);
  void rejected() const;

public:
  AbstractFileChooser(QObject *parent = nullptr) : QObject(parent) {}

  virtual bool isAvailable() const = 0;
  virtual bool open(const FileChooserOptions &options) = 0;

  /// Dismiss any visible dialog without emitting a result.
  virtual void close() {}
};

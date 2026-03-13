#include "native-file-chooser.hpp"
#include "theme.hpp"
#include <qfiledialog.h>
#include <qobject.h>

namespace fs = std::filesystem;

NativeFileChooser::NativeFileChooser(QObject *parent) : AbstractFileChooser(parent) {}

bool NativeFileChooser::open(const FileChooserOptions &options) {
  auto *dialog = new QFileDialog();
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->setStyleSheet(ThemeService::instance().nativeFilePickerStyleSheet());

  bool const directoryMode = options.canChooseDirectories;

  if (directoryMode) {
    dialog->setFileMode(QFileDialog::FileMode::Directory);
  } else if (options.allowMultipleSelection) {
    dialog->setFileMode(QFileDialog::ExistingFiles);
  } else {
    dialog->setFileMode(QFileDialog::ExistingFile);
  }

  connect(dialog, &QFileDialog::filesSelected, this, &NativeFileChooser::handleFiles);
  connect(dialog, &QFileDialog::rejected, this, &NativeFileChooser::rejected);

  dialog->open();
  return true;
}

void NativeFileChooser::handleFiles(const QStringList &files) {
  std::vector<fs::path> paths;
  paths.reserve(files.size());
  for (const auto &file : files) {
    paths.emplace_back(file.toStdString());
  }
  emit filesChosen(paths);
}

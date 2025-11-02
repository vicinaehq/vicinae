#include "native-file-chooser.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include "theme.hpp"
#include <qfiledialog.h>
#include <qnamespace.h>
#include <qobject.h>

namespace fs = std::filesystem;

static const int SIDE_PADDING = 20;
static constexpr const int PADDING = SIDE_PADDING * 2;

NativeFileChooser::NativeFileChooser(QObject *parent) : AbstractFileChooser(parent) {
  auto stylesheet = ThemeService::instance().nativeFilePickerStyleSheet();
  m_dialog.setStyleSheet(stylesheet);
  m_dialog.setFileMode(QFileDialog::ExistingFile);
  m_dialog.setModal(true);
  connect(&m_dialog, &QFileDialog::filesSelected, this, &NativeFileChooser::handleFiles);
  connect(&m_dialog, &QFileDialog::rejected, this, &NativeFileChooser::rejected);
}

bool NativeFileChooser::openFile() {
  m_dialog.open();
  return true;
}

void NativeFileChooser::setCurrentFolder(const std::filesystem::path &path) { m_dialog.setDirectory(path); }

void NativeFileChooser::setMimeTypeFilters(const QStringList &filters) {
  // TODO: implement proper filtering
  if (filters.contains("inode/directory")) { m_dialog.setFileMode(QFileDialog::FileMode::Directory); }
}

void NativeFileChooser::setMultipleSelection(bool value) { m_dialog.setFileMode(QFileDialog::ExistingFiles); }

void NativeFileChooser::handleFiles(const QStringList &files) {
  std::vector<fs::path> paths;
  paths.reserve(files.size());
  for (const auto &file : files) {
    paths.emplace_back(file.toStdString());
  }
  emit filesChosen(paths);
}

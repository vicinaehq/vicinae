#include "native-file-chooser.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include "theme.hpp"
#include <LayerShellQt/Shell>
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
  m_directoryMode = filters.contains("inode/directory");
  if (m_directoryMode) {
    m_dialog.setFileMode(QFileDialog::FileMode::Directory);
  } else {
    m_dialog.setFileMode(m_multiple ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
  }
}

void NativeFileChooser::setMultipleSelection(bool value) {
  m_multiple = value;
  if (m_directoryMode) {
    // Qt's QFileDialog doesn't support multiple directory selection
    // so we keep Directory mode even when multiple is requested
    m_dialog.setFileMode(QFileDialog::FileMode::Directory);
  } else {
    m_dialog.setFileMode(value ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
  }
}

void NativeFileChooser::handleFiles(const QStringList &files) {
  std::vector<fs::path> paths;
  paths.reserve(files.size());
  for (const auto &file : files) {
    paths.emplace_back(file.toStdString());
  }
  emit filesChosen(paths);
}

#include "native-file-chooser.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include "theme.hpp"
#include <LayerShellQt/Shell>
#include <qfiledialog.h>
#include <qnamespace.h>
#include "environment.hpp"
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

#ifdef WAYLAND_LAYER_SHELL
  if (Environment::isLayerShellEnabled()) {
    namespace Shell = LayerShellQt;

    m_dialog.createWinId();
    if (auto lshell = Shell::Window::get(m_dialog.windowHandle())) {
      lshell->setLayer(Shell::Window::LayerOverlay);
      lshell->setScope(Omnicast::APP_ID);
      lshell->setScreenConfiguration(Shell::Window::ScreenFromCompositor);
      lshell->setKeyboardInteractivity(Shell::Window::KeyboardInteractivityExclusive);
      lshell->setAnchors(Shell::Window::AnchorNone);
    } else {
      qWarning() << "Unable apply layer shell rules to QFileDialog window: LayerShellQt::Window::get() "
                    "returned null";
    }
  }
#endif
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

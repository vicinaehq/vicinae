#include "file-chooser.hpp"
#include <cstdlib>

FileChooser::FileChooser(QObject *parent) : QObject(parent), m_xdp(this) {
  connect(&m_xdp, &AbstractFileChooser::filesChosen, this, &FileChooser::filesChosen);
  connect(&m_xdp, &AbstractFileChooser::rejected, this, &FileChooser::rejected);
}

bool FileChooser::isAvailable() const {
  if (std::getenv("VICINAE_FORCE_QT_DIALOG") != nullptr) return false;
  return m_xdp.isAvailable();
}

bool FileChooser::open(const FileChooserOptions &options) { return m_xdp.open(options); }

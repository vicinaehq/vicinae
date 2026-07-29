#include "file-chooser.hpp"
#include <cstdlib>

#ifdef Q_OS_LINUX
FileChooser::FileChooser(QObject *parent) : QObject(parent), m_xdp(this) {
  connect(&m_xdp, &AbstractFileChooser::filesChosen, this, &FileChooser::filesChosen);
  connect(&m_xdp, &AbstractFileChooser::rejected, this, &FileChooser::rejected);
}

bool FileChooser::isAvailable() const {
  if (std::getenv("VICINAE_FORCE_QT_DIALOG") != nullptr) return false;
  return m_xdp.isAvailable();
}

bool FileChooser::open(const FileChooserOptions &options) { return m_xdp.open(options); }

void FileChooser::close() { m_xdp.close(); }
#else
FileChooser::FileChooser(QObject *parent) : QObject(parent) {}

bool FileChooser::isAvailable() const { return false; }

bool FileChooser::open(const FileChooserOptions &) { return false; }

void FileChooser::close() {}
#endif

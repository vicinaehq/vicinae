#include "file-chooser.hpp"
#include "xdp-file-chooser/xdp-file-chooser.hpp"
#include <qlogging.h>

FileChooser::FileChooser(QObject *parent) : QObject(parent) {}

void FileChooser::connectBackend(AbstractFileChooser *backend) {
  connect(backend, &AbstractFileChooser::filesChosen, this, &FileChooser::filesChosen);
  connect(backend, &AbstractFileChooser::rejected, this, &FileChooser::rejected);
}

void FileChooser::open(const FileChooserOptions &options) {
  auto *xdp = new XdpFileChooser(this);
  connectBackend(xdp);

  if (!xdp->open(options)) {
    qWarning() << "FileChooser: XDP portal unavailable, no fallback available";
    delete xdp;
    emit rejected();
  }
}

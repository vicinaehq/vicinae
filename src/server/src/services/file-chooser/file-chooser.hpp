#pragma once
#include "abstract-file-chooser.hpp"
#include "xdp-file-chooser/xdp-file-chooser.hpp"
#include <qobject.h>
#include <qtmetamacros.h>

// Thin wrapper around XdpFileChooser that exposes availability.
// When the portal is unavailable, callers should fall back to a QML dialog.
class FileChooser : public QObject {
  Q_OBJECT

signals:
  void filesChosen(const std::vector<std::filesystem::path> &paths);
  void rejected();

public:
  explicit FileChooser(QObject *parent = nullptr);

  bool isAvailable() const;
  bool open(const FileChooserOptions &options);
  void close();

private:
  XdpFileChooser m_xdp;
};

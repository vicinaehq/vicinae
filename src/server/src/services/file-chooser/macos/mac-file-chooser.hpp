#pragma once
#include "services/file-chooser/abstract-file-chooser.hpp"
#include <qobject.h>

class MacFileChooser : public AbstractFileChooser {
  Q_OBJECT

public:
  explicit MacFileChooser(QObject *parent = nullptr);
  ~MacFileChooser() override;

  bool isAvailable() const override;
  bool open(const FileChooserOptions &options) override;
  void close() override;

private:
  void *m_panel = nullptr; // retained NSOpenPanel
};

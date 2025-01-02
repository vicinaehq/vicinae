#pragma once
#include "common.hpp"
#include <QClipboard>
#include <qapplication.h>
#include <qclipboard.h>
#include <qdir.h>

class ClipboardService : public NonAssignable {
public:
  ClipboardService() {}

  void copyText(const QString &text) {
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setText(text);
  }
};

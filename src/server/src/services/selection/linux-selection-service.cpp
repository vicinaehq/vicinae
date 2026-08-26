#include "linux-selection-service.hpp"
#include <QClipboard>
#include <QGuiApplication>

QFuture<AbstractSelectionService::Result> LinuxSelectionService::selectedText() {
  QString text = QGuiApplication::clipboard()->text(QClipboard::Mode::Selection);

  if (text.isEmpty()) {
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(QStringLiteral("Unable to get selected text")));
  }

  return QtFuture::makeReadyValueFuture<Result>(std::move(text));
}

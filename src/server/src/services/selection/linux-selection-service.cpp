#include "linux-selection-service.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include <QClipboard>
#include <QGuiApplication>

LinuxSelectionService::LinuxSelectionService(ClipboardService &clipboard) {
  connect(&clipboard, &ClipboardService::primarySelectionChanged, this,
          [this](const QString &text) { m_primaryText = text; });
}

QFuture<AbstractSelectionService::Result> LinuxSelectionService::selectedText() {
  QString text = m_primaryText;

  // best effort if the compositor doesn't support data control, but that's not accurate.
  if (text.isEmpty()) text = QGuiApplication::clipboard()->text(QClipboard::Mode::Selection);

  if (text.isEmpty()) {
    return QtFuture::makeReadyValueFuture<Result>(
        std::unexpected(QStringLiteral("Unable to get selected text")));
  }

  return QtFuture::makeReadyValueFuture<Result>(std::move(text));
}

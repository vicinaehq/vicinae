#include "text-file-viewer.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"

void TextFileViewer::load(const std::filesystem::path &path) {
  size_t READ_LIMIT = 1024 * 10; // 10 KB
  QFile file(path);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Failed to open file at" << path.c_str();
    return;
  }

  edit->setPlainText(file.read(READ_LIMIT));
}

void TextFileViewer::load(const QByteArray &data) { edit->setPlainText(data); }

TextFileViewer::TextFileViewer() : edit(new QTextEdit()) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  auto layout = new QVBoxLayout;

  edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  edit->document()->setDocumentMargin(10);
  edit->setTabStopDistance(40);
  edit->setReadOnly(true);
  edit->setVerticalScrollBar(new OmniScrollBar);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(edit);
  setLayout(layout);
}

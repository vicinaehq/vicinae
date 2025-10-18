#include "text-file-viewer.hpp"
#include "layout.hpp"
#include "template-engine/template-engine.hpp"
#include "theme.hpp"
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

void TextFileViewer::updateStyle() {
  TemplateEngine engine;
  double size = ThemeService::instance().pointSize(TextSize::TextRegular);
  engine.setVar("FONT_SIZE", QString::number(size));
  QString stylesheet = engine.build(R"(
		QTextEdit {
			font-size: {FONT_SIZE}pt;
		}
	)");
  setStyleSheet(stylesheet);
}

TextFileViewer::TextFileViewer() : edit(new QTextEdit()) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  edit->document()->setDocumentMargin(10);
  edit->setTabStopDistance(40);
  edit->setReadOnly(true);
  edit->setVerticalScrollBar(new OmniScrollBar);
  updateStyle();
  VStack().add(edit).imbue(this);
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { updateStyle(); });
}

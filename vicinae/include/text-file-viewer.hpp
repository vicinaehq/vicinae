#include "ui/scroll-bar/scroll-bar.hpp"
#include <qboxlayout.h>
#include <qdir.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qstringview.h>
#include <qsyntaxhighlighter.h>
#include <qtextedit.h>
#include <qwidget.h>
#include <QTextEdit>

class TextFileViewer : public QWidget {
  QTextEdit *edit;

public:
  void load(const std::filesystem::path &path) {
    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { edit->setPlainText(file.readAll()); }
  }

  void load(const QByteArray &data) { edit->setPlainText(data); }

  void resizeEvent(QResizeEvent *event) override { QWidget::resizeEvent(event); }

  TextFileViewer() : edit(new QTextEdit()) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    auto layout = new QVBoxLayout;

    edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    edit->document()->setDocumentMargin(2);
    edit->setTabStopDistance(40);
    edit->setReadOnly(true);
    edit->setVerticalScrollBar(new OmniScrollBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(edit);
    setLayout(layout);
  }
};

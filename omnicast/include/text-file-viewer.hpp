#include "ui/omni-scroll-bar.hpp"
#include <qboxlayout.h>
#include <qdir.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qsyntaxhighlighter.h>
#include <qtextedit.h>
#include <qwidget.h>
#include <QTextEdit>

class TextFileViewer : public QWidget {
  QTextEdit *edit;

public:
  void load(const QString &path) {
    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { edit->setPlainText(file.readAll()); }
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    /*
int docHeight = edit->document()->size().toSize().height();
qDebug() << "resize" << docHeight;
edit->setFixedHeight(docHeight);
    */
  }

  TextFileViewer() : edit(new QTextEdit()) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    auto layout = new QVBoxLayout;

    connect(edit, &QTextEdit::textChanged, this, [this]() {
      // int docHeight = edit->document()->size().toSize().height();
      // qDebug() << "resize" << docHeight;
      // edit->setFixedHeight(docHeight);
    });

    edit->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    edit->setReadOnly(true);
    edit->setVerticalScrollBar(new OmniScrollBar);
    layout->addWidget(edit, 1);
    setLayout(layout);
  }
};

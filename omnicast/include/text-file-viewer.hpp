#include "theme.hpp"
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

protected:
  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();

    {
      QPainter painter(this);

      painter.setRenderHint(QPainter::Antialiasing);
      painter.setBrush(theme.colors.mainHoveredBackground);
      painter.setPen(QPen(theme.colors.border, 1));
      painter.drawRoundedRect(rect(), 4, 4);
    }

    QWidget::paintEvent(event);
  }

public:
  void load(const QString &path) {
    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { edit->setPlainText(file.readAll()); }
  }

  TextFileViewer() : edit(new QTextEdit()) {
    setAttribute(Qt::WA_TranslucentBackground, true);
    auto layout = new QVBoxLayout;

    edit->setVerticalScrollBar(new OmniScrollBar);
    layout->addWidget(edit);
    setLayout(layout);
  }
};

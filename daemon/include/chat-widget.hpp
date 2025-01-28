#pragma once
#include <qboxlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qtextedit.h>
#include <qwidget.h>

class ChatMessageWidget : public QWidget {
  QLabel *label;

public:
  ChatMessageWidget() : label(new QLabel) {
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto layout = new QHBoxLayout;

    layout->setAlignment(Qt::AlignTop);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    setLayout(layout);
  }

  void setMarkdown(const QString &markdown) { label->setText(markdown); }
  QString markdown() { return label->text(); }
};

class ChatWidget : public QScrollArea {
  QVBoxLayout *layout;
  QScrollBar *vscroll;

  void resizeEvent(QResizeEvent *event) override {
    QScrollArea::resizeEvent(event);
    // scrollToBottom();
  }

public:
  ChatWidget() : layout(new QVBoxLayout), vscroll(verticalScrollBar()) {
    auto widget = new QWidget;

    layout->setAlignment(Qt::AlignTop);
    widget->setLayout(layout);

    setWidget(widget);
    setWidgetResizable(true);
    widget->setAutoFillBackground(false);
  }

  void addMessage(ChatMessageWidget *widget) { layout->addWidget(widget); }

  void scrollToBottom() { vscroll->setValue(vscroll->maximum()); }
};

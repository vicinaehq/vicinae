#pragma once
#include "ui/test-list.hpp"
#include <QBoxLayout>
#include <QScrollArea>
#include <qboxlayout.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qminmax.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qwidget.h>

static int WIDGET_SIZE = 40;

class VirtualListWidget : public QScrollArea {
  QList<std::shared_ptr<AbstractNativeListItem>> items;
  QVBoxLayout *layout;
  QWidget *listWidget;
  QWidget *spacer;
  int y = 0;
  QHash<int, QWidget *> visibleWidgets;

private:
  void scrollContentsBy(int dx, int dy) override {
    // QScrollArea::scrollContentsBy(dx, dy);
    y -= dy;
    qDebug() << "scroll contents by" << dx << "x" << dy << "height" << height();
    qDebug() << "new y" << y;

    updateVisibleItems(y, height());
  }

  void updateVisibleItems(int y, int height) {
    int startIndex = qMax(0, y / WIDGET_SIZE);
    int endIndex = items.size() > 0 ? qMin(startIndex + (height / WIDGET_SIZE),
                                           items.size() - 1)
                                    : 0;

    qDebug() << "start" << startIndex << "end" << endIndex;

    spacer->setFixedHeight(startIndex * WIDGET_SIZE);

    qDebug() << "spacer" << startIndex * WIDGET_SIZE;

    if (!items.isEmpty()) {
      for (int i = startIndex; i <= endIndex; ++i) {
        if (visibleWidgets.contains(i))
          continue;

        auto &item = items.at(i);
        auto widget = item->createItem();

        if (!widget)
          continue;

        layout->insertWidget(i - startIndex + 1, widget);
        visibleWidgets.insert(i, widget);
        widget->setFixedHeight(WIDGET_SIZE);
      }
    }

    for (auto idx : visibleWidgets.keys()) {
      if (idx < startIndex || idx > endIndex) {
        auto widget = visibleWidgets.value(idx);

        layout->removeWidget(widget);
        widget->deleteLater();
        visibleWidgets.remove(idx);
      }
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    QScrollArea::resizeEvent(event);
    updateVisibleItems(y, event->size().height());
  }

public:
  VirtualListWidget()
      : listWidget(new QWidget), layout(new QVBoxLayout), spacer(new QWidget) {
    setStyleSheet("QWidget { background: transparent; }");

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(0);

    spacer->setFixedHeight(0);

    spacer->setStyleSheet("background-color: red");

    layout->addWidget(spacer);

    listWidget->setLayout(layout);
    setWidgetResizable(true);
    setWidget(listWidget);
  }

  void setItems(const QList<std::shared_ptr<AbstractNativeListItem>> &items) {
    while (layout->count() > 1) {
      if (auto item = layout->takeAt(1)) {
        item->widget()->deleteLater();
      }
    }
    visibleWidgets.clear();

    this->items = items;
    y = 0;
    qDebug() << "list size" << items.size() * 40;
    listWidget->setMinimumHeight(items.size() * 40);
    updateVisibleItems(y, height());
  }

  /*
  void setItems(const QList<std::shared_ptr<AbstractNativeListItem>> &items) {
    setAutoFillBackground(false);
    QWidget *list = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout;

    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    list->setLayout(layout);

    qDebug() << "will set " << items.size();

    for (const auto &item : items) {
      qDebug() << "add widget";
      auto widget = item->createItem();

      if (!widget) {
        qDebug() << "no item merde!";
        return;
      }

      widget->setFixedHeight(widget->sizeHint().height());

      layout->addWidget(widget);
    }

    if (auto oldWidget = widget())
      oldWidget->deleteLater();

    setWidget(list)
  }
  */
};

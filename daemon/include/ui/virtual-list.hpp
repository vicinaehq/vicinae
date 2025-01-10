#pragma once
#include "ui/native-list.hpp"
#include "ui/test-list.hpp"
#include <QBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <numbers>
#include <qboxlayout.h>
#include <qevent.h>
#include <qframe.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qminmax.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscrollarea.h>
#include <qstyleoption.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <regex>
#include <variant>

static int WIDGET_SIZE = 40;

struct VirtualListSection {
  QString name;
  QList<std::shared_ptr<AbstractNativeListItem>> items;
};

using VirtualListChild =
    std::variant<VirtualListSection, std::shared_ptr<AbstractNativeListItem>>;

struct VirtualListItem {
  int offset;
  std::shared_ptr<AbstractNativeListItem> item;
};

class SectionLabelListItem : public AbstractNativeListItem {
  QString section;
  int count;

  QWidget *createItem() const override {
    return new ListSectionHeader(section, "", count);
  }

  bool isSelectable() const override { return false; }

  int height() const override { return 30; }

public:
  void setCount(int count) { this->count = count; }

  SectionLabelListItem(const QString &section) : section(section) {}
};

class VirtualListModel : public QObject {
  Q_OBJECT

  using Item = std::shared_ptr<AbstractNativeListItem>;

  struct Section {
    QString name;
    QList<Item> items;
  };

  std::optional<Section> currentSection;
  QList<Item> items;

signals:
  void commitReset(const QList<Item> &items);

public:
  void beginReset() { items.clear(); }

  void endReset() {
    endSection();
    emit commitReset(items);
  }

  void beginSection(const QString &name) {
    endSection();
    currentSection = {.name = name};
  }

  void endSection() {
    if (currentSection) {
      if (!currentSection->items.isEmpty()) {
        auto labelItem =
            std::make_shared<SectionLabelListItem>(currentSection->name);

        labelItem->setCount(currentSection->items.size());
        items << labelItem << currentSection->items;
      }

      currentSection.reset();
    }
  }

  void addItem(const std::shared_ptr<AbstractNativeListItem> &item) {
    if (currentSection) {
      currentSection->items << item;
      return;
    }

    items << item;
  }

  VirtualListModel() {}
};
class VirtualListItemWidget : public QFrame {
  Q_OBJECT

  QWidget *widget = nullptr;

  void resizeEvent(QResizeEvent *event) override {
    qDebug() << "resize" << event->size();
    if (widget)
      widget->setFixedSize(event->size());
  }

public:
  VirtualListItemWidget(QWidget *parent = nullptr) : QFrame(parent) {}

  void setWidget(QWidget *w) {
    widget = w;
    w->setParent(this);
    qDebug() << "size" << size();
    w->setFixedSize(size());
  }
};

class VirtualListContainer : public QFrame {
  Q_OBJECT

public:
  QVBoxLayout *layout;
  QWidget *spacer;

  VirtualListContainer() : layout(new QVBoxLayout), spacer(new QWidget) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(0);
    layout->addWidget(spacer);

    spacer->setFixedHeight(0);
    setLayout(layout);
  }
};

class VirtualListWidget : public QScrollArea {
  Q_OBJECT

  int y = 0;
  QHash<int, QWidget *> visibleWidgets;
  QList<VirtualListItem> virtualItems;
  int selected = 0;

  VirtualListContainer *container;

  VirtualListModel *model = nullptr;

private:
  void scrollContentsBy(int dx, int dy) override {
    // QScrollArea::scrollContentsBy(dx, dy);
    y -= dy;
    qDebug() << "scroll contents by" << dx << "x" << dy << "height" << height();
    qDebug() << "new y" << y;

    updateVisibleItems(y, height());
  }

  int getPreviousSelected() {
    int previous = selected - 1;

    while (previous >= 0 && !virtualItems.at(previous).item->isSelectable())
      ++previous;

    return previous;
  }

  int getNextSelected() {
    int next = selected + 1;

    while (selected < virtualItems.size() &&
           !virtualItems.at(next).item->isSelectable())
      ++next;

    return next;
  }

  void selectItem(int index) {
    if (index >= virtualItems.size())
      return;

    qDebug() << "selecting" << index;

    for (auto idx : visibleWidgets.keys()) {
      auto widget = visibleWidgets.value(idx);

      widget->setProperty("selected", idx == index);
      widget->style()->unpolish(widget);
      widget->style()->polish(widget);

      // widget->setStyleSheet("background-color: red;");
    }

    selected = index;

    emit selectionChanged(*virtualItems.at(index).item);
    updateVisibleItems(y, height());
  }

  void updateVisibleItems(int y, int height) {
    if (virtualItems.isEmpty())
      return;

    int startIndex = 0;

    while (startIndex < virtualItems.size() - 1 &&
           virtualItems.at(startIndex).offset < y)
      ++startIndex;

    int endIndex = startIndex;
    int startOffset = virtualItems.at(endIndex).offset;

    while (endIndex < virtualItems.size() - 1 &&
           virtualItems.at(endIndex).offset - startOffset < height)
      ++endIndex;

    qDebug() << "range" << startIndex << "to" << endIndex;

    qDebug() << "start" << startIndex << "end" << endIndex;

    if (startIndex > 1) {
      container->spacer->setFixedHeight(virtualItems.at(startIndex - 1).offset);
    } else {
      container->spacer->setFixedHeight(0);
    }

    qDebug() << "spacer" << startOffset;

    for (auto idx : visibleWidgets.keys()) {
      if (idx < startIndex || idx > endIndex) {
        auto widget = visibleWidgets.value(idx);

        container->layout->removeWidget(widget);
        widget->deleteLater();
        visibleWidgets.remove(idx);
      }
    }

    for (int i = startIndex; i <= endIndex; ++i) {
      if (visibleWidgets.contains(i))
        continue;

      auto &item = virtualItems.at(i).item;
      auto widget = item->createItem();

      if (!widget)
        continue;

      auto listItemWidget = new VirtualListItemWidget();

      qDebug() << "new widget";

      listItemWidget->setFixedHeight(item->height());
      listItemWidget->setWidget(widget);

      container->layout->insertWidget(i - startIndex + 1, listItemWidget);
      visibleWidgets.insert(i, listItemWidget);
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    QScrollArea::resizeEvent(event);
    updateVisibleItems(y, event->size().height());
  }

  bool eventFilter(QObject *, QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);

      switch (keyEvent->key()) {
      case Qt::Key_Up:
        selectItem(getPreviousSelected());
        return true;
        break;
      case Qt::Key_Down:
        selectItem(getNextSelected());
        return true;
        break;
      default:
        break;
      }
    }

    return false;
  }

public:
  VirtualListWidget() : container(new VirtualListContainer) {
    installEventFilter(this);
    setFrameShape(QFrame::NoFrame);
    setWidgetResizable(true);
    setWidget(container);
    container->setAutoFillBackground(false);
  }

  void setModel(VirtualListModel *newModel) {
    if (model)
      model->deleteLater();
    model = newModel;
    connect(model, &VirtualListModel::commitReset, this,
            &VirtualListWidget::setItems);
  }

  void clear() {
    while (container->layout->count() > 1) {
      if (auto item = container->layout->takeAt(1)) {
        item->widget()->deleteLater();
      }
    }
    visibleWidgets.clear();
  }

  void setItems(const QList<std::shared_ptr<AbstractNativeListItem>> &items) {
    QList<VirtualListItem> virtualItems;
    int offset = 0;

    clear();

    for (auto &item : items) {
      virtualItems.push_back({.offset = offset, .item = item});
      offset += item->height();
    }

    this->virtualItems = virtualItems;
    container->setMinimumHeight(offset);

    if (verticalScrollBar()->value() > 0) {
      verticalScrollBar()->setValue(0);
    } else {
      updateVisibleItems(y, height());
    }
  }

signals:
  void selectionChanged(const AbstractNativeListItem &);
};

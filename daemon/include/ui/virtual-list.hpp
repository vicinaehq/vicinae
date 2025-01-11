#pragma once
#include "ui/native-list.hpp"
#include "ui/test-list.hpp"
#include <QBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <numbers>
#include <qboxlayout.h>
#include <qcoreevent.h>
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
    if (widget)
      widget->setFixedSize(event->size());
  }

  void updateStyle() {
    style()->polish(this);
    style()->unpolish(this);
  }

  void enterEvent(QEnterEvent *event) override {
    setProperty("hovered", true);
    updateStyle();
  }

  void leaveEvent(QEvent *event) override {
    setProperty("hovered", false);
    updateStyle();
  }

public:
  VirtualListItemWidget(QWidget *parent = nullptr) : QFrame(parent) {
    setMouseTracking(true);
  }

  void setSelected(bool selected) {
    setProperty("selected", selected);
    updateStyle();
  }

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

  QHash<int, VirtualListItemWidget *> visibleWidgets;
  QList<VirtualListItem> virtualItems;
  int currentSelectionIndex = 0;

  VirtualListContainer *container;
  VirtualListModel *model = nullptr;

private:
  void scrollContentsBy(int dx, int dy) override {
    /*
// QScrollArea::scrollContentsBy(dx, dy);
y -= dy;
qDebug() << "scroll contents by" << dx << "x" << dy << "height" << height();
qDebug() << "new y" << y;

updateVisibleItems(y, height());
  */
  }

  void valueChanged(int value) {
    qDebug() << "value" << value;
    updateVisibleItems(value, height());
  }

  int getPreviousSelected() {
    if (currentSelectionIndex == 0)
      return currentSelectionIndex;

    int previous = currentSelectionIndex - 1;

    while (previous >= 0 && !virtualItems.at(previous).item->isSelectable())
      ++previous;

    return previous;
  }

  int getNextSelected() {
    if (currentSelectionIndex == virtualItems.size() - 1)
      return currentSelectionIndex;

    int next = currentSelectionIndex + 1;

    while (currentSelectionIndex < virtualItems.size() &&
           !virtualItems.at(next).item->isSelectable())
      ++next;

    return next;
  }

  bool isInViewport() { auto value = verticalScrollBar()->value(); }

  void selectItem(int selectionIndex) {
    if (selectionIndex >= virtualItems.size())
      return;

    qDebug() << "selecting" << selectionIndex;

    if (auto previousItem = visibleWidgets.value(currentSelectionIndex)) {
      previousItem->setSelected(false);
    }

    if (auto nextItem = visibleWidgets.value(selectionIndex)) {
      nextItem->setSelected(true);
    }

    auto &newItem = virtualItems.at(selectionIndex);

    auto scrollValue = verticalScrollBar()->value();

    int targetOffset = newItem.offset;

    int scrollOffset = scrollValue + height();

    qDebug() << QString("%1 > %2").arg(targetOffset).arg(scrollOffset);

    if (newItem.offset < scrollOffset) {
      int space = scrollOffset - newItem.offset;
      int height = newItem.item->height();

      if (space < height) {
        verticalScrollBar()->setValue(scrollValue + height - space);
      }
    }

    if (targetOffset > scrollOffset) {
      verticalScrollBar()->setValue(scrollValue + targetOffset - scrollOffset);
    }

    currentSelectionIndex = selectionIndex;

    emit selectionChanged(*virtualItems.at(selectionIndex).item);
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

      listItemWidget->setFixedHeight(item->height());
      listItemWidget->setWidget(widget);

      if (currentSelectionIndex == i)
        listItemWidget->setSelected(true);

      container->layout->insertWidget(i - startIndex + 1, listItemWidget);
      visibleWidgets.insert(i, listItemWidget);
    }
  }

  /*
  void resizeEvent(QResizeEvent *event) override {
    QScrollArea::resizeEvent(event);
    updateVisibleItems(y, event->size().height());
  }
  */

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

    connect(verticalScrollBar(), &QScrollBar::valueChanged, this,
            &VirtualListWidget::valueChanged);
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
      updateVisibleItems(0, height());
    }

    int firstSelected = 0;

    while (firstSelected < virtualItems.size() &&
           !virtualItems.at(firstSelected).item->isSelectable()) {
      ++firstSelected;
    }

    selectItem(firstSelected);
  }

signals:
  void selectionChanged(const AbstractNativeListItem &);
};

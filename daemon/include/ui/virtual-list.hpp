#pragma once
#include "extension_manager.hpp"
#include "ui/text-label.hpp"
#include <QWidget>
#include <chrono>
#include <numbers>
#include <qapplication.h>
#include <QStack>
#include <qboxlayout.h>
#include <qevent.h>
#include <qobject.h>
#include <qpainterpath.h>
#include <QVBoxLayout>
#include <qscrollbar.h>
#include <qscrollarea.h>
#include <qpainter.h>
#include <qframe.h>
#include <qtimer.h>
#include <quuid.h>
#include <qwidget.h>
#include <sys/socket.h>

class ListSectionHeader : public QWidget {
public:
  ListSectionHeader(const QString &title, const QString &subtitle, size_t count) {
    setAttribute(Qt::WA_StyledBackground);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(8, 8, 8, 8);

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(title));
    if (count > 0) { leftLayout->addWidget(new TextLabel(QString::number(count))); }
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

    setLayout(layout);
  }
};

static const int SCROLL_GAP = 5;

class AbstractVirtualListItem : public QObject {
  size_t m_id;

public:
  virtual QWidget *createItem() const = 0;
  virtual QWidget *updateItem(QWidget *widget) const { return createItem(); };
  virtual bool isSelectable() const { return true; }
  virtual int role() const { return qHash(QUuid::createUuid()); }
  virtual int height() const = 0;
  virtual size_t id() const {
    qDebug() << "default id called";
    return m_id;
  }

  AbstractVirtualListItem(size_t id = qHash(QUuid::createUuid())) : m_id(id) {}
};

struct VirtualListSection {
  QString name;
  QList<std::shared_ptr<AbstractVirtualListItem>> items;
};

using VirtualListChild = std::variant<VirtualListSection, std::shared_ptr<AbstractVirtualListItem>>;

struct VirtualListItem {
  int offset;
  std::shared_ptr<AbstractVirtualListItem> item;
};

enum BuiltinListRole { SectionLabel = 256 };

class SectionLabelListItem : public AbstractVirtualListItem {
  QString section;
  int count;

  QWidget *createItem() const override { return new ListSectionHeader(section, "", count); }

  int role() const override { return SectionLabel; }

  bool isSelectable() const override { return false; }

  int height() const override { return 30; }

public:
  void setCount(int count) { this->count = count; }

  SectionLabelListItem(const QString &section) : section(section) {}
};

class VirtualListModel : public QObject {
  Q_OBJECT

  using Item = std::shared_ptr<AbstractVirtualListItem>;

  struct Section {
    QString name;
    QList<Item> items;
  };

  std::optional<Section> currentSection;
  QList<Item> items;

signals:
  void commitReset(const QList<Item> &items);
  void selectionChanged(const Item &item);
  void itemRemoved(size_t id);

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
        auto labelItem = std::make_shared<SectionLabelListItem>(currentSection->name);

        labelItem->setCount(currentSection->items.size());
        items << labelItem << currentSection->items;
      }

      currentSection.reset();
    }
  }

  void setSelected(const std::shared_ptr<AbstractVirtualListItem> &item) { emit selectionChanged(item); }

  void addItem(const std::shared_ptr<AbstractVirtualListItem> &item) {
    if (currentSection) {
      currentSection->items << item;
      return;
    }

    items << item;
  }

  void removeItem(size_t id) {
    for (size_t i = 0; i != items.size(); ++i) {
      const auto &item = items.at(i);

      if (item->id() != id) continue;

      if (i > 0) {
        const auto &before = items.at(i - 1);
        bool beforeCheck = before->role() == BuiltinListRole::SectionLabel;
        bool afterCheck = i + 1 == items.size() || items.at(i + 1)->role() == BuiltinListRole::SectionLabel;

        if (beforeCheck && afterCheck) removeItem(before->id());
      }

      items.removeAt(i);
      break;
    }

    emit itemRemoved(id);
  }

  void removeItem(const std::shared_ptr<AbstractVirtualListItem> &item) { removeItem(item->id()); }

  VirtualListModel() {}
};

class VirtualListItemWidget : public QFrame {
  Q_OBJECT

  bool selectable = true;
  bool isSelected = false;
  bool isHovered = false;

  void resizeEvent(QResizeEvent *event) override {
    if (widget) widget->setFixedSize(event->size());
  }

  void enterEvent(QEnterEvent *event) override { emit hovered(true); }
  void leaveEvent(QEvent *event) override { emit hovered(false); }

  void mousePressEvent(QMouseEvent *event) override {
    if (!selectable) return;
    if (event->button() == Qt::LeftButton) { emit selected(); }
  }

  void mouseDoubleClickEvent(QMouseEvent *event) override {
    if (!selectable) return;
    if (event->button() == Qt::LeftButton) { emit leftDoubleClick(); }
  }

  void paintEvent(QPaintEvent *) override {
    if (isSelected || isHovered) {
      int borderRadius = 10;
      QPainter painter(this);

      painter.setRenderHint(QPainter::Antialiasing, true);

      QPainterPath path;
      path.addRoundedRect(rect(), borderRadius, borderRadius);

      painter.setClipPath(path);

      QColor backgroundColor("#282726");

      painter.fillPath(path, backgroundColor);
    }
  }

public:
  QWidget *widget = nullptr;
  int type;

  VirtualListItemWidget(QWidget *parent = nullptr) : QFrame(parent) {}

  void setSelectable(bool selectable = true) { this->selectable = selectable; }

  void setSelected(bool selected) {
    if (!selectable) return;

    isSelected = selected;
    repaint();
  }

  void setHovered(bool hovered) {
    if (!selectable) return;

    isHovered = hovered;
    repaint();
  }

  void setWidget(QWidget *w, int type) {
    this->type = type;
    widget = w;
    w->setParent(this);
    w->setFixedSize(size());
  }

signals:
  void hovered(bool flag);
  void selected();
  void leftDoubleClick();
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

class VirtualListWidget : public QWidget {
  Q_OBJECT

  QScrollBar *scrollBar;
  QMap<int, VirtualListItemWidget *> visibleWidgets;
  QList<VirtualListItem> items;
  int currentSelectionIndex = 0;
  VirtualListContainer *container;
  VirtualListModel *model = nullptr;
  QHash<int, QStack<VirtualListItemWidget *>> widgetPools;

  int virtualScrollHeight = 0;
  int virtualHeight = 0;

  QWidget *viewport;

private:
  void valueChanged(int value) { updateVisibleItems(value, height()); }

  void resizeEvent(QResizeEvent *event) override {
    virtualScrollHeight = qMax(virtualHeight - height(), 0);
    scrollBar->setMaximum(virtualScrollHeight);

    updateVisibleItems(scrollBar->value(), event->size().height());
    QWidget::resizeEvent(event);
  }

  int getPreviousSelected() {
    if (currentSelectionIndex == 0) return currentSelectionIndex;

    int previous = currentSelectionIndex - 1;

    while (previous >= 0 && !items.at(previous).item->isSelectable())
      --previous;

    return previous;
  }

  int getNextSelected() {
    if (currentSelectionIndex == items.size() - 1) return currentSelectionIndex;

    int next = currentSelectionIndex + 1;

    while (currentSelectionIndex < items.size() && !items.at(next).item->isSelectable())
      ++next;

    return next;
  }

  void updateVisibleItems(int y, int height) {
    if (virtualScrollHeight == 0)
      scrollBar->hide();
    else
      scrollBar->show();

    if (items.isEmpty()) return;

    int startIndex = 0;

    while (startIndex < items.size() - 1 &&
           items.at(startIndex).offset + items.at(startIndex).item->height() < y)
      ++startIndex;

    int endIndex = startIndex;
    int startOffset = items.at(endIndex).offset;

    while (endIndex < items.size() - 1 && items.at(endIndex).offset - startOffset < height)
      ++endIndex;

    qDebug() << "items" << items.size();
    qDebug() << "y" << y;

    qDebug() << "range" << startIndex << "to" << endIndex;
    qDebug() << "start" << startIndex << "end" << endIndex;

    int offset = items.at(startIndex).offset - y;

    qDebug() << "base offset" << offset;

    for (int idx : visibleWidgets.keys()) {
      // if (idx >= startIndex && idx <= endIndex) continue;

      visibleWidgets.value(idx)->deleteLater();

      /*
    if (auto widget = visibleWidgets.value(idx)) {
      widget->deleteLater();
      widget->widget->deleteLater();

auto &pool = widgetPools[widget->type];

widget->setParent(nullptr);
widget->hide();
pool.push_back(widget);
    }
    */

      visibleWidgets.remove(idx);
    }

    visibleWidgets.clear();

    for (int i = startIndex; i <= endIndex; ++i) {
      VirtualListItemWidget *itemWidget = visibleWidgets.value(i);
      auto &item = items.at(i).item;

      if (!itemWidget) {
        /*
auto &pool = widgetPools[item->role()];

if (!pool.isEmpty()) {
itemWidget = pool.pop();
auto updatedWidget = item->updateItem(itemWidget->widget);

if (updatedWidget != itemWidget->widget) {
  itemWidget->widget->deleteLater();
  itemWidget->setWidget(updatedWidget, item->role());
}
}
      */

        itemWidget = new VirtualListItemWidget(viewport);

        itemWidget->setFixedSize({viewport->width(), item->height()});
        itemWidget->setWidget(item->createItem(), item->role());
        itemWidget->setSelectable(item->isSelectable());

        connect(itemWidget, &VirtualListItemWidget::selected, this, [this, i]() { selectItem(i); });
        connect(itemWidget, &VirtualListItemWidget::leftDoubleClick, this,
                [this, item]() { itemActivated(item); });
        connect(itemWidget, &VirtualListItemWidget::hovered, this,
                [this, i, itemWidget](bool hovered) { itemWidget->setHovered(hovered); });
      }

      itemWidget->move(0, offset);
      itemWidget->show();
      offset += item->height();

      if (currentSelectionIndex == i) itemWidget->setSelected(true);
      visibleWidgets.insert(i, itemWidget);
    }
  }

  void keyPressEvent(QKeyEvent *keyEvent) override {
    if (keyEvent->modifiers() != Qt::Modifiers{}) return;

    switch (keyEvent->key()) {
    case Qt::Key_Up:
      selectItem(getPreviousSelected());
      return;
    case Qt::Key_Down:
      selectItem(getNextSelected());
      return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      activateSelectedItem();
      return;
    default:
      break;
    }
  }

  void activateSelectedItem() {
    qDebug() << "Activate selected";
    if (currentSelectionIndex >= items.size()) return;

    emit itemActivated(items.at(currentSelectionIndex).item);
  }

  void removeById(size_t id) {
    qDebug() << "remove by id" << id;

    for (int i = 0; i != items.size(); ++i) {
      const auto &item = items.at(i).item;

      qDebug() << item->id() << "VS" << id;

      if (item->id() == id) {
        qDebug() << "found" << id << "at index" << i;
        for (int j = i + 1; j < items.size(); ++j) {
          items[j].offset -= item->height();
        }

        items.removeAt(i);

        if (currentSelectionIndex == i) selectItem(getNextSelected());
      }
    }

    updateVisibleItems(scrollBar->value(), height());
  }

  void setItems(const QList<std::shared_ptr<AbstractVirtualListItem>> &items) {
    QList<VirtualListItem> virtualItems;
    int offset = 0;

    clear();

    for (auto &item : items) {
      virtualItems.push_back({.offset = offset, .item = item});
      offset += item->height();
    }

    this->items = virtualItems;
    scrollBar->setMinimum(0);

    virtualHeight = offset;
    virtualScrollHeight = qMax(offset - height(), 0);
    scrollBar->setMaximum(virtualScrollHeight);

    qDebug() << "set items YOLO";

    if (scrollBar->value() > 0) {
      scrollBar->setValue(0);
    } else {
      updateVisibleItems(0, height());
    }

    int firstSelected = 0;

    while (firstSelected < virtualItems.size() && !virtualItems.at(firstSelected).item->isSelectable()) {
      ++firstSelected;
    }

    selectItem(firstSelected);
  }

  bool eventFilter(QObject *watched, QEvent *event) override {
    if (watched == viewport && event->type() == QEvent::Wheel) {
      QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
      QApplication::sendEvent(scrollBar, event);

      return true; // Event is handled, no need to propagate
    }

    return QWidget::eventFilter(watched, event); // Default behavior
  }

public:
  VirtualListWidget(QWidget *parent = nullptr)
      : QWidget(parent), viewport(new QWidget), scrollBar(new QScrollBar) {
    setFocusPolicy(Qt::NoFocus);

    auto layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(viewport);
    layout->addWidget(scrollBar);

    viewport->installEventFilter(this);

    scrollBar->setSingleStep(40);

    setLayout(layout);
    connect(scrollBar, &QScrollBar::valueChanged, this, &VirtualListWidget::valueChanged);
  }

  int selected() { return currentSelectionIndex; }

  void selectFrom(int index) {
    while (index < items.size() && !items.at(index).item->isSelectable())
      ++index;

    if (index == items.size()) return;

    selectItem(index);
  }

  void selectItem(int selectionIndex) {
    if (selectionIndex < 0 || selectionIndex >= items.size()) return;

    qDebug() << "selecting" << selectionIndex;

    if (auto previousItem = visibleWidgets.value(currentSelectionIndex)) { previousItem->setSelected(false); }

    if (auto nextItem = visibleWidgets.value(selectionIndex)) { nextItem->setSelected(true); }

    auto &newItem = items.at(selectionIndex);
    auto scrollHeight = scrollBar->value();

    currentSelectionIndex = selectionIndex;

    if (newItem.offset <= scrollHeight || newItem.offset - scrollHeight < newItem.item->height()) {
      auto distance = scrollHeight - newItem.offset;

      qDebug() << distance;
      scrollBar->setValue(scrollHeight - distance - newItem.item->height() - SCROLL_GAP);
      qDebug() << "scroll up";
      // scroll up
    } else if ((newItem.offset - scrollHeight) > (height() - newItem.item->height())) {
      // scroll down
      auto distance = abs(newItem.offset - scrollHeight - height());
      scrollBar->setValue(scrollHeight + distance + SCROLL_GAP);

      qDebug() << distance;
      qDebug() << "scroll down";
    }

    emit selectionChanged(items.at(selectionIndex).item);
  }

  void clear() {
    for (auto widget : visibleWidgets) {
      widget->deleteLater();
    }
    visibleWidgets.clear();
  }

  void setSelected(size_t id) {
    for (size_t i = 0; i != items.size(); ++i) {
      if (items.at(i).item->id() == id) {
        selectItem(i);
        break;
      }
    }
  }

  void setModel(VirtualListModel *newModel) {
    if (model) model->deleteLater();
    model = newModel;
    connect(model, &VirtualListModel::commitReset, this, &VirtualListWidget::setItems);
    connect(model, &VirtualListModel::itemRemoved, this, &VirtualListWidget::removeById);
  }

signals:
  void selectionChanged(const std::shared_ptr<AbstractVirtualListItem> &);
  void itemActivated(const std::shared_ptr<AbstractVirtualListItem> &);
};

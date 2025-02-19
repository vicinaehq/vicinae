#pragma once
#include "builtin_icon.hpp"
#include "ui/action_popover.hpp"
#include "ui/text-label.hpp"
#include <qboxlayout.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qhash.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <quuid.h>
#include <qwidget.h>

class EllidedLabel : public QLabel {
  void paintEvent(QPaintEvent *event) override {
    QFrame::paintEvent(event);
    QPainter painter(this);
    auto metrics = fontMetrics();
    auto elided = metrics.elidedText(text(), Qt::ElideRight, width());

    painter.drawText(QPoint(0, metrics.ascent()), elided);
  }

public:
  EllidedLabel() { setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); }
};

class AbstractGridItemWidget;

enum GridMemberRole {
  GridSectionNameRole,
  GridItemRole,
};

class AbstractGridMember : public QObject {
public:
  virtual AbstractGridItemWidget *widget(int columnWidth) const = 0;
  virtual int heightForWidth(int columnWidth) const = 0;
  virtual bool selectable() { return true; }
  virtual bool role() { return GridItemRole; }
  virtual int key() const {
    qDebug() << "default key()";
    return qHash(QUuid::createUuid().toString());
  }

  AbstractGridMember() {}
  virtual ~AbstractGridMember() {}
};

class AbstractGridItemWidget : public QWidget {
  Q_OBJECT

public:
  virtual void selectionChanged(bool selected) {}

signals:
  void clicked();
  void doubleClicked();

public:
  AbstractGridItemWidget(QWidget *parent = nullptr) : QWidget(parent) {}
};

class GridListSectionHeader : public AbstractGridItemWidget {
  TextLabel *countLabel;

public:
  GridListSectionHeader(const QString &title, const QString &subtitle, size_t count)
      : countLabel(new TextLabel) {
    setAttribute(Qt::WA_StyledBackground);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(5, 15, 5, 10);

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(title));
    leftLayout->addWidget(countLabel);

    countLabel->setText(QString::number(count));
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

    setLayout(layout);
  }

  void setCount(int count) { countLabel->setText(QString::number(count)); }
};

struct GridSectionLabel : public AbstractGridMember {
  QString name;
  int count;

  AbstractGridItemWidget *widget(int columnWidth) const override {
    return new GridListSectionHeader(name, "", count);
  }

  int heightForWidth(int columnWidth) const override {
    GridListSectionHeader w(name, "", count);

    return w.sizeHint().height();
  }

  int key() const override { return qHash("section:" + name); }

  bool role() override { return GridSectionNameRole; }

  bool selectable() override { return false; }

  void setCount(int count) { this->count = count; }

  GridSectionLabel(const QString &name) : name(name), count(0) {}
};

class VirtualGridSection {
  QString m_name;
  QList<AbstractGridMember *> m_items;

public:
  VirtualGridSection(const QString &name) : m_name(name) {}

  const QString &name() const { return m_name; }
  const QList<AbstractGridMember *> &items() const { return m_items; }

  void setTitle(const QString &title) { m_name = title; }
  void addItem(AbstractGridMember *item) { m_items << item; }
  void removeItem(AbstractGridMember *item) {
    for (int i = 0; i != m_items.size(); ++i) {
      if (m_items[i] == item) {
        m_items.removeAt(i);
        break;
      }
    }
  }
};

class GridItemWrapper : public QWidget {
  Q_OBJECT

  AbstractGridMember *item = nullptr;
  AbstractGridItemWidget *widget = nullptr;

  void resizeEvent(QResizeEvent *event) override {
    if (widget) { widget->setFixedSize(event->size()); }
    QWidget::resizeEvent(event);
  }

public:
  void setItem(AbstractGridMember *item) {
    this->widget = item->widget(maximumWidth());
    this->widget->setFixedSize(maximumSize());
    this->widget->setParent(this);
    this->item = item;

    connect(this->widget, &AbstractGridItemWidget::clicked, this, &GridItemWrapper::clicked);
    connect(this->widget, &AbstractGridItemWidget::doubleClicked, this, &GridItemWrapper::doubleClicked);
  }

  AbstractGridItemWidget *unwrap() const { return widget; }

  void setSelected(bool selected) { widget->selectionChanged(selected); }

  GridItemWrapper(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void clicked();
  void doubleClicked();
};

class OmniScrollBar : public QScrollBar {
  Q_OBJECT

  bool isSliderShown = false;
  bool isHovered = false;
  QTimer dismissTimer;

  void handleValueChanged(int value) {
    if (!isHovered) {
      isSliderShown = true;
      dismissTimer.start();
    }
  }

  void setSliderVisibility(bool visible) {
    isSliderShown = visible;
    update();
  }

  void enterEvent(QEnterEvent *event) override {
    isHovered = true;
    setSliderVisibility(true);
  }
  void leaveEvent(QEvent *event) override {
    isHovered = false;
    setSliderVisibility(false);
  }

  void paintEvent(QPaintEvent *event) override {
    Q_UNUSED(event);

    if (!isSliderShown) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Get scrollbar handle geometry
    QStyleOptionSlider opt;
    opt.initFrom(this);
    opt.orientation = orientation();
    opt.minimum = minimum();
    opt.maximum = maximum();
    opt.sliderPosition = value();
    opt.sliderValue = value();
    opt.singleStep = singleStep();
    opt.pageStep = pageStep();

    QRect handleRect = style()->subControlRect(QStyle::CC_ScrollBar, &opt, QStyle::SC_ScrollBarSlider, this);
    QColor handleColor = QColor(100, 100, 100, 150);

    // painter.setBrush(QColor("red"));
    // painter.drawRect(rect());

    painter.setBrush(handleColor);
    painter.setPen(Qt::NoPen);

    // Rounded scrollbar
    int radius = 4;
    if (orientation() == Qt::Horizontal) {
      // handleRect.setHeight(8); // Ensure correct thickness
      qDebug() << "horizontal";
    } else {
      handleRect.setWidth(8);

      auto xAdjust = round((width() - handleRect.width()) / (double)2);

      handleRect = handleRect.adjusted(xAdjust, 0, xAdjust, 0);
    }
    painter.drawRoundedRect(handleRect, radius, radius);
  }

public:
  OmniScrollBar(QWidget *parent = nullptr) : QScrollBar(parent) {
    setStyleSheet("background: transparent;");
    setAttribute(Qt::WA_Hover, true);

    dismissTimer.setSingleShot(true);
    dismissTimer.setInterval(500);

    connect(this, &QScrollBar::valueChanged, this, &OmniScrollBar::handleValueChanged);
    connect(&dismissTimer, &QTimer::timeout, this, [this]() { setSliderVisibility(false); });
  }
};

class VirtualGridWidget : public QWidget {
  Q_OBJECT

  struct VirtualWidget {
    int key;
    int height;
    int offset;
    int widthOffset;
    int width;
    AbstractGridMember *item;
  };

  struct ListItemInfo {
    AbstractGridMember *item;
    bool visible;
  };

  QList<VirtualGridSection> m_sections;
  std::vector<VirtualWidget> m_virtual_items;
  std::vector<ListItemInfo> m_items;

  struct RemoveOperation {
    AbstractGridMember *item;
  };

  std::vector<RemoveOperation> pendingRm;

  int ncols = 1;
  int m_spacing = 10;
  int m_selected = -1;
  int m_selection_key = -1;
  struct {
    int left;
    int top;
    int bottom;
    int right;
  } margins;

  QScrollBar *scrollBar;
  QHash<AbstractGridMember *, GridItemWrapper *> itemWidgets;
  QHash<int, GridItemWrapper *> visibleWidgets;
  QHash<int, GridItemWrapper *> widgetCache;

  QHash<AbstractGridMember *, int> vitemMap;

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
      selectLeft();
      break;
    case Qt::Key_Right:
      selectRight();
      break;
    case Qt::Key_Up:
      selectTop();
      break;
    case Qt::Key_Down:
      selectBottom();
      break;
    case Qt::Key_Return:
      if (m_selected >= 0 && m_selected < m_virtual_items.size())
        emit itemActivated(*m_virtual_items[m_selected].item);
      break;
    }
  }

  void selectLeft() {
    auto previous = previousSelectableIndex(m_selected - 1);

    if (previous != -1) setSelected(previous);
  }

  void selectRight() {
    auto next = nextSelectableIndex(m_selected + 1);

    if (next != -1) setSelected(next);
  }

  void selectTop() {
    if (m_selected == -1 || m_selected >= m_virtual_items.size()) return;

    auto current = m_virtual_items[m_selected];
    int nextRowStart = m_selected;

    while (nextRowStart >= 0) {
      auto item = m_virtual_items[nextRowStart];

      if (item.item->selectable() && item.offset < current.offset &&
          item.widthOffset <= current.widthOffset) {
        setSelected(nextRowStart);
        return;
      }

      --nextRowStart;
    }
  }

  void selectBottom() {
    if (m_selected == -1 || m_selected >= m_virtual_items.size()) return;

    int selected = m_selected;
    auto current = m_virtual_items[selected];
    int nextRowStart = selected;

    while (nextRowStart < m_virtual_items.size()) {
      while (nextRowStart < m_virtual_items.size() &&
             m_virtual_items[nextRowStart].offset == current.offset) {
        ++nextRowStart;
      }

      int nextRowEnd = nextRowStart;

      while (nextRowEnd < m_virtual_items.size() &&
             m_virtual_items[nextRowEnd].offset == m_virtual_items[nextRowStart].offset) {
        ++nextRowEnd;
      }

      int rowCount = nextRowEnd - nextRowStart;

      for (int i = 0; i != rowCount; ++i) {
        int index = nextRowEnd - i - 1;
        auto item = m_virtual_items[index];

        if (item.item->selectable() && item.widthOffset <= current.widthOffset) {
          setSelected(index);
          return;
        }
      }

      nextRowStart += rowCount;
    }
  }

  void clearVisibilityMap() {
    for (auto widget : visibleWidgets) {
      widget->hide();
    }
    visibleWidgets.clear();
  }

  void updateViewport() {
    for (auto widget : widgetCache) {
      widget->hide();
    }

    int scrollHeight = scrollBar->value();
    int startIndex = 0;

    while (startIndex < m_virtual_items.size()) {
      auto item = m_virtual_items[startIndex];

      if (item.offset + item.height >= scrollHeight) break;
      ++startIndex;
    }

    if (startIndex >= m_virtual_items.size()) return;

    int height = scrollHeight == 0 ? margins.top : m_virtual_items[startIndex].offset - scrollHeight;
    int endIndex = startIndex;

    int rowOffset = scrollHeight == 0 ? 0 : m_virtual_items[startIndex].offset;

    for (; endIndex < m_virtual_items.size(); ++endIndex) {
      auto vitem = m_virtual_items[endIndex];
      auto item = vitem.item;
      auto widget = widgetCache.value(vitem.key);
      int key = vitem.key;

      if (rowOffset < vitem.offset) {
        height += vitem.offset - rowOffset;
        rowOffset = vitem.offset;
      }

      if (!widget) {
        widget = new GridItemWrapper(this);
        widget->setFixedSize({vitem.width, vitem.height});
        widget->setItem(item);
        widgetCache.insert(vitem.key, widget);

      } else {
        widget->setFixedSize({vitem.width, vitem.height});
      }

      if (item->selectable()) {
        widget->disconnect();
        connect(widget, &GridItemWrapper::clicked, this, [this, endIndex]() {
          qDebug() << "clicked";
          setSelected(endIndex);
        });
        connect(widget, &GridItemWrapper::doubleClicked, this, [this, item]() { emit itemActivated(*item); });
      }

      visibleWidgets.insert(endIndex, widget);
      widget->setSelected(m_selected == endIndex);

      QPoint pos(vitem.widthOffset, height);

      if (pos != widget->pos()) { widget->move(pos); }
      if (!widget->isVisible()) widget->show();
      if (height > this->height()) break;
    }

    for (auto idx : visibleWidgets.keys()) {
      if (idx < startIndex || idx > endIndex) {
        auto vitem = m_virtual_items[idx];
        auto widget = visibleWidgets[idx];

        widgetCache.remove(vitem.key);
        visibleWidgets.remove(idx);
        widget->deleteLater();
      }
    }
  }

  void wheelEvent(QWheelEvent *event) override { QApplication::sendEvent(scrollBar, event); }

  int virtualHeight = 0;
  bool resetFlag = false;

public:
  void setColumns(int columns = 1) { this->ncols = columns; }

  VirtualGridSection *section(const QString &key) {
    for (auto &section : m_sections) {
      if (section.name() == key) return &section;
    }

    m_sections << VirtualGridSection(key);

    return &m_sections[m_sections.size() - 1];
  }

  const QList<VirtualGridSection> &sections() { return m_sections; }

  void setMargins(int left, int top, int right, int bottom) {
    margins.left = left;
    margins.top = top;
    margins.right = right;
    margins.bottom = bottom;
  }

  void updateLayout() { calculateLayout(); }

  // todo: fix linear search
  void setItemVisibility(AbstractGridMember *item, bool value) {
    for (auto &li : m_items) {
      if (li.item == item) li.visible = value;
    }
  }

  int indexOfItem(AbstractGridMember *item) {
    for (int i = 0; i != m_virtual_items.size(); ++i) {
      if (m_virtual_items[i].item == item) return i;
    }

    return -1;
  }

  const std::vector<ListItemInfo> &items() { return m_items; }

  void addItem(AbstractGridMember *item, const QString &section = "") {
    m_items.push_back({.item = item, .visible = true});
  }

  QHash<QString, GridSectionLabel *> sectionLabelMap;

  bool removeSection(const QString &name) {
    for (int i = 0; i != m_sections.size(); ++i) {
      if (m_sections[i].name() == name) {
        sectionLabelMap.value(name)->deleteLater();
        m_sections.removeAt(i);

        return true;
      }
    }

    return false;
  }

  int totalItemSize() {
    int count = 0;

    for (const auto &section : m_sections) {
      count += section.items().size();
    }

    return count;
  }

  void calculateLayout() {
    auto start = std::chrono::high_resolution_clock::now();

    int offset = 0;
    QHash<int, GridItemWrapper *> updatedCache;
    int size = totalItemSize();

    m_virtual_items.clear();
    m_virtual_items.reserve(size);

    int marginFreeWidth = width() - margins.left - margins.right;

    qDebug() << "total items" << size;

    for (const auto &section : m_sections) {
      int innerGapSpacing = m_spacing * (ncols - 1);
      int usableSpace = marginFreeWidth - innerGapSpacing;
      int idealColumnWidth = usableSpace / ncols;
      int error = usableSpace - idealColumnWidth * ncols;
      auto &items = section.items();
      int maxHeight = 0;

      if (!items.isEmpty() && !section.name().isEmpty()) {
        auto labelItem = sectionLabelMap.value(section.name());

        if (!labelItem) {
          labelItem = new GridSectionLabel(section.name());
          sectionLabelMap.insert(section.name(), labelItem);
        }

        labelItem->setCount(items.size());

        auto key = labelItem->key();

        if (auto widget = widgetCache.value(key)) {
          auto labelWidget = static_cast<GridListSectionHeader *>(widget->unwrap());

          labelWidget->setCount(items.size());
          updatedCache.insert(key, widget);
        }

        int height = labelItem->heightForWidth(width() - margins.left - margins.right);

        VirtualWidget vitem{.key = key,
                            .height = height,
                            .offset = offset,
                            .widthOffset = margins.left,
                            .width = width() - margins.left - margins.right,
                            .item = labelItem};

        m_virtual_items.push_back(std::move(vitem));
        offset += height;
      }

      int widthOffset = margins.left;

      for (int i = 0; i != items.size(); ++i) {
        auto item = items.at(i);
        int colWidth = idealColumnWidth + (error < i % ncols);
        int height = item->heightForWidth(colWidth);
        int key = item->key();

        if (auto widget = widgetCache.value(key)) { updatedCache.insert(key, widget); }

        VirtualWidget vitem{.key = key,
                            .height = height,
                            .offset = offset,
                            .widthOffset = widthOffset,
                            .width = colWidth,
                            .item = item};

        m_virtual_items.push_back(std::move(vitem));
        widthOffset += colWidth + m_spacing;
        maxHeight = std::max(height, maxHeight);

        if ((i + 1) % ncols == 0) {
          offset += maxHeight + m_spacing;
          widthOffset = margins.left;
          maxHeight = 0;
        }
      }

      if (items.size() % ncols) { offset += maxHeight; }
    }

    for (auto key : widgetCache.keys()) {
      if (!updatedCache.value(key)) { widgetCache.value(key)->deleteLater(); }
    }

    /*
for (auto key : updatedCache.keys()) {
  qDebug() << "cached" << key;
}
    */

    widgetCache = updatedCache;
    offset += margins.bottom;

    int virtualScrollHeight = std::max(0, offset - height());

    scrollBar->setMaximum(virtualScrollHeight);
    scrollBar->setVisible(virtualScrollHeight > 0);
    clearVisibilityMap();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    qDebug() << "computed layout in" << duration << "ms";

    updateViewport();
  }

  void clear() {
    for (auto widget : sectionLabelMap) {
      widget->deleteLater();
    }

    sectionLabelMap.clear();
    clearVisibilityMap();
    scrollBar->setValue(0);

    m_virtual_items.clear();
    m_sections.clear();
  }

  void setSections(const QList<VirtualGridSection> &sections) {
    clear();
    this->m_sections = sections;
    calculateLayout();

    m_selected = -1;
    setSelected(nextSelectableIndex(0));
  }

  int previousSelectableIndex(int base) {
    for (int i = base; i >= 0; --i) {
      if (m_virtual_items[i].item->selectable()) return i;
    }

    return -1;
  }

  int nextSelectableIndex(int base) {
    for (int i = base; i < m_virtual_items.size(); ++i) {
      if (m_virtual_items[i].item->selectable()) return i;
    }

    return -1;
  }

  AbstractGridItemWidget *itemWidget(AbstractGridMember *item) {
    int idx = indexOfItem(item);

    if (idx == -1) return nullptr;

    int key = m_virtual_items[idx].key;

    if (auto wrapper = widgetCache.value(key)) { return wrapper->unwrap(); }

    return nullptr;
  }

  void setSpacing(int spacing) { m_spacing = spacing; }

  void remove(AbstractGridMember *member) {}

  void setSelected(int index) {
    if (m_selected == index || index < 0 || index >= m_virtual_items.size()) return;

    while (index < m_virtual_items.size() && !m_virtual_items[index].item->selectable()) {
      ++index;
    }

    if (index >= m_virtual_items.size()) return;

    auto item = m_virtual_items[index];
    int scrollHeight = scrollBar->value();

    m_selected = index;

    int previousRowEnd = index;

    while (previousRowEnd >= 0 && m_virtual_items[previousRowEnd].offset == item.offset) {
      --previousRowEnd;
    }

    if (previousRowEnd >= 0) {
      auto previousItem = m_virtual_items[previousRowEnd];

      if (previousItem.item->role() == GridSectionNameRole && previousItem.offset - scrollHeight < 0) {
        scrollBar->setValue(scrollHeight - (scrollHeight - item.offset) -
                            m_virtual_items[previousRowEnd].height);
        updateViewport();
        return;
      }
    }

    if (item.offset + item.height - scrollHeight > height()) {
      scrollBar->setValue(item.offset + item.height - height());
    } else if (item.offset - scrollHeight < 0) {
      scrollBar->setValue(scrollHeight - (scrollHeight - item.offset));
    }

    updateViewport();

    if (m_selection_key != item.key) {
      emit selectionChanged(*item.item);
      m_selection_key = item.key;
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    scrollBar->setFixedSize(margins.right, height());
    scrollBar->move(width() - margins.right, 0);
    calculateLayout();
  }

  VirtualGridWidget() : scrollBar(new OmniScrollBar(this)) {
    setMargins(20, 10, 20, 10);
    scrollBar->hide();
    scrollBar->setMinimum(0);
    scrollBar->setSingleStep(40);
    connect(scrollBar, &QScrollBar::valueChanged, this, &VirtualGridWidget::updateViewport);
  }

signals:
  void itemActivated(const AbstractGridMember &item);
  void selectionChanged(const AbstractGridMember &item);
};

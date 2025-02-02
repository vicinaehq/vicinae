#pragma once
#include "builtin_icon.hpp"
#include "omni-icon.hpp"
#include <qboxlayout.h>
#include <qhash.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qscrollbar.h>
#include <qtmetamacros.h>
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

class AbstractGridItem {
public:
  virtual QString title() const { return {}; }
  virtual QString subtitle() const { return {}; }
  virtual QWidget *widget() const = 0;
  virtual QString tooltip() const { return {}; }
};

class AbstractIconGridItem : public AbstractGridItem {
  virtual QWidget *widget() const {
    auto icon = new OmniIcon;

    icon->setIcon(iconName(), {38, 38});

    return icon;
  }

  virtual QString iconName() const = 0;
};

class MainWidget : public QWidget {
  Q_OBJECT

  class Tooltip : public QWidget {
    QLabel *label;

    void paintEvent(QPaintEvent *event) override {
      int borderRadius = 10;
      QColor borderColor("#444444");

      QPainter painter(this);

      painter.setRenderHint(QPainter::Antialiasing, true);

      QPainterPath path;
      path.addRoundedRect(rect(), borderRadius, borderRadius);

      painter.setClipPath(path);

      QColor backgroundColor("#171615");

      painter.fillPath(path, backgroundColor);

      // Draw the border
      QPen pen(borderColor, 1); // Border with a thickness of 2
      painter.setPen(pen);
      painter.drawPath(path);
    }

  public:
    Tooltip(QWidget *parent = nullptr) : QWidget(parent), label(new QLabel) {
      setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
      setAttribute(Qt::WA_TranslucentBackground);

      auto layout = new QVBoxLayout;

      layout->addWidget(label);
      setLayout(layout);
    }

    void setText(const QString &s) { label->setText(s); }
    QString text() { return label->text(); }
  };

  QVBoxLayout *layout;
  OmniIcon *icon;
  bool selected;
  bool hovered;
  Tooltip *tooltip;

protected:
  int borderWidth() const { return 3; }

  QColor borderColor() const {
    if (selected) return "#BBBBBB";
    if (hovered) return "#888888";

    return "#202020";
  }

  void paintEvent(QPaintEvent *event) override {
    int borderRadius = 10;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor backgroundColor("#202020");

    painter.fillPath(path, backgroundColor);

    QPen pen(borderColor(), 3);
    painter.setPen(pen);
    painter.drawPath(path);
  }

  void enterEvent(QEnterEvent *event) override { setHovered(true); }
  void leaveEvent(QEvent *event) override { setHovered(false); }

  void mousePressEvent(QMouseEvent *event) override { emit clicked(); }
  void mouseDoubleClickEvent(QMouseEvent *event) override { emit doubleClicked(); }

public:
  MainWidget()
      : layout(new QVBoxLayout), icon(new OmniIcon), selected(false), hovered(false), tooltip(new Tooltip) {
    setLayout(layout);

    tooltip->hide();
  }

  ~MainWidget() { tooltip->deleteLater(); }

  void setTooltipText(const QString &text) { tooltip->setText(text); }

  void showTooltip() {
    const QPoint globalPos = mapToGlobal(QPoint(0, height() + 5));

    tooltip->adjustSize();
    tooltip->move(globalPos);
    tooltip->show();
  }

  void hideTooltip() { tooltip->hide(); }

  void setHovered(bool hovered) {
    this->hovered = hovered;

    if (hovered && !tooltip->text().isEmpty())
      showTooltip();
    else
      hideTooltip();

    update();
  }

  void setSelected(bool selected) {
    this->selected = selected;
    update();
  }
  void setInset(int inset) { layout->setContentsMargins(inset, inset, inset, inset); }
  void setIcon(const QString &name) { icon->setIcon(name, {56, 56}); }
  void setWidget(QWidget *widget) {
    if (layout->count() > 0) {
      auto old = layout->itemAt(0)->widget();

      layout->replaceWidget(old, widget);
      old->deleteLater();
    } else {
      layout->addWidget(widget, 0, Qt::AlignCenter);
    }
  }

signals:
  void clicked();
  void doubleClicked();
};

class GridItemWidget : public QWidget {
  Q_OBJECT

  QVBoxLayout *layout;
  EllidedLabel *titleLabel;
  EllidedLabel *subtitleLabel;
  bool m_selected;

public:
  MainWidget *main;

  GridItemWidget(QWidget *parent = nullptr)
      : QWidget(parent), layout(new QVBoxLayout), main(new MainWidget), titleLabel(new EllidedLabel),
        subtitleLabel(new EllidedLabel), m_selected(false) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(main);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    setLayout(layout);

    connect(main, &MainWidget::clicked, this, &GridItemWidget::clicked);
    connect(main, &MainWidget::doubleClicked, this, &GridItemWidget::doubleClicked);
  }

  void setInset(int inset) { main->setInset(inset); }

  void setSelected(bool selected) { main->setSelected(selected); }

  void setItem(const AbstractGridItem &item) {
    auto title = item.title();
    auto subtitle = item.subtitle();

    if (auto text = item.tooltip(); !text.isEmpty()) { main->setTooltipText(text); }

    titleLabel->setText(title);
    subtitleLabel->setText(subtitle);
    titleLabel->setVisible(!title.isEmpty());
    subtitleLabel->setVisible(!subtitle.isEmpty());
    main->setWidget(item.widget());
  }

  int computeItemHeight(const AbstractGridItem &item) {
    int height = main->height();
    auto fm = fontMetrics();
    auto spacing = layout->spacing();
    auto title = item.title();
    auto subtitle = item.subtitle();

    if (!title.isEmpty()) { height += fm.ascent() + spacing; }
    if (!subtitle.isEmpty()) { height += fm.ascent() + spacing; }

    return height;
  }

signals:
  void clicked();
  void doubleClicked();
};

class VirtualGridWidget : public QWidget {
  Q_OBJECT

  struct VirtualItem {
    int height;
    int offset;
    bool visible;
    AbstractGridItem *item;
  };

  QList<VirtualItem> m_virtual_items;

  int ncols = 6;
  int m_inset = 0;
  int m_spacing = 10;
  int m_padding = 10;
  int m_selected = -1;

  QWidget *viewport;
  QScrollBar *scrollBar;
  QHash<int, GridItemWidget *> visibleWidgets;

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
      setSelected(qMax(0, m_selected - 1));
      break;
    case Qt::Key_Right:
      setSelected(qMin(m_selected + 1, m_virtual_items.size() - 1));
      break;
    case Qt::Key_Up:
      setSelected(qMax(0, m_selected - ncols));
      break;
    case Qt::Key_Down:
      setSelected(qMin(m_selected + ncols, m_virtual_items.size() - 1));
      break;
    case Qt::Key_Return:
      if (m_selected >= 0 && m_selected < m_virtual_items.size())
        emit itemActivated(*m_virtual_items[m_selected].item);
      break;
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    updateViewport();
  }

  int innerTotalSpacing() { return (ncols - 1) * m_spacing; }
  int columnWidth() { return (viewport->width() - innerTotalSpacing() - m_padding * 2) / ncols; }

  void updateViewport() {
    int scrollHeight = scrollBar->value();
    int width = m_padding;
    int colWidth = columnWidth();

    int maxHeight = 0;
    int startIndex = 0;

    while (startIndex < m_virtual_items.size()) {
      auto item = m_virtual_items[startIndex];

      if (item.offset + item.height >= scrollHeight) break;
      ++startIndex;
    }

    int height = scrollHeight == 0 ? m_padding : m_virtual_items[startIndex].offset - scrollHeight;
    int endIndex = startIndex;

    for (; endIndex != m_virtual_items.size(); ++endIndex) {
      auto vitem = m_virtual_items[endIndex];
      auto item = vitem.item;
      auto *widget = visibleWidgets.value(endIndex);

      if (!widget) {
        widget = new GridItemWidget(viewport);
        widget->setInset(m_inset);
        widget->setFixedWidth(colWidth);
        widget->main->setFixedSize(colWidth, colWidth);
        widget->setItem(*item);
        visibleWidgets.insert(endIndex, widget);

        connect(widget, &GridItemWidget::clicked, this, [this, endIndex]() { setSelected(endIndex); });
        connect(widget, &GridItemWidget::doubleClicked, this, [this, item]() { emit itemActivated(*item); });
      }

      widget->setSelected(m_selected == endIndex);
      widget->move(width, height);
      widget->show();

      width += colWidth + m_spacing;
      maxHeight = qMax(maxHeight, vitem.height);

      if (width + colWidth > viewport->width()) {
        height += maxHeight + m_spacing;
        width = m_padding;
        maxHeight = 0;
      }
      if (height > viewport->height()) break;
    }

    for (auto idx : visibleWidgets.keys()) {
      if (idx < startIndex || idx > endIndex) {
        visibleWidgets.value(idx)->deleteLater();
        visibleWidgets.remove(idx);
      }
    }
  }

  bool eventFilter(QObject *watched, QEvent *event) override {
    if (watched == viewport && event->type() == QEvent::Wheel) {
      QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
      QApplication::sendEvent(scrollBar, event);

      return true; // Event is handled, no need to propagate
    }

    return QWidget::eventFilter(watched, event); // Default behavior
  }

  void setupUi() {
    auto layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(viewport);
    layout->addWidget(scrollBar);
    setLayout(layout);
  }

public:
  void setColumns(int columns = 1) { this->ncols = columns; }
  void setInset(int inset = 10) { this->m_inset = inset; }

  void setItems(const QList<AbstractGridItem *> &items) {
    m_selected = 0;

    GridItemWidget widget;
    QList<VirtualItem> virtualItems;

    int offset = 0;
    int colWidth = columnWidth();

    widget.setFixedWidth(colWidth);
    widget.main->setFixedSize(colWidth, colWidth);

    int maxHeight = 0;

    for (int i = 0; i != items.size(); ++i) {
      if (i > 0 && i % ncols == 0) {
        offset += maxHeight + m_spacing;
        maxHeight = 0;
      }

      auto item = items[i];
      int height = widget.computeItemHeight(*item);

      qDebug() << "itemHeight" << height;

      virtualItems << VirtualItem{
          .height = height,
          .offset = offset,
          .visible = true,
          .item = item,
      };
      maxHeight = std::max(height, maxHeight);
    }

    if (items.size() % ncols) { offset += maxHeight + m_spacing; }

    for (const auto &widget : visibleWidgets)
      widget->deleteLater();

    visibleWidgets.clear();

    qDebug() << "offset=" << offset << "height=" << viewport->height();

    int virtualScrollHeight = qMax(0, offset - viewport->height());

    m_virtual_items = virtualItems;

    scrollBar->setVisible(virtualScrollHeight > 0);
    scrollBar->setValue(0);
    scrollBar->setMaximum(virtualScrollHeight);

    QTimer::singleShot(0, [this]() { updateViewport(); });
  }

  void setSelected(int index) {
    if (m_selected == index || index < 0 || index >= m_virtual_items.size()) return;

    auto item = m_virtual_items[index];

    m_selected = index;
    updateViewport();
    emit selectionChanged(*m_virtual_items[index].item);
  }

  VirtualGridWidget() : viewport(new QWidget), scrollBar(new QScrollBar) {
    setupUi();
    installEventFilter(this);
    connect(scrollBar, &QScrollBar::valueChanged, this, &VirtualGridWidget::updateViewport);
  }

signals:
  void itemActivated(const AbstractGridItem &item);
  void selectionChanged(const AbstractGridItem &item);
};

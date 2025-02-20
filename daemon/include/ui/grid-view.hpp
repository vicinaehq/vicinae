#pragma once
#include "app.hpp"
#include "ui/list-view.hpp"
#include "ui/virtual-grid.hpp"
#include "ui/ellided-label.hpp"
#include "view.hpp"
#include <qnamespace.h>
#include <qobject.h>
#include <qwidget.h>

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
  MainWidget() : layout(new QVBoxLayout), selected(false), hovered(false), tooltip(new Tooltip) {
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

class GridItemWidget : public AbstractGridItemWidget {
  QVBoxLayout *layout;
  EllidedLabel *titleLabel;
  EllidedLabel *subtitleLabel;
  bool m_selected;

  void resizeEvent(QResizeEvent *event) override {
    auto size = event->size();

    main->setFixedSize({size.width(), size.width()});
    AbstractGridItemWidget::resizeEvent(event);
  }

public:
  MainWidget *main;

  GridItemWidget(QWidget *parent = nullptr)
      : AbstractGridItemWidget(parent), layout(new QVBoxLayout), main(new MainWidget),
        titleLabel(new EllidedLabel), subtitleLabel(new EllidedLabel), m_selected(false) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(main);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    setLayout(layout);

    connect(main, &MainWidget::clicked, this, &AbstractGridItemWidget::clicked);
    connect(main, &MainWidget::doubleClicked, this, &AbstractGridItemWidget::doubleClicked);
  }

  void selectionChanged(bool selected) override { main->setSelected(selected); }

  void setTitle(const QString &title) {
    titleLabel->setText(title);
    titleLabel->setVisible(!title.isEmpty());
  }

  void setSubtitle(const QString &subtitle) {
    subtitleLabel->setText(subtitle);
    subtitleLabel->setVisible(!subtitle.isEmpty());
  }

  void setTooltipText(const QString &tooltip) { main->setTooltipText(tooltip); }

  void setWidget(QWidget *widget) { main->setWidget(widget); }
  size_t spacing() { return layout->spacing(); }
};

class AbstractGridItem : public AbstractActionnableGridItem {
public:
  virtual QString title() const { return {}; }
  virtual QString subtitle() const { return {}; }

  virtual QWidget *centerWidget() const = 0;

  int heightForWidth(int columnWidth) const override {
    static GridItemWidget ruler;

    auto fm = ruler.fontMetrics();
    auto spacing = ruler.spacing();
    int height = columnWidth;

    if (!title().isEmpty()) { height += fm.ascent() + spacing; }
    if (!subtitle().isEmpty()) { height += fm.ascent() + spacing; }

    return height;
  }

  virtual AbstractGridItemWidget *widget(int columnWidth) const override {
    auto item = new GridItemWidget();

    item->setTitle(title());
    item->setSubtitle(subtitle());
    item->setTooltipText(tooltip());
    item->setWidget(centerWidget());

    return item;
  }

  virtual int updateId() const override { return qHash("grid-item"); }

  virtual bool update(AbstractGridItemWidget *widget, int colWidth) const override {
    auto item = static_cast<GridItemWidget *>(widget);

    if (!item) {
      qDebug() << "wrong widget" << typeid(item).name();
      return false;
    }

    qDebug() << "valid widget" << typeid(item).name();

    item->setTitle(title());
    item->setSubtitle(subtitle());
    item->setTooltipText(tooltip());
    item->setWidget(centerWidget());

    return true;
  }

  virtual QString tooltip() const { return {}; }

  virtual QList<AbstractAction *> createActions() const { return {}; }
};

class GridView : public View {
protected:
  VirtualGridWidget *grid;

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
      QApplication::sendEvent(grid, event);
      return true;
    }

    return View::inputFilter(event);
  }

  void selectionChanged(const AbstractGridMember &member) {
    auto item = static_cast<const AbstractActionnableGridItem *>(&member);

    setSignalActions(item->createActions());
  }

public:
  GridView(AppWindow &app) : View(app), grid(new VirtualGridWidget) {
    connect(grid, &VirtualGridWidget::selectionChanged, this, &GridView::selectionChanged);
    widget = grid;
  }
};

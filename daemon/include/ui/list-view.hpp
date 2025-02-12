#pragma once

#include "common.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/list-model.hpp"
#include "image-viewer.hpp"
#include "markdown-renderer.hpp"
#include "omni-icon.hpp"
#include "ui/action_popover.hpp"
#include "ui/empty-view.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/virtual-grid.hpp"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtpreprocessorsupport.h>
#include <qwidget.h>

class ListItemWidget : public QWidget {
  QWidget *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ListItemWidget(QWidget *image, const QString &name, const QString &category, const QString &kind,
                 QWidget *parent = nullptr)
      : QWidget(parent), icon(image), name(new QLabel), category(new QLabel), kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    mainLayout->setContentsMargins(10, 0, 10, 0);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);

    setLayout(mainLayout);
  }

  QSize sizeHint() const override { return {0, 40}; }
};

class ListItemWidget2 : public QWidget {
  OmniIcon *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ListItemWidget2(const QString &iconDescriptor, const QString &name, const QString &category,
                  const QString &kind, QWidget *parent = nullptr)
      : QWidget(parent), icon(new OmniIcon), name(new QLabel), category(new QLabel), kind(new QLabel) {

    icon->setIcon(iconDescriptor, {25, 25});

    auto mainLayout = new QHBoxLayout();

    mainLayout->setContentsMargins(10, 0, 10, 0);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);

    setLayout(mainLayout);
  }

  QSize sizeHint() const override { return {0, 40}; }
};

class SimpleListWidget : public AbstractGridItemWidget {
  bool isSelected;
  bool isHovered;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

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

  void selectionChanged(bool selected) override {
    this->isSelected = selected;
    update();
  }

  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) { emit clicked(); }
  }

  void mouseDoubleClickEvent(QMouseEvent *event) override {
    if (event->button() == Qt::LeftButton) { emit doubleClicked(); }
  }

  void setHovered(bool hovered) {
    this->isHovered = hovered;
    update();
  }

  void enterEvent(QEnterEvent *event) override {
    Q_UNUSED(event);
    setHovered(true);
  }

  void leaveEvent(QEvent *event) override {
    Q_UNUSED(event);
    setHovered(false);
  }

public:
  SimpleListWidget(QWidget *parent = nullptr)
      : AbstractGridItemWidget(parent), isSelected(false), isHovered(false) {
    setAttribute(Qt::WA_Hover, true);
  }
};

class ListItemWidget3 : public SimpleListWidget {
  OmniIcon *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ListItemWidget3(const QString &iconDescriptor, const QString &name, const QString &category,
                  const QString &kind, QWidget *parent = nullptr)
      : SimpleListWidget(parent), icon(new OmniIcon), name(new QLabel), category(new QLabel),
        kind(new QLabel) {

    icon->setIcon(iconDescriptor, {25, 25});

    auto mainLayout = new QHBoxLayout();

    mainLayout->setContentsMargins(10, 8, 10, 8);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);

    setLayout(mainLayout);
  }
};

class AbstractActionnableGridItem : public AbstractGridMember {
public:
  virtual QList<AbstractAction *> createActions() const { return {}; }
};

class SimpleListGridItem : public AbstractActionnableGridItem {
  QString iconDescriptor;
  QString name;
  QString category;
  QString kind;

  AbstractGridItemWidget *widget(int columnWidth) const override {
    return new ListItemWidget3(iconDescriptor, name, category, kind);
  }

  int heightForWidth(int columnWidth) const override {
    static ListItemWidget3 ruler(":assets/icons/airplane-landing.svg", "", "", "", nullptr);

    return ruler.sizeHint().height();
  }

public:
  SimpleListGridItem(const QString &iconDescriptor, const QString &name, const QString &category,
                     const QString &kind)
      : iconDescriptor(iconDescriptor), name(name), category(category), kind(kind) {}
};

class DetailWidget : public QWidget {
  QVBoxLayout *layout;
  MarkdownView *markdownEditor;
  HorizontalMetadata *metadata;
  HDivider *divider;

public:
  DetailWidget()
      : layout(new QVBoxLayout), markdownEditor(new MarkdownView()), metadata(new HorizontalMetadata()),
        divider(new HDivider) {
    layout->addWidget(markdownEditor, 1);
    layout->addWidget(divider);
    layout->addWidget(metadata);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
  }

  void dispatchModel(const DetailModel &model) {
    markdownEditor->setMarkdown(model.markdown);

    if (model.metadata.children.isEmpty()) {
      divider->hide();
      metadata->hide();
    } else {
      divider->show();
      metadata->show();
    }

    for (const auto &child : model.metadata.children) {
      metadata->addItem(child);
    }
  }
};

class ListSectionHeaderWidget : public QWidget {
public:
  ListSectionHeaderWidget(const ListSectionModel &model) {
    auto layout = new QHBoxLayout();

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(model.title));
    leftLayout->addWidget(new TextLabel(QString::number(model.children.size())));
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(model.subtitle), 0, Qt::AlignRight | Qt::AlignVCenter);

    setLayout(layout);
  }
};

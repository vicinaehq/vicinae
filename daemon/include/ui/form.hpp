#pragma once

#include "ui/virtual-list.hpp"
#include <memory>
#include <qboxlayout.h>
#include <QPainterPath>
#include <qjsonvalue.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>

class FormQLineEdit : public QLineEdit {
public:
  FormQLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) { setProperty("isFormQLineEdit", true); }
};

class FormItemWidget : public QWidget {
  QLabel *nameLabel;
  QLabel *errorLabel;
  QWidget *widget;
  QHBoxLayout *layout;

public:
  void setName(const QString &name) { nameLabel->setText(name); }
  void setError(const QString &error) { errorLabel->setText(error); }

  FormItemWidget(const QString &name = "")
      : nameLabel(new QLabel), errorLabel(new QLabel), widget(new QWidget), layout(new QHBoxLayout) {
    layout->setSpacing(20);
    layout->addWidget(nameLabel, 1, Qt::AlignVCenter | Qt::AlignRight);
    layout->addWidget(widget, 4, Qt::AlignVCenter);
    layout->addWidget(errorLabel, 2, Qt::AlignVCenter);
    setLayout(layout);
  }

  void focus() { widget->setFocus(); }

  void setWidget(QWidget *widget) {
    layout->replaceWidget(this->widget, widget);
    this->widget->deleteLater();
    this->widget = widget;
  }
};

class FormInputWidget : public FormItemWidget {
  Q_OBJECT
  FormQLineEdit *input;

public:
  FormInputWidget(const QString &name) : FormItemWidget(name), input(new FormQLineEdit) {
    setWidget(input);
    connect(input, &QLineEdit::textChanged, this, &FormInputWidget::textChanged);
  }

  QString text() { return input->text(); }
  void setText(const QString &value) { input->setText(value); }
  void setPlaceholderText(const QString &text) { input->setPlaceholderText(text); }

signals:
  void textChanged(const QString &text);
};

class AbstractFormDropdownItem : public AbstractVirtualListItem {
public:
  virtual QIcon icon() const = 0;
  virtual QString displayName() const = 0;
  QWidget *createItem() const override {
    auto iconLabel = new QLabel;
    auto label = new QLabel;
    auto layout = new QHBoxLayout;

    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    iconLabel->setPixmap(icon().pixmap(20, 20));
    label->setText(displayName());

    layout->setSpacing(10);
    layout->addWidget(iconLabel);
    layout->addWidget(label);
    layout->setContentsMargins(10, 0, 10, 0);

    auto widget = new QWidget;

    widget->setLayout(layout);

    return widget;
  }
  int height() const override { return 35; }
};

class FormDropdownModel : public VirtualListModel {
public:
  void addItem(const std::shared_ptr<AbstractFormDropdownItem> &item) { VirtualListModel::addItem(item); }
};

class Popover : public QWidget {
  Q_OBJECT

  void closeEvent(QCloseEvent *event) override { emit closed(); }

public:
  Popover(QWidget *parent = nullptr) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
  }

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

signals:
  void closed();
};

class FormDropdown : public FormItemWidget {
  Q_OBJECT

  int POPOVER_HEIGHT = 300;

protected:
  VirtualListWidget *list;
  FormDropdownModel *listModel;
  FormQLineEdit *inputField;
  QLineEdit *searchField;
  Popover *popover;
  std::shared_ptr<AbstractFormDropdownItem> currentItem = nullptr;

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (obj == searchField) {
      if (event->type() == QEvent::KeyPress) {
        auto key = static_cast<QKeyEvent *>(event)->key();

        if (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Return || key == Qt::Key_Enter) {
          QApplication::sendEvent(list, event);
          return true;
        }
      }
    }

    if (obj == inputField) {
      if (event->type() == QEvent::KeyPress) {
        auto kv = static_cast<QKeyEvent *>(event);

        if (kv->modifiers() == Qt::Modifiers{} && kv->key() == Qt::Key_Return) {
          showPopover();
          return true;
        }
      }

      if (event->type() == QEvent::MouseButtonPress) {
        showPopover();
        return true;
      }
    }

    return false;
  }

public:
  FormDropdown(const QString &name = "")
      : FormItemWidget(name), listModel(new FormDropdownModel), inputField(new FormQLineEdit),
        searchField(new QLineEdit()), popover(new Popover(this)) {
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    popover->setProperty("class", "popover");

    // Main input field
    inputField->setPlaceholderText("Select an item...");
    inputField->setReadOnly(true); // Read-only to behave like a combo box
    layout->addWidget(inputField);

    // Create the popover
    popover->setWindowFlags(Qt::Popup);
    auto *popoverLayout = new QVBoxLayout(popover);
    popoverLayout->setContentsMargins(1, 1, 1, 1);
    popoverLayout->setSpacing(0);

    searchField = new QLineEdit(popover);
    searchField->setContentsMargins(15, 15, 15, 15);
    searchField->setPlaceholderText("Search...");
    popoverLayout->addWidget(searchField);

    list = new VirtualListWidget(popover);
    list->setModel(listModel);
    popoverLayout->addWidget(new HDivider);

    inputField->installEventFilter(this);
    searchField->installEventFilter(this);

    auto listContainerWidget = new QWidget;
    auto listContainerLayout = new QVBoxLayout;

    listContainerLayout->setContentsMargins(10, 10, 10, 10);
    listContainerLayout->addWidget(list);
    listContainerWidget->setLayout(listContainerLayout);

    popoverLayout->addWidget(listContainerWidget);

    connect(searchField, &QLineEdit::textChanged, this, &FormDropdown::textChanged);
    connect(list, &VirtualListWidget::itemActivated, this, &FormDropdown::itemActivated);
    connect(popover, &Popover::closed, this, [this]() { searchField->clear(); });

    auto widget = new QWidget();
    widget->setLayout(layout);

    setWidget(widget);
  }

  ~FormDropdown() {
    qDebug() << "~FormDropdown";
    listModel->deleteLater();
    popover->deleteLater();
  }

  FormDropdownModel *model() { return listModel; }
  const std::shared_ptr<AbstractFormDropdownItem> &value() { return currentItem; }
  void setValue(const std::shared_ptr<AbstractFormDropdownItem> &vitem) {
    list->setSelected(vitem->id());
    auto item = std::static_pointer_cast<AbstractFormDropdownItem>(vitem);

    inputField->setText(item->displayName());

    if (action) inputField->removeAction(action);

    action = inputField->addAction(item->icon(), QLineEdit::LeadingPosition);
    currentItem = item;
  }

  QString searchText() { return searchField->text(); }

signals:
  void textChanged(const QString &s);
  void selectionChanged(const std::shared_ptr<AbstractFormDropdownItem> &item);

protected:
  QAction *action = nullptr;

private slots:
  void itemActivated(const std::shared_ptr<AbstractVirtualListItem> &vitem) {
    auto item = std::static_pointer_cast<AbstractFormDropdownItem>(vitem);

    inputField->setText(item->displayName());

    if (action) inputField->removeAction(action);

    action = inputField->addAction(item->icon(), QLineEdit::LeadingPosition);

    currentItem = item;

    searchField->clear();
    popover->hide();
    emit selectionChanged(item);
  }

  void showPopover() {
    const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

    if (currentItem) {
      list->setSelected(currentItem->id());
      qDebug() << "selecting " << currentItem->id();
    } else {
      qDebug() << "no current item";
    }

    popover->move(globalPos);
    popover->resize(inputField->width(), POPOVER_HEIGHT);
    popover->show();
    searchField->setFocus();
  }
};

class FormWidget : public QWidget {
public:
  QVBoxLayout *layout;

  FormWidget() : layout(new QVBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    setLayout(layout);
  }

  void addInput(QWidget *input) { layout->addWidget(input); }
};

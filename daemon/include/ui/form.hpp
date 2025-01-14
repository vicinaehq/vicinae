#pragma once

#include "ui/virtual-list.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class FormItemWidget : public QWidget {
  QString m_name;
  QString m_error;

public:
  const QString &name() { return m_name; }
  const QString &error() { return m_error; }
  void setName(const QString &name) { this->m_name = name; }
  void setError(const QString &error) { this->m_error = error; }

  FormItemWidget(const QString &name = "") : m_name(name) {}
};

class FormInputWidget : public QWidget {
  Q_OBJECT

  QLabel *label;
  QLineEdit *input;
  QLabel *error;
  virtual bool validate() { return true; };

public:
  FormInputWidget(const QString &id) : label(new QLabel), input(new QLineEdit), error(new QLabel) {
    connect(input, &QLineEdit::textChanged, this, &FormInputWidget::textChanged);

    auto layout = new QHBoxLayout();

    layout->setSpacing(20);
    layout->addWidget(label, 1, Qt::AlignVCenter | Qt::AlignRight);
    layout->addWidget(input, 4, Qt::AlignVCenter);
    layout->addWidget(error, 2, Qt::AlignVCenter);

    setLayout(layout);
  }

  QString text() { return input->text(); }
  void setText(const QString &value) {}
  void setName(const QString &name) { label->setText(name); }
  void setError(const QString &error) {}

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

    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    iconLabel->setPixmap(icon().pixmap(25, 25));
    label->setText(displayName());

    layout->addWidget(iconLabel);
    layout->addWidget(label);
    layout->setContentsMargins(0, 0, 0, 0);

    auto widget = new QWidget;

    widget->setLayout(layout);

    return widget;
  }
  int height() const override { return 30; }
};

class FormDropdownModel : public VirtualListModel {
public:
  void addItem(const std::shared_ptr<AbstractFormDropdownItem> &item) { VirtualListModel::addItem(item); }
};

class FormDropdown : public QWidget {
  Q_OBJECT

protected:
  VirtualListWidget *list;
  FormDropdownModel *listModel;
  QLineEdit *inputField;
  QLineEdit *searchField;
  QWidget *popover;
  QString selectedItem;
  QStringList items;
  std::shared_ptr<AbstractFormDropdownItem> currentItem = nullptr;

public:
  FormDropdown()
      : listModel(new FormDropdownModel), inputField(new QLineEdit), searchField(new QLineEdit()),
        popover(new QWidget(this, Qt::Popup)) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    setProperty("class", "turbobox");

    popover->setProperty("class", "popover");

    // Main input field
    inputField = new QLineEdit(this);
    inputField->setPlaceholderText("Select an item...");
    inputField->setReadOnly(true); // Read-only to behave like a combo box
    layout->addWidget(inputField);

    // Create the popover
    popover->setWindowFlags(Qt::Popup);
    auto *popoverLayout = new QVBoxLayout(popover);
    popoverLayout->setContentsMargins(0, 0, 0, 0);
    popoverLayout->setSpacing(0);

    searchField = new QLineEdit(popover);
    searchField->setContentsMargins(10, 10, 10, 10);
    searchField->setPlaceholderText("Search...");
    popoverLayout->addWidget(searchField);

    list = new VirtualListWidget(popover);
    list->setModel(listModel);
    list->setContentsMargins(10, 10, 10, 10);
    popoverLayout->addWidget(new HDivider);
    popoverLayout->addWidget(list);

    items = {"Apple", "Banana", "Cherry", "Date", "Grape", "Kiwi", "Mango"};

    inputField->installEventFilter(this);
    searchField->installEventFilter(this);

    // connect(inputField, &QLineEdit::focusInEvent, this,
    // &Turbobox::showPopover);

    inputField->setProperty("class", "form-input");

    // connect(inputField, &QLineEdit::returnPressed, this, &FormDropdown::showPopover);
    connect(searchField, &QLineEdit::textChanged, this, &FormDropdown::filterItems);
    connect(searchField, &QLineEdit::textChanged, this, &FormDropdown::textChanged);
    connect(list, &VirtualListWidget::itemActivated, this, &FormDropdown::itemActivated);
  }

  FormDropdownModel *model() { return listModel; }

  const std::shared_ptr<AbstractFormDropdownItem> &value() { return currentItem; }

  void setValue(const std::shared_ptr<AbstractFormDropdownItem> &item) { itemActivated(item); }

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

  QString getSelectedItem() const { return selectedItem; }

signals:
  void itemChanged(const QString &s);
  void textChanged(const QString &s);
  void selectionChanged(const std::shared_ptr<AbstractFormDropdownItem> &item);

protected:
  virtual void filterItems(const QString &text) {}

  QAction *action = nullptr;

private slots:
  void itemActivated(const std::shared_ptr<AbstractVirtualListItem> &vitem) {
    auto item = std::static_pointer_cast<AbstractFormDropdownItem>(vitem);

    inputField->setText(item->displayName());

    if (action) inputField->removeAction(action);

    action = inputField->addAction(item->icon(), QLineEdit::LeadingPosition);

    currentItem = item;

    popover->hide();
    emit selectionChanged(item);
  }

  void showPopover() {
    // Position the popover just below the input field
    const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));
    popover->move(globalPos);
    popover->resize(inputField->width(), 300); // Adjust height as needed
    popover->show();
    searchField->setFocus();
    filterItems("");
  }

  virtual void selectItem(QListWidgetItem *item) {
    // Set selected item in the input field
    selectedItem = item->text();
    inputField->setText(selectedItem);
    popover->hide();
    emit itemChanged(selectedItem);
  }
};

class FormWidget : public QWidget {
  QVBoxLayout *layout;

public:
  FormWidget() : layout(new QVBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop);

    setLayout(layout);
  }

  void addInput(QWidget *input) { layout->addWidget(input); }
};

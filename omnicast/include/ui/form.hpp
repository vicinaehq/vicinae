#pragma once

#include "common.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/omni-list.hpp"
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
#include <qsharedpointer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qwidget.h>

class FormQLineEdit : public QLineEdit {
  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 5;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    painter.fillPath(path, theme.colors.statusBackground);

    // Draw the border
    QPen pen(theme.colors.border, 1); // Border with a thickness of 2
    painter.setPen(pen);
    painter.drawPath(path);

    QLineEdit::paintEvent(event);
  }

public:
  FormQLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) { setContentsMargins(8, 8, 8, 8); }
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
  void selectAll() { input->selectAll(); }
  void setPlaceholderText(const QString &text) { input->setPlaceholderText(text); }

signals:
  void textChanged(const QString &text);
};

class AbstractFormDropdownItem : public AbstractDefaultListItem {
public:
  AbstractFormDropdownItem() {}

  virtual QString icon() const = 0;
  virtual QString displayName() const = 0;
  ItemData data() const override {
    return {.iconUrl = OmniIconUrl::makeSystem(icon()), .name = displayName()};
  }
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
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 10;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath path;
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    painter.fillPath(path, theme.colors.statusBackground);

    // Draw the border
    QPen pen(theme.colors.border, 1); // Border with a thickness of 2
    painter.setPen(pen);
    painter.drawPath(path);
  }

signals:
  void closed();
};

class DefaultFormDropdownFilter : public OmniList::AbstractItemFilter {
  QString query;

  bool matches(const OmniList::AbstractVirtualItem &item) override {
    auto &dropdowmItem = static_cast<const AbstractFormDropdownItem &>(item);

    return dropdowmItem.displayName().contains(query, Qt::CaseInsensitive);
  }

public:
  DefaultFormDropdownFilter(const QString &query) : query(query) {}
};

class FormDropdown : public FormItemWidget {
  Q_OBJECT

  int POPOVER_HEIGHT = 300;

protected:
  OmniList *list;
  FormQLineEdit *inputField;
  QLineEdit *searchField;
  Popover *popover;
  QString selectedId;

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

  using UpdateItemCallback = std::function<void(AbstractFormDropdownItem *item)>;

  struct UpdateItemPayload {
    QString icon;
    QString displayName;
  };

public:
  FormDropdown(const QString &name = "")
      : FormItemWidget(name), list(new OmniList), inputField(new FormQLineEdit), searchField(new QLineEdit()),
        popover(new Popover(this)) {
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

    popoverLayout->addWidget(new HDivider);

    inputField->installEventFilter(this);
    searchField->installEventFilter(this);

    auto listContainerWidget = new QWidget;
    auto listContainerLayout = new QVBoxLayout;

    listContainerLayout->setContentsMargins(0, 0, 0, 0);
    listContainerLayout->addWidget(list);
    listContainerWidget->setLayout(listContainerLayout);

    popoverLayout->addWidget(listContainerWidget);

    connect(searchField, &QLineEdit::textChanged, this, &FormDropdown::handleTextChanged);
    connect(list, &OmniList::itemActivated, this, &FormDropdown::itemActivated);
    connect(list, &OmniList::itemUpdated, this, &FormDropdown::itemUpdated);
    connect(popover, &Popover::closed, this, [this]() { searchField->clear(); });

    auto widget = new QWidget();
    widget->setLayout(layout);

    setWidget(widget);
  }

  ~FormDropdown() {
    qDebug() << "~FormDropdown2";
    popover->deleteLater();
  }

  void beginUpdate() { list->beginUpdate(); }

  void commitUpdate() { list->commitUpdate(); }

  void clear() {
    inputField->clear();
    inputField->removeAction(action);
    list->clear();
    selectedId.clear();
  }

  void addItem(std::unique_ptr<AbstractFormDropdownItem> item) { list->addItem(std::move(item)); }

  void addSection(const QString &name) { list->addSection(name); }

  void updateItem(const QString &id, const UpdateItemCallback &cb) {
    list->updateItem(id, [&cb](OmniList::AbstractVirtualItem *item) {
      cb(static_cast<AbstractFormDropdownItem *>(item));
    });
  }

  const AbstractFormDropdownItem *value() const {
    if (auto selected = list->itemAt(selectedId); selected) {
      return static_cast<const AbstractFormDropdownItem *>(selected);
    }

    return nullptr;
  }

  void setValue(const QString &id) {
    auto selectedItem = list->setSelected(id);

    if (!selectedItem) {
      qDebug() << "selectValue: no item with ID:" << id;
      return;
    }

    selectedId = id;

    auto item = static_cast<const AbstractFormDropdownItem *>(selectedItem);

    inputField->setText(item->displayName());

    auto icon = new OmniIcon;

    connect(icon, &OmniIcon::imageUpdated, this, [this, icon](QPixmap pixmap) {
      if (action) inputField->removeAction(action);
      action = inputField->addAction(QIcon(pixmap), QLineEdit::LeadingPosition);
      icon->deleteLater();
    });

    QTimer::singleShot(0,
                       [icon, item]() { icon->setIcon(item->icon(), {32, 32}, "application-x-executable"); });
  }

  QString searchText() { return searchField->text(); }

signals:
  void textChanged(const QString &s);
  void selectionChanged(const AbstractFormDropdownItem &item);

protected:
  QAction *action = nullptr;

private slots:
  void handleTextChanged(const QString &text) {
    list->setFilter(std::make_unique<DefaultFormDropdownFilter>(text));
    emit textChanged(text);
  }

  void itemActivated(const OmniList::AbstractVirtualItem &vitem) {
    setValue(vitem.id());
    searchField->clear();
    popover->hide();
    emit selectionChanged(static_cast<const AbstractFormDropdownItem &>(vitem));
  }

  void itemUpdated(const OmniList::AbstractVirtualItem &item) {
    qDebug() << "updated" << item.id() << "vs" << selectedId;
    if (item.id() == selectedId) setValue(item.id());
  }

  void showPopover() {
    const QPoint globalPos = inputField->mapToGlobal(QPoint(0, inputField->height() + 10));

    if (!selectedId.isEmpty()) {
      list->setSelected(selectedId);
    } else {
      list->selectFirst();
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

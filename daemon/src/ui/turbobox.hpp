#include "common.hpp"
#include "omnicast.hpp"
#include "xdg-desktop-database.hpp"
#include <QEvent>
#include <qaction.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class Turbobox : public QWidget {
  Q_OBJECT

protected:
  QListWidget *listWidget;
  QLineEdit *inputField;
  QLineEdit *searchField;
  QWidget *popover;
  QString selectedItem;
  QStringList items;

public:
  Turbobox()
      : inputField(new QLineEdit), searchField(new QLineEdit()),
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

    listWidget = new QListWidget(popover);
    listWidget->setContentsMargins(10, 10, 10, 10);
    listWidget->setFocusPolicy(Qt::NoFocus);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popoverLayout->addWidget(new HDivider);
    popoverLayout->addWidget(listWidget);

    items = {"Apple", "Banana", "Cherry", "Date", "Grape", "Kiwi", "Mango"};

    inputField->installEventFilter(this);
    searchField->installEventFilter(this);

    // connect(inputField, &QLineEdit::focusInEvent, this,
    // &Turbobox::showPopover);

    inputField->setProperty("class", "form-input");

    connect(inputField, &QLineEdit::returnPressed, this,
            &Turbobox::showPopover);
    connect(searchField, &QLineEdit::textChanged, this, &Turbobox::filterItems);
    connect(listWidget, &QListWidget::itemActivated, this,
            &Turbobox::selectItem);
  }

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (obj == searchField) {
      if (event->type() == QEvent::KeyPress) {
        auto key = static_cast<QKeyEvent *>(event)->key();

        if (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Return ||
            key == Qt::Key_Enter) {
          QApplication::sendEvent(listWidget, event);
          return true;
        }
      }
    }

    if (obj == inputField) {
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

protected:
  virtual void filterItems(const QString &text) {
    // Filter items in the list
    listWidget->clear();
    for (const auto &item : items) {
      if (item.contains(text, Qt::CaseInsensitive)) {
        listWidget->addItem(item);
      }
    }

    if (listWidget->count() > 0) {
      listWidget->setCurrentRow(0);
    }
  }

private slots:
  void showPopover() {
    // Position the popover just below the input field
    const QPoint globalPos =
        inputField->mapToGlobal(QPoint(0, inputField->height() + 10));
    popover->move(globalPos);
    popover->resize(inputField->width(), 400); // Adjust height as needed
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

class AppTurbobox : public Turbobox {
  Service<XdgDesktopDatabase> xdd;

  struct DesktopItemWidget : public QWidget {
    App app;

  public:
    DesktopItemWidget(App app) : app(app) {
      auto icon = QIcon::fromTheme(app.icon.c_str());
      auto layout = new QHBoxLayout();
      auto iconLabel = new QLabel();

      layout->setAlignment(Qt::AlignLeft);

      iconLabel->setPixmap(icon.pixmap(18, 18));

      layout->setSpacing(10);
      layout->addWidget(iconLabel);
      layout->addWidget(new QLabel(app.name.c_str()));

      setLayout(layout);
    }
  };

public:
  QAction *action = nullptr;

  AppTurbobox(Service<XdgDesktopDatabase> xdd) : xdd(xdd) {
    inputField->setText("Chromium");
  }

  void filterItems(const QString &text) override {
    listWidget->clear();
    for (const auto &app : xdd->query(text.toLatin1().data())) {
      if (QString::fromStdString(app.normalizedName)
              .contains(text, Qt::CaseInsensitive)) {
        auto item = new QListWidgetItem();

        listWidget->addItem(item);
        auto widget = new DesktopItemWidget(app);
        listWidget->setItemWidget(item, widget);
        item->setSizeHint(widget->sizeHint());
      }
    }

    if (listWidget->count() > 0) {
      listWidget->setCurrentRow(0);
    }
  }

  void selectItem(QListWidgetItem *item) override {
    auto widget =
        static_cast<DesktopItemWidget *>(listWidget->itemWidget(item));

    inputField->setText(widget->app.name.c_str());

    auto label = new QLabel();

    auto icon = QIcon::fromTheme(widget->app.icon.c_str());

    if (action)
      inputField->removeAction(action);

    action = inputField->addAction(icon, QLineEdit::LeadingPosition);

    popover->hide();
  }
};

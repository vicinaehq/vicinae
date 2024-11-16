#include "command-database.hpp"
#include "xdg-desktop-database.hpp"
#include <QPainter>
#include <qboxlayout.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class GenericListItem : public QWidget {
  QLabel *iconLabel;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  GenericListItem(const QString &iconName, const QString &name,
                  const QString &category, const QString &kind,
                  QWidget *parent = nullptr)
      : QWidget(parent), iconLabel(new QLabel), name(new QLabel),
        category(new QLabel), kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    setLayout(mainLayout);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    // if (!iconName.isEmpty() && iconName.at(0) == ':') {
    //  todo
    // this->iconLabel->setPixmap(QIcon::fromTheme("window").pixmap(25, 25));
    //} else {

    auto icon = QIcon::fromTheme(iconName);

    if (icon.isNull())
      icon = QIcon::fromTheme("desktop");

    this->iconLabel->setPixmap(icon.pixmap(25, 25));

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->iconLabel);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);

    qDebug() << "Created GenericListItem";
  }
};

class CategoryNameListItem : public QListWidgetItem {
public:
  CategoryNameListItem(QListWidget *list) : QListWidgetItem(list) {
    auto widget = new QLabel("Results");

    setFlags(flags() & !Qt::ItemIsSelectable);
    widget->setProperty("class", "minor category-name");

    list->addItem(this);
    list->setItemWidget(this, widget);
    setSizeHint(widget->sizeHint());
  }
};

class VStack : public QWidget {
public:
  VStack(QWidget *left, QWidget *right) {
    auto layout = new QVBoxLayout();

    // setStyleSheet("background-color: green");

    layout->setSpacing(10);
    layout->addWidget(left, 1, Qt::AlignCenter);
    layout->addWidget(right, 1, Qt::AlignBottom | Qt::AlignCenter);

    // right->setStyleSheet("background-color: red");

    setLayout(layout);
  }
};

class Chip : public QLabel {
public:
  Chip(const QString &s) {
    setText(s);
    setContentsMargins(10, 5, 10, 5);
    setProperty("class", "chip");
  }
};

class ColorCircle : public QWidget {
  QString s;
  QSize size;

protected:
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    std::cout << "painter" << std::endl;

    painter.setPen(Qt::NoPen);

    int w = width();
    int h = height();

    painter.setBrush(QColor("#BBBBBB"));
    painter.drawEllipse(0, 0, w, h);
    painter.setBrush(QColor(s));
    painter.drawEllipse(3, 3, w - 6, h - 6);
  }

  QSize sizeHint() const override { return size; }

public:
  ColorCircle(const QString &color, QSize size, QWidget *parent = nullptr)
      : QWidget(parent), s(color), size(size) {
    setFixedSize(size);
    qDebug() << "constructed ColorCircle";
  }
};

class TransformResult : public QWidget {

protected:
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    int w = width();
    int h = height();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#666666"));
    painter.drawRect(w / 2, 0, 1, h / 2 - 20);
    painter.drawRect(w / 2, h / 2 + 20, 1, h / 2 - 20);

    std::cout << "HI PAINT" << std::endl;
  }

public:
  TransformResult(QWidget *posLeft, QWidget *posRight) {
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(posLeft, 1);
    layout->addWidget(posRight, 1);
    setLayout(layout);
  }
};

class StatusBar : public QWidget {
public:
  StatusBar(QWidget *parent = nullptr) : QWidget(parent) {
    auto layout = new QHBoxLayout();

    setProperty("class", "status-bar");

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    left->setLayout(leftLayout);
    auto leftIcon = new QLabel();
    QIcon::setThemeName("Papirus-Dark");
    leftIcon->setPixmap(QIcon::fromTheme("desktop").pixmap(20, 20));
    leftLayout->addWidget(leftIcon);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto right = new QWidget();
    auto rightLayout = new QHBoxLayout();

    right->setLayout(rightLayout);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addWidget(new QLabel("Open Command"));
    rightLayout->addWidget(new QLabel("Actions"));

    layout->addWidget(left, 0, Qt::AlignLeft);
    layout->addWidget(right, 0, Qt::AlignRight);

    setLayout(layout);
  }
};

class IndexCommand : public QWidget {
  std::unique_ptr<XdgDesktopDatabase> xdg;
  std::unique_ptr<CommandDatabase> cmdDb;
  QList<Command> usableWithCommands;
  QLineEdit *input = nullptr;
  QListWidget *list = nullptr;
  StatusBar *statusBar = nullptr;

private slots:
  void inputTextChanged(const QString &);

public:
  IndexCommand(QWidget *parent = nullptr);
};

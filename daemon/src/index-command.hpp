#include "command-database.hpp"
#include "quicklist-database.hpp"
#include "xdg-desktop-database.hpp"
#include <QPainter>
#include <numbers>
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qicon.h>
#include <qitemselectionmodel.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qmap.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <variant>

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

class InlineQLineEdit : public QLineEdit {
  Q_OBJECT

private:
  void resizeFromText(const QString &s) {
    auto fm = fontMetrics();

    setFixedWidth(fm.boundingRect(s).width() + 15);
  }

public:
  InlineQLineEdit(const QString &placeholder, QWidget *parent = nullptr)
      : QLineEdit(parent) {
    connect(this, &QLineEdit::textChanged, this, &InlineQLineEdit::textChanged);

    setPlaceholderText(placeholder);
    resizeFromText(placeholder + "...");
    setTextMargins(5, 5, 0, 5);
  }

protected slots:
  void textChanged(const QString &s) {
    const QString &text = s.isEmpty() ? placeholderText() : s;

    resizeFromText(text);
  }
};

class QuicklinkCompleter : public QWidget {

public:
  QList<InlineQLineEdit *> inputs;

  QuicklinkCompleter(const Quicklink &link, QWidget *parent = nullptr)
      : QWidget(parent) {
    auto mainContainer = new QHBoxLayout();

    QIcon::setThemeName("Papirus-Dark");

    setProperty("class", "quicklink-completion");

    // mainContainer->setAlignment(Qt::AlignVCenter);
    mainContainer->setContentsMargins(0, 0, 0, 0);
    mainContainer->setSpacing(10);

    auto iconLabel = new QLabel();

    iconLabel->setPixmap(QIcon::fromTheme(link.iconName).pixmap(22, 22));
    mainContainer->addWidget(iconLabel, 0);

    for (const auto &placeholder : link.placeholders) {
      auto input = new InlineQLineEdit(placeholder);

      inputs.push_back(input);
      mainContainer->addWidget(input, 1, Qt::AlignLeft);
    }

    setLayout(mainContainer);
  }

  QList<QString> collectArgs() {
    QList<QString> ss;

    for (const auto &input : inputs)
      ss.push_back(input->text());

    return ss;
  }

  // focus first empty placeholder
  // returns true if something was focused, false otherwise
  bool focusFirstEmpty() {
    for (auto &input : inputs) {
      if (input->text().isEmpty()) {
        std::cout << "found empty input" << std::endl;
        emit input->setFocus();
        return true;
      }
    }

    return false;
  }
};

struct TopBar : QWidget {
  QHBoxLayout *layout;
  QLineEdit *input;
  QuicklinkCompleter *quickInput = nullptr;

public:
  TopBar(QWidget *parent = nullptr)
      : QWidget(parent), layout(new QHBoxLayout()), input(new QLineEdit()) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(input);
    layout->setSpacing(0);
    setLayout(layout);
    setProperty("class", "top-bar");
  }

  void destroyQuicklinkCompleter() {
    if (quickInput) {
      layout->removeWidget(quickInput);
      quickInput->deleteLater();
      quickInput = nullptr;
      input->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
  }

  void activateQuicklinkCompleter(const Quicklink &link) {
    destroyQuicklinkCompleter();

    auto completion = new QuicklinkCompleter(link);

    quickInput = completion;
    auto fm = input->fontMetrics();
    input->setFixedWidth(fm.boundingRect(input->text()).width() + 35);
    layout->addWidget(completion, 1);
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

struct CodeToColor {
  QString input;
};

struct Calculator {
  QString expression;
  QString result;
};

using Selectable =
    std::variant<Command, App, Quicklink, CodeToColor, Calculator>;

class ManagedList : public QListWidget {
  Q_OBJECT

  QList<QString> sectionNames;
  QList<Selectable> items;
  QMap<QListWidgetItem *, Selectable> widgetToData;

signals:
  void itemSelected(const Selectable &data);
  void itemActivated(const Selectable &data);

protected slots:
  void selectionChanged(QListWidgetItem *item, QListWidgetItem *previous) {
    if (auto it = widgetToData.find(item); it != widgetToData.end()) {
      emit itemSelected(*it);
    }
  }

  void itemActivate(QListWidgetItem *item) {
    if (auto it = widgetToData.find(item); it != widgetToData.end()) {
      emit itemActivated(*it);
    }
  }

public:
  ManagedList(QWidget *parent = nullptr) : QListWidget(parent) {
    connect(this, &QListWidget::currentItemChanged, this,
            &ManagedList::selectionChanged);
    connect(this, &QListWidget::itemActivated, this,
            &ManagedList::itemActivate);
  }

  void addSection(const QString &name) {
    auto item = new QListWidgetItem(this);
    auto widget = new QLabel(name);

    widget->setContentsMargins(8, sectionNames.count() > 0 ? 10 : 0, 0, 10);
    item->setFlags(item->flags() & !Qt::ItemIsSelectable);
    widget->setProperty("class", "minor category-name");

    addItem(item);
    setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
    sectionNames.push_back(name);
  }

  void addWidgetItem(const Selectable &data, QWidget *widget) {
    auto item = new QListWidgetItem(this);

    addItem(item);
    setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
    widgetToData.insert(item, data);
  }
};

class IndexCommand : public QWidget {
  Q_OBJECT

  std::unique_ptr<XdgDesktopDatabase> xdg;
  std::unique_ptr<CommandDatabase> cmdDb;
  std::unique_ptr<QuicklistDatabase> quicklinkDb;
  QList<Command> usableWithCommands;
  TopBar *topBar = nullptr;
  ManagedList *list = nullptr;
  StatusBar *statusBar = nullptr;
  QList<Selectable> selectables;

private slots:
  void inputTextChanged(const QString &);
  bool eventFilter(QObject *obj, QEvent *event) override;
  void itemSelected(const Selectable &item);
  void itemActivated(const Selectable &item);

public:
  IndexCommand(QWidget *parent = nullptr);
};

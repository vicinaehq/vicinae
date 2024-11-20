#pragma once
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPainter>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <qtmetamacros.h>
#include <qwidget.h>

static void xdgOpen(const QString &url) {
  QProcess process;

  process.startDetached("xdg-open", QStringList() << url);
}

class IAction {
public:
  virtual QString name() const = 0;
  virtual void exec(const QList<QString> cmd) const = 0;
};

class IActionnable {
protected:
  using ActionList = QList<std::shared_ptr<IAction>>;

public:
  virtual ActionList generateActions() const = 0;
};

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

class VStack : public QWidget {
public:
  VStack(QWidget *left, QWidget *right) {
    auto layout = new QVBoxLayout();

    layout->setSpacing(10);
    layout->addWidget(left, 1, Qt::AlignCenter);
    layout->addWidget(right, 1, Qt::AlignBottom | Qt::AlignCenter);
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

class InputCompleter : public QWidget {

public:
  QList<InlineQLineEdit *> inputs;
  QLabel *iconLabel;

  InputCompleter(const QList<QString> &placeholders, QWidget *parent = nullptr)
      : QWidget(parent) {
    auto mainContainer = new QHBoxLayout();

    QIcon::setThemeName("Papirus-Dark");

    setProperty("class", "quicklink-completion");

    // mainContainer->setAlignment(Qt::AlignVCenter);
    mainContainer->setContentsMargins(0, 0, 0, 0);
    mainContainer->setSpacing(10);

    iconLabel = new QLabel();

    mainContainer->addWidget(iconLabel, 0);

    for (const auto &placeholder : placeholders) {
      auto input = new InlineQLineEdit(placeholder);

      inputs.push_back(input);
      mainContainer->addWidget(input, 1, Qt::AlignLeft);
    }

    setLayout(mainContainer);
  }

  void setIcon(const QString &iconName) {
    iconLabel->setPixmap(QIcon::fromTheme(iconName).pixmap(22, 22));
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
        emit input->setFocus();
        return true;
      }
    }

    return false;
  }
};

struct TopBar : QWidget {
  QLabel *backButtonLabel = nullptr;
  QHBoxLayout *layout;
  QLineEdit *input;
  InputCompleter *quickInput = nullptr;
  QWidget *backWidget = nullptr;

public:
  TopBar(QWidget *parent = nullptr)
      : QWidget(parent), layout(new QHBoxLayout()), input(new QLineEdit()) {
    backButtonLabel = new QLabel();

    QIcon::setThemeName("Papirus-Dark");

    input->setTextMargins(10, 10, 10, 10);

    backWidget = new QWidget();
    auto backContainer = new QVBoxLayout();

    backContainer->setContentsMargins(0, 0, 0, 0);
    backContainer->addWidget(backButtonLabel, 0, Qt::AlignCenter);
    backWidget->setLayout(backContainer);

    backButtonLabel->setPixmap(QIcon::fromTheme("arrow-left").pixmap(20, 20));

    backButtonLabel->setProperty("class", "back-button");
    backWidget->hide();

    layout->addWidget(backWidget);
    layout->addWidget(input);
    layout->setSpacing(0);
    setLayout(layout);
    setProperty("class", "top-bar");
  }

  void showBackButton() { backWidget->show(); }

  void hideBackButton() { backWidget->hide(); }

  void destroyQuicklinkCompleter() {
    if (quickInput) {
      layout->removeWidget(quickInput);
      quickInput->deleteLater();
      quickInput = nullptr;
      input->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
  }

  void activateQuicklinkCompleter(const QList<QString> &placeholders) {
    destroyQuicklinkCompleter();

    auto completion = new InputCompleter(placeholders);

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

class ManagedList : public QListWidget {
  Q_OBJECT

  QList<QString> sectionNames;
  QList<IActionnable *> items;
  QMap<QListWidgetItem *, IActionnable *> widgetToData;

signals:
  void itemSelected(const IActionnable &data);
  void itemActivated(const IActionnable &data);

protected slots:
  void selectionChanged(QListWidgetItem *item, QListWidgetItem *previous) {
    if (auto it = widgetToData.find(item); it != widgetToData.end()) {
      emit itemSelected(**it);
    }
  }

  void itemActivate(QListWidgetItem *item) {
    if (auto it = widgetToData.find(item); it != widgetToData.end()) {
      emit itemActivated(**it);
    }
  }

public:
  ManagedList(QWidget *parent = nullptr) : QListWidget(parent) {
    connect(this, &QListWidget::currentItemChanged, this,
            &ManagedList::selectionChanged);
    connect(this, &QListWidget::itemActivated, this,
            &ManagedList::itemActivate);
  }

  void clear() {
    for (const auto &item : items) {
      delete item;
    }

    items.clear();
    widgetToData.clear();
    QListWidget::clear();
  }

  void selectFirstEligible() {
    for (int i = 0; i != count(); ++i) {
      auto item = QListWidget::item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      QListWidget::setCurrentItem(item);
      break;
    }
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

  void addWidgetItem(IActionnable *data, QWidget *widget) {
    auto item = new QListWidgetItem(this);

    addItem(item);
    setItemWidget(item, widget);
    item->setSizeHint(widget->sizeHint());
    widgetToData.insert(item, data);
  }
};

class StatusBar : public QWidget {
  QLabel *selectedActionLabel;

public:
  void setSelectedAction(const std::shared_ptr<IAction> &action) {
    selectedActionLabel->setText(action->name());
  }

  StatusBar(QWidget *parent = nullptr) : QWidget(parent) {
    auto layout = new QHBoxLayout();

    setProperty("class", "status-bar");

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    left->setLayout(leftLayout);
    auto leftIcon = new QLabel();
    QIcon::setThemeName("Papirus-Dark");
    leftIcon->setPixmap(
        QIcon::fromTheme(":/assets/icons/tux.svg").pixmap(22, 22));
    leftLayout->addWidget(leftIcon);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto right = new QWidget();
    auto rightLayout = new QHBoxLayout();

    right->setLayout(rightLayout);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    selectedActionLabel = new QLabel();

    rightLayout->addWidget(selectedActionLabel);
    rightLayout->addWidget(new QLabel("Actions"));

    layout->addWidget(left, 0, Qt::AlignLeft);
    layout->addWidget(right, 0, Qt::AlignRight);

    setLayout(layout);
  }
};

class Command;
class CommandWidget;

class AppWindow : public QMainWindow {
  Q_OBJECT

public:
  CommandWidget *baseCommand = nullptr;
  TopBar *topBar = nullptr;
  StatusBar *statusBar = nullptr;
  CommandWidget *command = nullptr;
  QVBoxLayout *layout = nullptr;
  std::optional<const Command *> currentCommand = std::nullopt;

  AppWindow(QWidget *parent = 0);

  void resetCommand();
  void setCommandWidget(CommandWidget *cmd);

public slots:
  void setCommand(const Command *command);
  bool eventFilter(QObject *obj, QEvent *event) override;
};

class CommandWidget : public QWidget {
  Q_OBJECT

protected:
  AppWindow *app = nullptr;

  void createCompletion(const QList<QString> &inputs) {
    app->topBar->activateQuicklinkCompleter(inputs);
  }

  void destroyCompletion() {
    if (app->topBar->quickInput) {
      app->topBar->destroyQuicklinkCompleter();
    }
  }

  QLineEdit *searchbar() { return app->topBar->input; }

public:
  CommandWidget(AppWindow *app) : app(app) { setObjectName("CommandWidget"); }

signals:
  void replaceCommand(const Command *);
};

#include "actions.hpp"
#include "app-database.hpp"
#include "command-object.hpp"
#include "common.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/managed_list.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <unistd.h>

class QuickLinkManagerCommand : public CommandObject {
  Service<QuicklistDatabase> quicklinkDb;
  Service<AppDatabase> appDb;
  ManagedList *list;
  QHBoxLayout *layout;
  SplitView *split;

public:
  class InfoListWidget : public QWidget {
    QVBoxLayout *layout;

  public:
    InfoListWidget() {
      layout = new QVBoxLayout();

      layout->setContentsMargins(10, 10, 10, 10);
      setLayout(layout);
    }

    void addRow(QWidget *left, QWidget *right) {
      auto widget = new QWidget();
      auto rowLayout = new QHBoxLayout();

      if (layout->count() > 0) {
        layout->addWidget(new HDivider);
      }

      rowLayout->setContentsMargins(0, 0, 0, 0);
      rowLayout->addWidget(left, 0, Qt::AlignLeft | Qt::AlignVCenter);
      rowLayout->addWidget(right, 0, Qt::AlignRight | Qt::AlignVCenter);
      widget->setLayout(rowLayout);
      layout->addWidget(widget);
    }
  };

  struct QuicklinkInfoListWidget : public InfoListWidget {
    QLabel *name;
    QLabel *link;
    QLabel *appIcon;
    QLabel *appName;
    QLabel *lastUpdated;
    QLabel *opened;

  public:
    QuicklinkInfoListWidget() {
      auto appLeft = new QWidget();
      auto appLayout = new QHBoxLayout();

      appLayout->setContentsMargins(0, 0, 0, 0);

      appLeft->setLayout(appLayout);

      appIcon = new QLabel();
      appIcon->setPixmap(QIcon::fromTheme("chromium").pixmap(20, 20));
      appName = new QLabel("Chromium");

      appLayout->addWidget(appIcon);
      appLayout->addWidget(appName);

      this->name = new QLabel();
      this->link = new QLabel();
      this->lastUpdated = new QLabel("Never");
      this->opened = new QLabel("12");

      addRow(new QLabel("Name"), this->name);
      addRow(new QLabel("Link"), this->link);
      addRow(new QLabel("Application"), appLeft);
      addRow(new QLabel("Last Updated"), this->lastUpdated);
      addRow(new QLabel("Opened"), this->opened);
    }
  };

  class QuicklinkDetailsWidget : public QWidget {
    QuicklinkInfoListWidget *info;
    Service<AppDatabase> &appDb;

  public:
    QuicklinkDetailsWidget(Service<AppDatabase> &appDb) : appDb(appDb) {
      auto layout = new QVBoxLayout();

      layout->setContentsMargins(0, 0, 0, 0);

      info = new QuicklinkInfoListWidget();

      layout->addWidget(new QWidget(), 1);
      layout->addWidget(new HDivider);
      layout->addWidget(info);

      setLayout(layout);
    }

    void load(const Quicklink &rhs) {
      if (auto app = appDb->getById(rhs.app)) {
        info->appName->setText(app->name);
        info->appIcon->setPixmap(app->icon().pixmap(20, 20));
      } else {
        info->appIcon->setPixmap(QIcon::fromTheme(rhs.iconName).pixmap(20, 20));
        info->appName->setText("-/-");
      }

      info->name->setText(rhs.name);
      info->link->setText(rhs.url);

      if (rhs.lastUsedAt) {
        info->lastUpdated->setText(rhs.lastUsedAt->toString());
      } else {
        info->lastUpdated->setText("Never");
      }

      info->opened->setText(QString::number(rhs.openCount));
    }
  };

  QuicklinkDetailsWidget *details = nullptr;

  QuickLinkManagerCommand(AppWindow *app)
      : CommandObject(app), list(new ManagedList()) {
    quicklinkDb = service<QuicklistDatabase>();
    appDb = service<AppDatabase>();

    forwardInputEvents(list);

    details = new QuicklinkDetailsWidget(appDb);
    list->setStyleSheet("QListWidget { margin-top: 10px; }");
    widget = split = new SplitView(list, details);

    connect(list, &ManagedList::itemSelected, this,
            &QuickLinkManagerCommand::itemSelected);
  }

  void onAttach() override {
    clearSearch();
    setSearchPlaceholder("Browse quicklinks");
  }

  void onSearchChanged(const QString &query) override {
    list->clear();

    QList<const Quicklink *> links;

    for (auto &link : quicklinkDb->list()) {
      if (link->name.toLower().contains(query.toLower()))
        links.push_back(link.get());
    }

    list->addSection(QString("%1 quicklinks").arg(links.size()));

    for (const auto link : links) {
      list->addWidgetItem(new ActionnableQuicklink(*this, *link),
                          new GenericListItem(QIcon::fromTheme(link->iconName),
                                              link->name, "", ""));
    }

    if (links.isEmpty()) {
      destroyCompletion();
      split->collapse();
    } else {
      list->selectFirstEligible();
    }
  }

  void onActionActivated(std::shared_ptr<IAction> action) override {
    action->exec(*this);
  }

public slots:
  void itemSelected(const IActionnable &item) {
    auto action = static_cast<const ActionnableQuicklink &>(item);

    qDebug() << "item selected " << action.link.name;

    if (!action.link.placeholders.isEmpty()) {
      createCompletion(action.link.placeholders, action.link.iconName);
    } else {
      destroyCompletion();
    }

    setActions(item.generateActions());
    details->load(action.link);
    split->expand();
  }

  QString name() override { return "Browse quicklinks"; }
  QIcon icon() override { return QIcon::fromTheme("link"); }
};

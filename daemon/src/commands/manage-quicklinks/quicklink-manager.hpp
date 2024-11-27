#include "command-object.hpp"
#include "common.hpp"
#include "omnicast.hpp"
#include "quicklist-database.hpp"
#include "ui/managed_list.hpp"
#include <qboxlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <unistd.h>

class QuickLinkManagerCommand : public CommandObject {
  std::shared_ptr<QuicklistDatabase> quicklinkDb;
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

  public:
    QuicklinkDetailsWidget() {
      auto layout = new QVBoxLayout();

      layout->setContentsMargins(0, 0, 0, 0);

      info = new QuicklinkInfoListWidget();

      layout->addWidget(new QWidget(), 1);
      layout->addWidget(new HDivider);
      layout->addWidget(info);

      setLayout(layout);
    }

    void load(const Quicklink &rhs) {
      info->name->setText(rhs.name);
      info->link->setText(rhs.url);
      info->appIcon->setPixmap(QIcon::fromTheme("chromium").pixmap(20, 20));
      info->appName->setText("chromium");
      info->lastUpdated->setText("Never");
      info->opened->setText("0");
    }
  };

  QuicklinkDetailsWidget *details = nullptr;

  QuickLinkManagerCommand(AppWindow *app)
      : CommandObject(app), list(new ManagedList()) {
    quicklinkDb = service<QuicklistDatabase>();

    for (const auto &link : quicklinkDb->links) {
      qDebug() << link.name;
    }

    forwardInputEvents(list);

    details = new QuicklinkDetailsWidget();
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

    QList<Quicklink *> links;

    for (auto &link : quicklinkDb->links) {
      if (link.name.toLower().contains(query.toLower()))
        links.push_back(&link);
    }

    list->addSection(QString("%1 quicklinks").arg(links.size()));

    for (const auto link : links) {
      list->addWidgetItem(
          link, new GenericListItem(link->iconName, link->name, "", ""));
    }

    if (links.isEmpty()) {
      destroyCompletion();
      split->collapse();
    } else {
      list->selectFirstEligible();
    }
  }

public slots:
  void itemSelected(const IActionnable &item) {
    const Quicklink &link = static_cast<const Quicklink &>(item);

    qDebug() << "item selected " << link.name;

    createCompletion(link.placeholders, link.iconName);
    details->load(link);
    split->expand();
  }

  QString name() override { return "Browse quicklinks"; }
  QIcon icon() override { return QIcon::fromTheme("link"); }
};

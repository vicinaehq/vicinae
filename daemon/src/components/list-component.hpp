#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "command-object.hpp"
#include "common.hpp"
#include "omnicast.hpp"
#include <QTextBrowser>
#include <QTextEdit>
#include <qboxlayout.h>
#include <qhash.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qstringtokenizer.h>
#include <qwidget.h>
#include <variant>

struct MetadataLabel {
  QString title;
  QString icon;
  QString text;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataSeparator>;

struct Metadata {
  QList<MetadataItem> items;
};

struct Detail {
  QString markdown;
  std::optional<Metadata> metadata;
};

struct BasicListItem {
  QString id;
  QString title;
  QString icon;
  std::optional<Detail> detail;
};

class CustomListItem {
public:
  virtual QWidget *render() = 0;
};

struct ListLabel {
  QString name;
};

using ListDisplayable = std::variant<BasicListItem, ListLabel>;

struct ListComponentProps {
  bool filtering;
  QString navigationTitle;
  QString searchBarPlaceholder;
};

class DetailWidget : public QWidget {
public:
  DetailWidget(const Detail &detail) {
    auto layout = new QVBoxLayout();

    auto view = new QTextBrowser();

    view->setHtml(detail.markdown);
    view->setStyleSheet("QTextEdit { background: transparent; }");

    layout->addWidget(view);
    layout->addWidget(new QLabel("metadataaaa"));

    setLayout(layout);
  }
};

class ListComponent : public CommandObject {
  QHash<QListWidgetItem *, BasicListItem> itemMap;
  QHBoxLayout *layout;
  QListWidget *list;
  VDivider *divider;
  DetailWidget *detail = nullptr;

  void onSearchChanged(const QString &s) override { renderItems(s); }

public slots:
  void currentItemChanged(QListWidgetItem *next, QListWidgetItem *prev) {
    auto nextItem = itemMap[next];

    if (nextItem.detail) {
      auto newDetail = new DetailWidget(*nextItem.detail);

      if (detail) {
        layout->replaceWidget(detail, newDetail);
        detail->deleteLater();
      } else {
        layout->addWidget(newDetail, 2);
      }

      detail = newDetail;

      qDebug() << "Rendering details view";
    }

    qDebug() << "item selection changed";
  }

public:
  ListComponent(AppWindow *app, const ListComponentProps &props)
      : CommandObject(app), layout(new QHBoxLayout()), list(new QListWidget()),
        divider(new VDivider()) {
    if (!props.searchBarPlaceholder.isEmpty()) {
      setSearchPlaceholder(props.searchBarPlaceholder);
    }

    forwardInputEvents(list);

    connect(list, &QListWidget::currentItemChanged, this,
            &ListComponent::currentItemChanged);

    list->setProperty("class", "managed-list");
    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(list, 1);
    layout->addWidget(divider);
    widget->setLayout(layout);
  }

  void renderItems(const QString &q = "") {
    auto items = onSearchTextChanged(q);

    qDebug() << "Rendering new list" << items.size();

    list->clear();
    itemMap.clear();

    for (const auto &item : items) {
      if (auto basic = std::get_if<BasicListItem>(&item)) {
        auto widget = new GenericListItem(QIcon::fromTheme(basic->icon),
                                          basic->title, "", "");
        auto listItem = new QListWidgetItem();

        list->addItem(listItem);
        list->setItemWidget(listItem, widget);
        listItem->setSizeHint(widget->sizeHint());
        itemMap[listItem] = *basic;
      } else if (auto label = std::get_if<ListLabel>(&item)) {
        auto item = new QListWidgetItem();
        auto widget = new QLabel(label->name);

        widget->setProperty("class", "list-label minor category-name");

        widget->setContentsMargins(0, 10, 0, 10);
        item->setFlags(item->flags() & !Qt::ItemIsSelectable);

        list->addItem(item);
        list->setItemWidget(item, widget);
        item->setSizeHint(widget->sizeHint());
      }
    }

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

  virtual QList<ListDisplayable> onSearchTextChanged(const QString &s) = 0;
};

class ListTestImplementation : public ListComponent {
  Service<AppDatabase> appDb;

public:
  ListTestImplementation(AppWindow *app)
      : ListComponent(app, {.searchBarPlaceholder = "test this"}),
        appDb(service<AppDatabase>()) {
    renderItems("");
  }

  BasicListItem createItem(const DesktopEntry &entry) {
    // clang-format off
	  BasicListItem item{
		  .id = entry.id,
		  .title = entry.name,
		  .icon = entry.data.icon,
		  .detail = Detail{
			  .markdown = R"(
<img src="https://files.raycast.com/d6ynnbw8q6hmxcjwoc8167ohsnha" alt="illustration" />

			
			  )",
			  .metadata = Metadata{
				  .items = {
					 MetadataLabel{
						  .title = "Path",
						  .text = entry.path,
					  },
					  MetadataLabel{
						  .title = "Name",
						  .text = entry.name,
					  },
					  MetadataLabel{
						  .title = "Comment",
						  .text = entry.data.comment,
					  }
				  }
			  }
		  }
	  };
    // clang-format on

    return item;
  }

  QList<ListDisplayable> onSearchTextChanged(const QString &s) override {
    QList<ListDisplayable> items;

    qDebug() << "updating list items for" << s;

    items.push_back(ListLabel{"Results"});

    for (const auto &app : appDb.apps) {
      if (app->name.contains(s, Qt::CaseInsensitive)) {
        items.push_back(createItem(*app));
      }
    }

    return items;
  }
};

#pragma once
#include "bookmark-service.hpp"
#include "create-quicklink-command.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include <memory>
#include <qclipboard.h>
#include <qlogging.h>

class OpenBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;
  std::vector<QString> m_arguments;

  void execute(AppWindow &app) override {}

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto appDb = ServiceRegistry::instance()->appDb();
    QString expanded;
    size_t argumentIndex = 0;

    for (const auto &part : m_bookmark->parts()) {
      if (auto s = std::get_if<QString>(&part)) {
        expanded += *s;
      } else if (auto placeholder = std::get_if<Bookmark::ParsedPlaceholder>(&part)) {
        if (placeholder->id == "clipboard") {
          expanded += QApplication::clipboard()->text();
        } else if (placeholder->id == "selected") {
          // TODO: selected text
        } else if (placeholder->id == "uuid") {
          expanded += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
        } else {
          if (argumentIndex < m_arguments.size()) { expanded += m_arguments.at(argumentIndex++); }
        }
      }
    }

    qDebug() << "opening link" << expanded;

    if (auto app = appDb->findById(m_bookmark->app())) { appDb->launch(*app, {expanded}); }

    ui->popToRoot();
    ui->closeWindow();
  }

  QString title() const override { return "Open bookmark"; }

public:
  OpenBookmarkAction(const std::shared_ptr<Bookmark> &bookmark, const std::vector<QString> &arguments)
      : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark), m_arguments(arguments) {}
};

class OpenCompletedBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    OpenBookmarkAction open(m_bookmark, ui->topView()->argumentValues());

    open.execute();
  }

public:
  OpenCompletedBookmarkAction(const std::shared_ptr<Bookmark> &bookmark)
      : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark) {}
};

class OpenBookmarkFromSearchText : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    OpenBookmarkAction open(m_bookmark, {ui->topView()->searchText()});

    open.execute();
  }

public:
  OpenBookmarkFromSearchText(const std::shared_ptr<Bookmark> &bookmark)
      : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark) {}
};

struct EditBookmarkAction : public AbstractAction {
public:
  std::shared_ptr<Bookmark> m_bookmark;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto view = new EditBookmarkView(m_bookmark);
    auto ui = ServiceRegistry::instance()->UI();

    ui->pushView(view);
  }

  EditBookmarkAction(const std::shared_ptr<Bookmark> &bookmark, const QList<QString> &args = {})
      : AbstractAction("Edit bookmark", BuiltinOmniIconUrl("pencil")), m_bookmark(bookmark) {}
};

struct RemoveBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;

  void execute(AppWindow &app) override {}

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto bookmarkDb = ServiceRegistry::instance()->bookmarks();
    bool removeResult = bookmarkDb->removeBookmark(m_bookmark->id());

    if (removeResult) {
      ui->setToast("Removed link");
    } else {
      ui->setToast("Failed to remove link", ToastPriority::Danger);
    }
  }

  RemoveBookmarkAction(const std::shared_ptr<Bookmark> &link)
      : AbstractAction("Remove link", BuiltinOmniIconUrl("trash")), m_bookmark(link) {}
};

struct DuplicateBookmarkAction : public AbstractAction {
public:
  std::shared_ptr<Bookmark> link;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto view = new DuplicateBookmarkView(link);

    emit ui->pushView(view, {.navigation = NavigationStatus{
                                 .title = "Duplicate link",
                                 .iconUrl = BuiltinOmniIconUrl("link").setBackgroundTint(ColorTint::Red)}});
  }

  DuplicateBookmarkAction(const std::shared_ptr<Bookmark> &link)
      : AbstractAction("Duplicate link", BuiltinOmniIconUrl("duplicate")), link(link) {}
};

#pragma once
#include "services/bookmark/bookmark-service.hpp"
#include "create-quicklink-command.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/toast.hpp"
#include <memory>
#include <qclipboard.h>
#include <qlogging.h>
#include <ranges>

class OpenBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;
  std::vector<QString> m_arguments;
  std::shared_ptr<Application> m_app;

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

    if (m_app) {
      appDb->launch(*m_app, {expanded});
    } else if (auto app = appDb->findById(m_bookmark->app())) {
      appDb->launch(*app, {expanded});
    } else {
      ui->setToast("No app with id " + m_bookmark->app(), ToastPriority::Danger);
      return;
    }

    ui->popToRoot();
    ui->closeWindow();
  }

  QString title() const override { return "Open bookmark"; }

public:
  OpenBookmarkAction(const std::shared_ptr<Bookmark> &bookmark, const std::vector<QString> &arguments,
                     const std::shared_ptr<Application> &app = nullptr)
      : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark), m_arguments(arguments),
        m_app(app) {}
};

class OpenCompletedBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;
  std::shared_ptr<Application> m_app;

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    OpenBookmarkAction open(m_bookmark, ui->argumentValues(), m_app);

    open.execute();
  }

  OpenCompletedBookmarkAction(const std::shared_ptr<Bookmark> &bookmark,
                              const std::shared_ptr<Application> &app = nullptr)
      : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark), m_app(app) {}
};

class OpenBookmarkFromSearchText : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;

  void execute(AppWindow &app) override {}

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    OpenBookmarkAction open(m_bookmark, {ui->searchText()});

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
      : AbstractAction("Remove link", BuiltinOmniIconUrl("trash")), m_bookmark(link) {
    setStyle(AbstractAction::Danger);
    setShortcut({.key = "X", .modifiers = {"ctrl", "shift"}});
  }
};

struct DuplicateBookmarkAction : public AbstractAction {
public:
  std::shared_ptr<Bookmark> link;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto view = new DuplicateBookmarkView(link);

    emit ui->pushView(view,
                      {.navigation = NavigationStatus{
                           .title = "Duplicate link",
                           .iconUrl = BuiltinOmniIconUrl("link").setBackgroundTint(SemanticColor::Red)}});
  }

  DuplicateBookmarkAction(const std::shared_ptr<Bookmark> &link)
      : AbstractAction("Duplicate link", BuiltinOmniIconUrl("duplicate")), link(link) {}
};

/**
 * Submenu action to let the user select which app to open the bookmark
 * with. The list of available apps depends on the bookmark url.
 */
class OpenCompletedBookmarkWithAction : public AbstractAction {
  class OpenAction : public AbstractAction {
    std::shared_ptr<Bookmark> m_bookmark;
    std::shared_ptr<Application> m_app;

    void execute() override { OpenCompletedBookmarkAction(m_bookmark, m_app).execute(); }

    QString id() const override { return m_app->id(); }

  public:
    OpenAction(const std::shared_ptr<Bookmark> &bookmark, const std::shared_ptr<Application> &app)
        : AbstractAction(app->name(), app->iconUrl()), m_bookmark(bookmark), m_app(app) {}
  };

  std::shared_ptr<Bookmark> m_bookmark;

  bool isSubmenu() const override { return true; }

  ActionPanelView *createSubmenu() const override {
    auto appDb = ServiceRegistry::instance()->appDb();
    auto apps = appDb->findOpeners(m_bookmark->url());
    auto mapAction = [&](auto &&app) { return new OpenAction(m_bookmark, app); };
    auto panel = new ActionPanelStaticListView();
    bool hasBookmarkAction =
        std::ranges::any_of(apps, [&](auto &&app) { return m_bookmark->app() == app->id(); });

    panel->setTitle("Open with...");

    if (!hasBookmarkAction) {
      if (auto app = appDb->findById(m_bookmark->app())) { panel->addAction(mapAction(app)); }
    }

    std::ranges::for_each(apps | std::views::transform(mapAction),
                          [&](auto &&action) { panel->addAction(action); });

    return panel;
  }

public:
  OpenCompletedBookmarkWithAction(const std::shared_ptr<Bookmark> &bookmark)
      : AbstractAction("Open with...", BuiltinOmniIconUrl("arrow-clockwise")), m_bookmark(bookmark) {}
};

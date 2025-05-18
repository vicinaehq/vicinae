#pragma once
#include "bookmark-service.hpp"
#include "create-quicklink-command.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include <memory>
#include <qclipboard.h>

class OpenBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;

  void execute(AppWindow &app) override {
    auto appDb = ServiceRegistry::instance()->appDb();
    QString expanded;
    std::vector<std::pair<QString, QString>> args = app.topBar->m_completer->collect();
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
          if (argumentIndex < args.size()) { expanded += args.at(argumentIndex++).second; }
        }
      }
    }

    qDebug() << "opening link" << expanded;

    if (auto app = appDb->findById(m_bookmark->app())) { appDb->launch(*app, {expanded}); }

    app.closeWindow(true);
  }

  QString title() const override { return "Open bookmark"; }

public:
  OpenBookmarkAction(const std::shared_ptr<Bookmark> &bookmark)
      : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark) {}
};

struct EditBookmarkAction : public AbstractAction {
public:
  std::shared_ptr<Bookmark> m_bookmark;
  QList<QString> args;

  void execute(AppWindow &app) override {
    auto view = new EditBookmarkView(app, m_bookmark);

    emit app.pushView(view,
                      {.navigation = NavigationStatus{
                           .title = "Edit link",
                           .iconUrl = BuiltinOmniIconUrl("bookmark").setBackgroundTint(ColorTint::Red)}});
  }

  void setArgs(const QList<QString> &args) { this->args = args; }

  EditBookmarkAction(const std::shared_ptr<Bookmark> &bookmark, const QList<QString> &args = {})
      : AbstractAction("Edit bookmark", BuiltinOmniIconUrl("pencil")), m_bookmark(bookmark), args(args) {}
};

struct RemoveBookmarkAction : public AbstractAction {
  std::shared_ptr<Bookmark> m_bookmark;

public:
  void execute(AppWindow &app) override {
    auto bookmarkDb = ServiceRegistry::instance()->bookmarks();
    bool removeResult = bookmarkDb->removeBookmark(m_bookmark->id());

    if (removeResult) {
      app.statusBar->setToast("Removed link");
    } else {
      app.statusBar->setToast("Failed to remove link", ToastPriority::Danger);
    }
  }

  RemoveBookmarkAction(const std::shared_ptr<Bookmark> &link)
      : AbstractAction("Remove link", BuiltinOmniIconUrl("trash")), m_bookmark(link) {}
};

struct DuplicateBookmarkAction : public AbstractAction {
public:
  std::shared_ptr<Bookmark> link;

  void execute(AppWindow &app) override {
    auto view = new DuplicateBookmarkView(app, link);

    emit app.pushView(view, {.navigation = NavigationStatus{
                                 .title = "Duplicate link",
                                 .iconUrl = BuiltinOmniIconUrl("link").setBackgroundTint(ColorTint::Red)}});
  }

  DuplicateBookmarkAction(const std::shared_ptr<Bookmark> &link)
      : AbstractAction("Duplicate link", BuiltinOmniIconUrl("duplicate")), link(link) {}
};

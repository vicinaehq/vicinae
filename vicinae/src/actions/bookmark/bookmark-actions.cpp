#include "actions/bookmark/bookmark-actions.hpp"
#include "create-quicklink-command.hpp"
#include "service-registry.hpp"
#include "ui/views/base-view.hpp"

void OpenBookmarkAction::execute() {
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

OpenBookmarkAction::OpenBookmarkAction(const std::shared_ptr<Bookmark> &bookmark,
                                       const std::vector<QString> &arguments)
    : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark), m_arguments(arguments) {}

// OpenCompletedBookmarkAction

void OpenCompletedBookmarkAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  OpenBookmarkAction open(m_bookmark, ui->topView()->argumentValues());

  open.execute();
}

OpenCompletedBookmarkAction::OpenCompletedBookmarkAction(const std::shared_ptr<Bookmark> &bookmark)
    : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark) {}

// OpenBookmarkFromSearchText

void OpenBookmarkFromSearchText::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  OpenBookmarkAction open(m_bookmark, {ui->topView()->searchText()});

  open.execute();
}

OpenBookmarkFromSearchText::OpenBookmarkFromSearchText(const std::shared_ptr<Bookmark> &bookmark)
    : AbstractAction("Open bookmark", bookmark->icon()), m_bookmark(bookmark) {}

// EditBookmarkAction

void EditBookmarkAction::execute() {
  auto view = new EditBookmarkView(m_bookmark);
  auto ui = ServiceRegistry::instance()->UI();

  ui->pushView(view);
}

EditBookmarkAction::EditBookmarkAction(const std::shared_ptr<Bookmark> &bookmark, const QList<QString> &args)
    : AbstractAction("Edit bookmark", ImageURL::builtin("pencil")), m_bookmark(bookmark) {}

// RemoveBookmarkAction

void RemoveBookmarkAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto bookmarkDb = ServiceRegistry::instance()->bookmarks();
  bool removeResult = bookmarkDb->removeBookmark(m_bookmark->id());

  if (removeResult) {
    ui->setToast("Removed link");
  } else {
    ui->setToast("Failed to remove link", ToastPriority::Danger);
  }
}

RemoveBookmarkAction::RemoveBookmarkAction(const std::shared_ptr<Bookmark> &link)
    : AbstractAction("Remove link", ImageURL::builtin("trash")), m_bookmark(link) {}

// DuplicateBookmarkAction

void DuplicateBookmarkAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto view = new DuplicateBookmarkView(link);

  emit ui->pushView(view, {.navigation = NavigationStatus{
                               .title = "Duplicate link",
                               .iconUrl = ImageURL::builtin("link").setBackgroundTint(ColorTint::Red)}});
}

DuplicateBookmarkAction::DuplicateBookmarkAction(const std::shared_ptr<Bookmark> &link)
    : AbstractAction("Duplicate link", ImageURL::builtin("duplicate")), link(link) {}

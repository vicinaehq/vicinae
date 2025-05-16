#pragma once
#include "bookmark-service.hpp"
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

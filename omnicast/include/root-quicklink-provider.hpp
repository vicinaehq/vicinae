#pragma once
#include "root-item-manager.hpp"
#include "quicklist-database.hpp"
#include "quicklink-actions.hpp"

class RootQuicklinkProvider : public RootProvider {
  QuicklistDatabase &m_db;

  class RootQuicklinkItem : public RootItem {
    std::shared_ptr<Quicklink> m_link;

    QString displayName() const override { return m_link->name; }

    ArgumentList arguments() const override {
      ArgumentList args;

      for (const auto &placeholder : m_link->placeholders) {
        CommandArgument arg;

        arg.type = CommandArgument::Text;
        arg.required = true;
        arg.placeholder = placeholder;
        arg.name = placeholder;
        args.emplace_back(arg);
      }

      return args;
    }

    OmniIconUrl iconUrl() const override {
      OmniIconUrl url(m_link->iconName);

      if (url.type() == OmniIconType::Builtin) { url.setBackgroundTint(ColorTint::Red); }

      return url;
    }

    QString uniqueId() const override { return QString("quicklink:%1").arg(m_link->id); }

    QList<AbstractAction *> actions() const override {
      QList<AbstractAction *> list;

      list << new OpenCompletedQuicklinkAction(m_link);
      list << new EditQuicklinkAction(m_link);
      list << new DuplicateQuicklinkAction(m_link);
      list << new RemoveQuicklinkAction(m_link);

      return list;
    }

  public:
    RootQuicklinkItem(const std::shared_ptr<Quicklink> &link) : m_link(link) {}
  };

public:
  QString displayName() const override { return "Quicklinks"; }
  QString uniqueId() const override { return "quicklinks"; }

  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_db.list() | std::views::transform([](const auto &a) {
             return std::static_pointer_cast<RootItem>(std::make_shared<RootQuicklinkItem>(a));
           }) |
           std::ranges::to<std::vector>();
  };

  RootQuicklinkProvider(QuicklistDatabase &db) : m_db(db) {}
};

#pragma once
#include "actions/shortcut/shortcut-actions.hpp"
#include "ui/detail/detail-widget.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "common.hpp"
#include "services/shortcut/shortcut-service.hpp"
#include "extend/metadata-model.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/shortcut/shortcut.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "ui/typography/typography.hpp"
#include <memory>
#include <qboxlayout.h>
#include <QClipboard>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <sys/socket.h>
#include <qscrollarea.h>
#include "ui/views/list-view.hpp"
#include "utils/layout.hpp"

class ShortcutDetailWidget : public DetailWidget {
  TypographyWidget *m_expandedLink = new TypographyWidget(this);
  std::shared_ptr<Shortcut> m_shortcut;

  QString expandLink(const Shortcut &shortcut, const std::vector<QString> &arguments) {
    auto appDb = ServiceRegistry::instance()->appDb();
    QString expanded;
    size_t argumentIndex = 0;

    for (const auto &part : m_shortcut->parts()) {
      if (auto s = std::get_if<QString>(&part)) {
        expanded += *s;
      } else if (auto placeholder = std::get_if<Shortcut::ParsedPlaceholder>(&part)) {
        if (placeholder->id == "clipboard") {
          expanded += QApplication::clipboard()->text();
        } else if (placeholder->id == "selected") {
          // TODO: selected text
        } else if (placeholder->id == "uuid") {
          expanded += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
        } else {
          if (argumentIndex < arguments.size()) { expanded += arguments.at(argumentIndex++); }
        }
      }
    }

    return expanded;
  }

public:
  void setShortcut(const std::shared_ptr<Shortcut> &shortcut) {
    auto appDb = ServiceRegistry::instance()->appDb();
    std::vector<MetadataItem> meta;

    m_shortcut = shortcut;

    auto name = MetadataLabel{
        .text = m_shortcut->name(),
        .title = "Name",
    };

    meta.emplace_back(name);

    if (auto app = appDb->findById(shortcut->app())) {
      meta.emplace_back(MetadataLabel{
          .text = app->displayName(),
          .title = "Application",
          .icon = app->iconUrl(),
      });
    }

    meta.emplace_back(MetadataLabel{
        .text = QString::number(m_shortcut->openCount()),
        .title = "Opened",
    });

    meta.emplace_back(MetadataLabel{
        .text = m_shortcut->lastOpenedAt() ? m_shortcut->lastOpenedAt()->toString() : "Never",
        .title = "Last Opened",
    });

    meta.emplace_back(MetadataLabel{
        .text = m_shortcut->createdAt().toString(),
        .title = "Created at",
    });

    setMetadata(meta);
    m_expandedLink->setText(expandLink(*m_shortcut.get(), {}));
  }

  void updateArguments(const std::vector<QString> &arguments) {
    m_expandedLink->setText(expandLink(*m_shortcut.get(), arguments));
  }

  ShortcutDetailWidget(QWidget *parent = nullptr) {
    m_expandedLink->setWordWrap(true);
    m_expandedLink->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setContent(VStack().add(m_expandedLink).margins(10).buildWidget());
  }
};

class QuicklinkItem : public AbstractDefaultListItem, public ListView::Actionnable {
  std::shared_ptr<Shortcut> link;

public:
  const std::shared_ptr<Shortcut> &shortcut() const { return link; }

  std::unique_ptr<CompleterData> createCompleter() const override {
    ArgumentList args;

    for (const auto &barg : link->arguments()) {
      CommandArgument cmdArg;

      cmdArg.type = CommandArgument::Text;
      cmdArg.required = barg.defaultValue.isEmpty();
      cmdArg.placeholder = barg.name;
      args.emplace_back(cmdArg);
    }

    return std::make_unique<CompleterData>(CompleterData{
        .iconUrl = link->icon(),
        .arguments = args,
    });
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto mainSection = panel->createSection();
    auto itemSection = panel->createSection();
    auto dangerSection = panel->createSection();

    auto open = new OpenCompletedShortcutAction(link);
    auto edit = new EditShortcutAction(link);
    auto duplicate = new DuplicateShortcutAction(link);
    auto remove = new RemoveShortcutAction(link);

    duplicate->setShortcut(Keybind::DuplicateAction);
    edit->setShortcut(Keybind::EditAction);
    remove->setShortcut(Keybind::RemoveAction);

    panel->setTitle(link->name());
    mainSection->addAction(open);
    mainSection->addAction(edit);
    mainSection->addAction(duplicate);
    dangerSection->addAction(remove);

    return panel;
  }

  ItemData data() const override {
    return {
        .iconUrl = link->icon(),
        .name = link->name(),
    };
  }

  QWidget *generateDetail() const override {
    auto widget = new ShortcutDetailWidget;

    widget->setShortcut(link);

    return widget;
  }

  QString generateId() const override { return link->id(); }

public:
  QuicklinkItem(const std::shared_ptr<Shortcut> &link) : link(link) {}
};

class ManageShortcutsView : public ListView {
  void renderList(const QString &s, OmniList::SelectionPolicy policy = OmniList::SelectFirst) {
    auto shortcutService = ServiceRegistry::instance()->shortcuts();

    m_list->updateModel(
        [&]() {
          auto &section = m_list->addSection("Shortcuts");

          for (const auto &shortcut : shortcutService->shortcuts()) {
            if (!shortcut->name().contains(s, Qt::CaseInsensitive)) continue;
            section.addItem(std::make_unique<QuicklinkItem>(shortcut));
          }
        },
        policy);
  }

  void itemSelected(const OmniList::AbstractVirtualItem *item) override {}

  void onShortcutRemoved() { renderList(searchText(), OmniList::PreserveSelection); }

  void onShortcutSaved() { renderList(searchText(), OmniList::PreserveSelection); }

  void onShortcutUpdated() { renderList(searchText(), OmniList::PreserveSelection); }

  void textChanged(const QString &s) override { renderList(s); }

  void initialize() override {
    setSearchPlaceholderText("Search shortcuts...");
    textChanged("");
  }

  void argumentValuesChanged(const std::vector<std::pair<QString, QString>> &args) override {
    std::vector<QString> values;

    for (const auto &pair : args) {
      values.emplace_back(pair.second);
    }

    if (auto shortcutDetail = dynamic_cast<ShortcutDetailWidget *>(detail())) {
      shortcutDetail->updateArguments(values);
    }
  }

public:
  ManageShortcutsView() {
    auto shortcutService = ServiceRegistry::instance()->shortcuts();

    connect(shortcutService, &ShortcutService::shortcutSaved, this, &ManageShortcutsView::onShortcutSaved);
    connect(shortcutService, &ShortcutService::shortcutUpdated, this,
            &ManageShortcutsView::onShortcutUpdated);
    connect(shortcutService, &ShortcutService::shortcutRemoved, this,
            &ManageShortcutsView::onShortcutRemoved);
  }
};

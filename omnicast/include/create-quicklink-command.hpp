#pragma once
#include "base-view.hpp"
#include "builtin_icon.hpp"
#include "services/bookmark/bookmark-service.hpp"
#include "favicon/favicon-service.hpp"
#include "action-panel/action-panel.hpp"
#include "omni-icon.hpp"
#include "quicklist-database.hpp"
#include "service-registry.hpp"
#include "timer.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/completed-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/omni-list.hpp"
#include "ui/toast.hpp"
#include "ui/form/form.hpp"
#include <functional>
#include <memory>
#include <qboxlayout.h>
#include <qlocale.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <qsharedpointer.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>
#include <qwidget.h>
#include <ranges>
#include <sched.h>
#include <unistd.h>

class AppSelectorItem : public SelectorInput::AbstractItem {
public:
  std::shared_ptr<Application> app;
  bool isDefault;

  std::optional<OmniIconUrl> icon() const override { return app->iconUrl(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    return name;
  }

  AbstractItem *clone() const override { return new AppSelectorItem(*this); }

  void setApp(const std::shared_ptr<Application> &app) { this->app = app; }

  QString generateId() const override { return app->id(); }

  AppSelectorItem(const std::shared_ptr<Application> &app) : app(app) {}
};

class DefaultAppItem : public AppSelectorItem {
  QString generateId() const override { return "default"; }
  QString displayName() const override { return AppSelectorItem::displayName() + " (Default)"; }
  AbstractItem *clone() const override { return new DefaultAppItem(*this); }

public:
  DefaultAppItem(const std::shared_ptr<Application> &app) : AppSelectorItem(app) {}
};

class IconSelectorItem : public SelectorInput::AbstractItem {
public:
  QString name;
  QString dname;
  OmniIconUrl iconUrl;

  std::optional<OmniIconUrl> icon() const override { return iconUrl; }

  QString displayName() const override { return iconUrl.name(); }

  QString generateId() const override { return iconUrl.toString(); }

  void setDisplayName(const QString &name) { this->dname = name; }

  AbstractItem *clone() const override { return new IconSelectorItem(*this); }

  void setIcon(const OmniIconUrl &url) { this->iconUrl = url; }

  IconSelectorItem(const OmniIconUrl &url, const QString &displayName = "")
      : dname(displayName), iconUrl(url) {}
};

class DefaultIconSelectorItem : public IconSelectorItem {
  QString generateId() const override { return "default"; }
  QString displayName() const override { return "Default"; }
  AbstractItem *clone() const override { return new DefaultIconSelectorItem(*this); }

public:
  DefaultIconSelectorItem(const OmniIconUrl &url, const QString &displayName = "")
      : IconSelectorItem(url, displayName) {}
};

struct LinkDynamicPlaceholder {
  OmniIconUrl icon;
  QString title;
  QString value;
  QString id;
  std::vector<std::pair<QString, QString>> arguments;
};

class CompletionListItem : public AbstractDefaultListItem {
  LinkDynamicPlaceholder m_data;

  ItemData data() const override { return {.iconUrl = m_data.icon, .name = m_data.title, .accessories = {}}; }

  QString generateId() const override { return m_data.title; }

public:
  const LinkDynamicPlaceholder &argument() const { return m_data; }

  CompletionListItem(const LinkDynamicPlaceholder &data) : m_data(data) {}
};

class BookmarkFormView : public FormView {
  std::vector<LinkDynamicPlaceholder> mainLinkArguments{
      LinkDynamicPlaceholder{
          .icon = BuiltinOmniIconUrl("text-cursor"), .title = "Selected Text", .id = "selected"},
      LinkDynamicPlaceholder{
          .icon = BuiltinOmniIconUrl("copy-clipboard"), .title = "Clipboard Text", .id = "clipboard"},
      LinkDynamicPlaceholder{.icon = BuiltinOmniIconUrl("text-cursor"),
                             .title = "Argument",
                             .id = "argument",
                             .arguments = {{"name", "Argument"}}},
      LinkDynamicPlaceholder{.icon = BuiltinOmniIconUrl("fingerprint"), .title = "UUID", .id = "uuid"},
  };

  void handleAppSelectorTextChanged(const QString &text) {}

  void iconSelectorTextChanged(const QString &text) {}

  int getCurrentPlaceholderStartIndex(const QString &text) {
    for (int cursor = link->cursorPosition() - 1; cursor >= 0; --cursor) {
      QChar c = text.at(cursor);

      if (c == '}') { break; }
      if (c == '{') { return cursor; }
    }

    return -1;
  }

  void insertLinkPlaceholder(const LinkDynamicPlaceholder &placeholder) {
    QString value = link->text();
    int startIdx = getCurrentPlaceholderStartIndex(link->text());
    QString formatted = value.sliced(0, startIdx) + '{' + placeholder.id;
    int startSelection = -1;
    int selectionSize = 0;

    for (const auto &[k, v] : placeholder.arguments) {
      if (startSelection == -1) {
        startSelection = formatted.size() + k.size() + 3;
        selectionSize = v.size();
      }

      formatted += QString(" %1=\"%2\"").arg(k).arg(v);
    }

    int endIdx = startIdx + 1;

    while (endIdx < value.size() && value.at(endIdx) != '}' && value.at(endIdx) != '{') {
      ++endIdx;
    }

    if (endIdx < value.size() && value.at(endIdx) == '}') endIdx += 1;

    formatted += '}' + value.sliced(endIdx);

    link->setText(formatted);

    if (startSelection != -1) { link->input()->setSelection(startSelection, selectionSize); }
  }

  void handleLinkBlurred() {
    QString text = link->text();
    auto appDb = ServiceRegistry::instance()->appDb();
    QUrl url(text);

    if (auto app = appDb->findBestOpener(text)) {
      appSelector->updateItem("default", [&app](SelectorInput::AbstractItem *item) {
        static_cast<AppSelectorItem *>(item)->setApp(app);
      });
      iconSelector->updateItem("default", [this](SelectorInput::AbstractItem *item) {
        auto icon = static_cast<IconSelectorItem *>(item);
        auto appItem = static_cast<const AppSelectorItem *>(appSelector->value());

        if (auto ico = appItem->icon()) { icon->setIcon(*ico); }
        icon->setDisplayName("Default");
      });
    }

    if (url.scheme().startsWith("http")) {
      auto request =
          QSharedPointer<AbstractFaviconRequest>(FaviconService::instance()->makeRequest(url.host()));

      connect(request.get(), &AbstractFaviconRequest::finished, this, [this, url, request]() {
        iconSelector->updateItem("default", [&url](SelectorInput::AbstractItem *item) {
          auto icon = FaviconOmniIconUrl(url.host()).withFallback(BuiltinOmniIconUrl("image"));
          auto iconItem = static_cast<IconSelectorItem *>(item);

          iconItem->setIcon(icon);
          iconItem->setDisplayName(url.host());
        });
      });
      request->start();

      return;
    }
  }

  void handleLinkChange(const QString &text) {
    int openIdx = getCurrentPlaceholderStartIndex(text);

    if (openIdx != -1) {
      int closeIdx = -1;

      for (int i = openIdx + 1; i < text.size(); ++i) {
        if (text.at(i) == '}' || text.at(i) == '{') {
          closeIdx = i;
          break;
        }
      }

      QString base;

      if (closeIdx != -1) {
        base = text.sliced(openIdx + 1, closeIdx - openIdx - 1).trimmed();
      } else {
        base = text.sliced(openIdx + 1).trimmed();
      }

      qDebug() << "base" << base;

      auto completer = link->completer();

      completer->beginResetModel();

      auto mainItems = mainLinkArguments | std::views::filter([base](const LinkDynamicPlaceholder &arg) {
                         return arg.title.contains(base, Qt::CaseInsensitive);
                       }) |
                       std::views::transform([](const LinkDynamicPlaceholder &arg) {
                         qDebug() << "title" << arg.title;
                         return std::make_unique<CompletionListItem>(arg);
                       });
      auto &mainSection = completer->addSection("");

      for (auto item : mainItems) {
        mainSection.addItem(std::move(item));
      }

      completer->endResetModel(OmniList::SelectFirst);
      link->showCompleter();
    } else {
      link->hideCompleter();
    }
  }

  void appSelectionChanged(const SelectorInput::AbstractItem &item) {
    auto &appItem = static_cast<const AppSelectorItem &>(item);

    if (link->text().isEmpty()) return;

    iconSelector->updateItem("default", [appItem](SelectorInput::AbstractItem *item) {
      auto icon = static_cast<IconSelectorItem *>(item);

      icon->setIcon(appItem.app->iconUrl());
      icon->setDisplayName(appItem.displayName());
    });
  }

protected:
  FormWidget *form;
  BaseInput *name;
  CompletedInput *link;
  SelectorInput *appSelector;
  SelectorInput *iconSelector;

public:
  BookmarkFormView()
      : form(new FormWidget), name(new BaseInput), link(new CompletedInput), appSelector(new SelectorInput),
        iconSelector(new SelectorInput) {
    Timer timer;
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();

    name->setPlaceholderText("Bookmark name");
    link->setPlaceholderText("https://google.com/search?q={argument}");

    auto nameField = new FormField;
    auto linkField = new FormField;
    auto openField = new FormField;
    auto iconField = new FormField;

    nameField->setName("Name");
    nameField->setWidget(name, name->focusNotifier());
    linkField->setName("URL");
    linkField->setWidget(link, link->focusNotifier());
    linkField->setInfo("The URL that will be opened by the specified app. You can make it dynamic by using "
                       "placeholders such as `{argument}`");
    openField->setName("Open with");
    openField->setWidget(appSelector, appSelector->focusNotifier());
    iconField->setName("Icon");
    iconField->setWidget(iconSelector, iconSelector->focusNotifier());

    form->addField(nameField);
    form->addField(linkField);
    form->addField(openField);
    form->addField(iconField);

    connect(link, &CompletedInput::textChanged, this, &BookmarkFormView::handleLinkChange);
    connect(linkField, &FormField::blurred, this, &BookmarkFormView::handleLinkBlurred);
    connect(appSelector, &SelectorInput::textChanged, this, &BookmarkFormView::handleAppSelectorTextChanged);
    connect(iconSelector, &SelectorInput::textChanged, this, &BookmarkFormView::iconSelectorTextChanged);
    connect(appSelector, &SelectorInput::selectionChanged, this, &BookmarkFormView::appSelectionChanged);
    connect(link, &CompletedInput::completionActivated, this,
            [this](const OmniList::AbstractVirtualItem &item) {
              auto completion = static_cast<const CompletionListItem &>(item);

              insertLinkPlaceholder(completion.argument());
            });

    form->setContentsMargins(0, 10, 0, 0);
    setupUI(form);
    timer.time("Build form");
  }

  void initializeIconSelector() {
    std::vector<std::shared_ptr<SelectorInput::AbstractItem>> iconItems;
    auto mapItem = [](auto &&name) -> std::shared_ptr<SelectorInput::AbstractItem> {
      return std::make_shared<IconSelectorItem>(BuiltinOmniIconUrl(name));
    };

    iconItems.reserve(BuiltinIconService::icons().size() + 1);
    auto items = BuiltinIconService::icons() | std::views::transform(mapItem);

    iconItems.emplace_back(
        std::make_shared<DefaultIconSelectorItem>(BuiltinOmniIconUrl("bookmark"), "Default"));
    std::ranges::for_each(items, [&](auto item) { iconItems.emplace_back(item); });

    iconSelector->addSection("", iconItems);
    iconSelector->updateModel();
    iconSelector->setValue("default");
  }

  void initializeAppSelector() {
    auto appDb = ServiceRegistry::instance()->appDb();
    std::vector<std::shared_ptr<SelectorInput::AbstractItem>> appItems;
    auto appCandidates = appDb->list() | std::views::filter([](auto &&app) { return app->displayable(); });

    if (auto browser = appDb->webBrowser()) {
      appItems.emplace_back(std::make_unique<DefaultAppItem>(browser));
    }

    for (const auto &app : appCandidates) {
      appItems.emplace_back(std::make_unique<AppSelectorItem>(app));

      for (const auto &action : app->actions()) {
        appItems.emplace_back(std::make_unique<AppSelectorItem>(action));
      }
    }

    appSelector->addSection("", appItems);
    appSelector->updateModel();
    appSelector->setValue("default");
  }

  void initialize() override {
    initializeAppSelector();
    initializeIconSelector();
  }

  void loadLink(const Quicklink &quicklink) {
    auto appDb = ServiceRegistry::instance()->appDb();

    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.rawUrl);

    if (auto app = appDb->findById(quicklink.app)) {
      // appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onActivate() override {
    auto panel = new ActionPanelStaticListView;
    auto submitAction = new StaticAction("Submit", BuiltinOmniIconUrl("enter-key"), [this]() { submit(); });

    submitAction->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"shift"}});
    submitAction->setPrimary(true);

    panel->addAction(submitAction);
    m_actionPannelV2->setView(panel);
    form->focusFirst();
  }

  virtual void submit() {
    auto ui = ServiceRegistry::instance()->UI();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto bookmarkDb = ServiceRegistry::instance()->bookmarks();

    if (link->text().isEmpty()) {
      form->setError(link, "Required");
      return;
    }

    auto item = static_cast<const AppSelectorItem *>(appSelector->value());

    if (!item) {
      form->setError(appSelector, "Required");
      return;
    }

    auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

    if (!icon) {
      form->setError(iconSelector, "Required");
      return;
    }

    if (bookmarkDb->createBookmark(name->text(), icon->icon()->toString(), link->text(), item->app->id())) {
      ui->setToast("Created bookmark");
      ServiceRegistry::instance()->UI()->popView();
    } else {
      ui->setToast("Failed to create bookmark", ToastPriority::Danger);
    }
  }
};

class EditBookmarkView : public BookmarkFormView {
  std::shared_ptr<Bookmark> m_bookmark;

public:
  EditBookmarkView(const std::shared_ptr<Bookmark> &bookmark) : m_bookmark(bookmark) {
    auto appDb = ServiceRegistry::instance()->appDb();

    name->setText(m_bookmark->name());
    link->setText(m_bookmark->url());

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName, quicklink.iconName));
  }

  void initialize() override {
    BookmarkFormView::initialize();

    if (!iconSelector->setValue(m_bookmark->icon())) {
      iconSelector->updateItem("default", [&](SelectorInput::AbstractItem *item) {
        auto icon = static_cast<IconSelectorItem *>(item);

        icon->setIcon(m_bookmark->icon());
        icon->setDisplayName("Default");
      });
    }
    appSelector->setValue(m_bookmark->app());
  }

  void submit() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
    auto bookmarkDb = ServiceRegistry::instance()->bookmarks();
    auto item = static_cast<const AppSelectorItem *>(appSelector->value());

    if (!item) {
      form->setError(appSelector, "Required");
      return;
    }

    auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

    if (!icon) {
      form->setError(iconSelector, "Required");
      return;
    }

    bool updated = bookmarkDb->updateBookmark(m_bookmark->id(), name->text(), icon->icon().value().toString(),
                                              link->text(), item->app->id());

    if (!updated) {
      ui->setToast("Failed to update bookmark");
      return;
    }

    ui->popView();
  }
};

class DuplicateBookmarkView : public BookmarkFormView {
public:
  DuplicateBookmarkView(const std::shared_ptr<Bookmark> &bookmark) {
    auto appDb = ServiceRegistry::instance()->appDb();

    name->setText(QString("Copy of %1").arg(bookmark->name()));
    link->setText(bookmark->url());

    if (auto app = appDb->findById(bookmark->app())) { appSelector->setValue(bookmark->app()); }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onActivate() override { name->selectAll(); }

  void submit() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto bookmarkDb = ServiceRegistry::instance()->bookmarks();
    auto item = static_cast<const AppSelectorItem *>(appSelector->value());

    if (!item) {
      form->setError(appSelector, "Required");
      return;
    }

    auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

    if (!icon) {
      form->setError(iconSelector, "Required");
      return;
    }

    if (bookmarkDb->createBookmark(name->text(), icon->icon()->toString(), link->text(), item->app->id())) {
      ui->setToast("Created bookmark");
      ui->popView();
    } else {
      ui->setToast("Failed to create bookmark", ToastPriority::Danger);
    }
  }
};

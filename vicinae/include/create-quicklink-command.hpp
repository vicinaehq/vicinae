#pragma once
#include "common.hpp"
#include "lib/fzf.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/shortcut/shortcut.hpp"
#include "ui/form/selector-input-v2.hpp"
#include "ui/views/base-view.hpp"
#include "../src/ui/image/url.hpp"
#include "service-registry.hpp"
#include "timer.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/completed-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/form/form.hpp"
#include <functional>
#include <iterator>
#include <memory>
#include <qboxlayout.h>
#include <qfuturewatcher.h>
#include <qlocale.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <qsharedpointer.h>
#include <qtconcurrentfilter.h>
#include <qtconcurrentmap.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <qwidget.h>
#include <ranges>
#include <sched.h>
#include <unistd.h>
#include "services/app-service/app-service.hpp"
#include "ui/views/form-view.hpp"
#include "ui/vlist/common/dropdown-model.hpp"
#include "vicinae.hpp"

template <> struct fzf::Searchable<std::shared_ptr<AbstractApplication>> {
  using Input = std::shared_ptr<AbstractApplication>;
  using Output = AbstractApplication *;

  static constexpr std::vector<std::string> strings(const std::shared_ptr<AbstractApplication> &app) {
    return {app->displayName().toStdString()};
  }
  static constexpr AbstractApplication *transform(const std::shared_ptr<AbstractApplication> &app) {
    return app.get();
  }
};

class AppSelectorModel : public vicinae::ui::DropdownModel {
public:
  virtual int sectionCount() const override { return 1; }

  virtual int sectionItemCount(int id) const override { return m_apps.size(); }

  virtual Item sectionItemAt(int id, int itemIdx) const override {
    auto &app = m_apps.at(itemIdx);
    auto appPtr = app.data;
    return {.id = appPtr->id(), .title = appPtr->displayName(), .icon = appPtr->iconUrl()};
  }

  virtual int sectionIdFromIndex(int idx) const override { return idx; }

  virtual std::string_view sectionName(int id) const override { return ""; }

  void setApps(std::vector<Scored<AbstractApplication *>> apps) {
    m_apps = apps;
    emit dataChanged();
  }

private:
  std::vector<Scored<AbstractApplication *>> m_apps;
};

class AppSelector : public SelectorInputV2 {
public:
  AppSelector(const AppService &appDb) : m_appDb(appDb) {
    setModel(m_model);
    m_apps = appDb.list();
    setApps();
    connect(this, &SelectorInputV2::textChanged, this, [this]() { setApps(); });
  }

  void setApps() {
    auto query = searchText().toStdString();
    fzf::search(m_apps, m_scoredApps, query);
    m_model->setApps(m_scoredApps);
  }

private:
  std::vector<std::shared_ptr<AbstractApplication>> m_apps;
  std::vector<Scored<AbstractApplication *>> m_scoredApps;
  AppSelectorModel *m_model = new AppSelectorModel;
  const AppService &m_appDb;
};

struct LinkDynamicPlaceholder {
  ImageURL icon;
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

class ShortcutFormView : public ManagedFormView {
  std::vector<LinkDynamicPlaceholder> mainLinkArguments{
      LinkDynamicPlaceholder{
          .icon = ImageURL::builtin("text-cursor"), .title = "Selected Text", .id = "selected"},
      LinkDynamicPlaceholder{
          .icon = ImageURL::builtin("copy-clipboard"), .title = "Clipboard Text", .id = "clipboard"},
      LinkDynamicPlaceholder{.icon = ImageURL::builtin("text-cursor"),
                             .title = "Argument",
                             .id = "argument",
                             .arguments = {{"name", "Argument"}}},
      LinkDynamicPlaceholder{.icon = ImageURL::builtin("fingerprint"), .title = "UUID", .id = "uuid"},
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
    /*
QString text = link->text();
auto appDb = ServiceRegistry::instance()->appDb();
QUrl url(text);

if (auto app = appDb->findDefaultOpener(text)) {
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
using Watcher = QFutureWatcher<FaviconService::FaviconResponse>;

// we fetch to know whether we need to update the icon or not
// but we don't use the data the first time
auto watcher = std::make_unique<Watcher>();
auto ptr = watcher.get();

watcher->setFuture(FaviconService::instance()->makeRequest(url.host()));
connect(ptr, &Watcher::finished, this, [this, url, watcher = std::move(watcher)]() {
  iconSelector->updateItem("default", [&url](SelectorInput::AbstractItem *item) {
    auto icon = ImageURL::favicon(url.host()).withFallback(ImageURL::builtin("image"));
    auto iconItem = static_cast<IconSelectorItem *>(item);

    iconItem->setIcon(icon);
    iconItem->setDisplayName(url.host());
  });
});

return;
}
  */
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
    /*
auto &appItem = static_cast<const AppSelectorItem &>(item);

if (link->text().isEmpty()) return;

iconSelector->updateItem("default", [appItem](SelectorInput::AbstractItem *item) {
auto icon = static_cast<IconSelectorItem *>(item);

icon->setIcon(appItem.app->iconUrl());
icon->setDisplayName(appItem.displayName());
});
  */
  }

protected:
  BaseInput *name;
  CompletedInput *link;
  SelectorInputV2 *appSelector;
  SelectorInput *iconSelector;

public:
  // to force default icon update
  void blurLink() { handleLinkBlurred(); }

  ShortcutFormView() : name(new BaseInput), link(new CompletedInput), iconSelector(new SelectorInput) {}

  void setPrefilledValues(const QString &link, const QString &name = "", const QString &application = "",
                          const QString &icon = "") {
    m_prefilledName = name;
    m_prefilledLink = link;
    m_prefilledApp = application;
    m_prefilledIcon = icon;
  }

  void initializeForm() override {
    auto appDb = context()->services->appDb();

    if (!m_prefilledName.isEmpty()) { name->setText(m_prefilledName); }
    if (!m_prefilledLink.isEmpty()) {
      link->setText(m_prefilledLink);
      handleLinkBlurred();
    }
    if (!m_prefilledApp.isEmpty() && appDb->findById(m_prefilledApp)) {
      appSelector->setValue(m_prefilledApp);
    }
    if (!m_prefilledIcon.isEmpty()) { iconSelector->setValue(m_prefilledIcon); }

    QTimer::singleShot(0, this, [this]() {
      if (!m_prefilledName.isEmpty()) {
        name->selectAll();
      } else if (!m_prefilledLink.isEmpty()) {
        link->input()->setFocus();
      } else {
        form()->focusFirst();
      }
    });

    Timer timer;
    name->setPlaceholderText("Shortcut Name");
    link->setPlaceholderText("https://google.com/search?q={argument}");

    appSelector = new AppSelector(*appDb);

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

    form()->addField(nameField);
    form()->addField(linkField);
    form()->addField(openField);
    form()->addField(iconField);

    connect(link, &CompletedInput::textChanged, this, &ShortcutFormView::handleLinkChange);
    connect(linkField, &FormField::blurred, this, &ShortcutFormView::handleLinkBlurred);
    // connect(appSelector, &SelectorInput::textChanged, this,
    // &ShortcutFormView::handleAppSelectorTextChanged);
    connect(iconSelector, &SelectorInput::textChanged, this, &ShortcutFormView::iconSelectorTextChanged);
    // connect(appSelector, &SelectorInput::selectionChanged, this, &ShortcutFormView::appSelectionChanged);
    connect(link, &CompletedInput::completionActivated, this,
            [this](const OmniList::AbstractVirtualItem &item) {
              auto completion = static_cast<const CompletionListItem &>(item);

              insertLinkPlaceholder(completion.argument());
            });

    form()->setContentsMargins(0, 10, 0, 0);
  }

private:
  QString m_prefilledLink;
  QString m_prefilledName;
  QString m_prefilledApp;
  QString m_prefilledIcon;

  void onSubmit() override {
    auto shortcutDb = context()->services->shortcuts();
    auto toast = context()->services->toastService();

    if (link->text().isEmpty()) {
      form()->setError(link, "Required");
      return;
    }

    /*
auto item = static_cast<const AppSelectorItem *>(appSelector->value());

if (!item) {
  form()->setError(appSelector, "Required");
  return;
}

auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

if (!icon) {
  form()->setError(iconSelector, "Required");
  return;
}

    if (shortcutDb->createShortcut(name->text(), icon->icon()->toString(), link->text(), item->app->id())) {
      toast->setToast("Created shortcut");
      popSelf();
    } else {
      toast->setToast("Failed to create shortcut", ToastStyle::Danger);
    }
    */
  }
};

class EditShortcutView : public ShortcutFormView {
  std::shared_ptr<Shortcut> m_shortcut;

public:
  EditShortcutView(const std::shared_ptr<Shortcut> &shortcut) : m_shortcut(shortcut) {}

  QString initialNavigationTitle() const override { return "Edit Shortcut"; }
  ImageURL initialNavigationIcon() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void initializeForm() override {
    auto appDb = context()->services->appDb();

    ShortcutFormView::initializeForm();
    name->setText(m_shortcut->name());
    link->setText(m_shortcut->url());

    if (!iconSelector->setValue(m_shortcut->icon())) { blurLink(); }

    if (auto app = appDb->findById(m_shortcut->app())) { appSelector->setValue(m_shortcut->app()); }
  }

  void onSubmit() override {
    /*
auto shortcutDb = context()->services->shortcuts();
auto toast = context()->services->toastService();
auto item = static_cast<const AppSelectorItem *>(appSelector->value());

if (!item) {
form()->setError(appSelector, "Required");
return;
}

auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

if (!icon) {
form()->setError(iconSelector, "Required");
return;
}

bool updated = shortcutDb->updateShortcut(m_shortcut->id(), name->text(), icon->icon().value().toString(),
                                        link->text(), item->app->id());

if (!updated) {
toast->setToast("Failed to update shortcut");
return;
}

popSelf();
  */
  }
};

class DuplicateShortcutView : public ShortcutFormView {
  std::shared_ptr<Shortcut> m_shortcut;

public:
  DuplicateShortcutView(const std::shared_ptr<Shortcut> &shortcut) : m_shortcut(shortcut) {}

  QString initialNavigationTitle() const override { return "Duplicate Shortcut"; }

  ImageURL initialNavigationIcon() const override {
    return ImageURL::builtin("link").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void initializeForm() override {
    ShortcutFormView::initializeForm();

    auto appDb = context()->services->appDb();

    name->setText(QString("Copy of %1").arg(m_shortcut->name()));
    link->setText(m_shortcut->url());

    if (!iconSelector->setValue(m_shortcut->icon())) {
      // do something if we are dealing with the default value
      blurLink();
    }

    if (auto app = appDb->findById(m_shortcut->app())) { appSelector->setValue(m_shortcut->app()); }
  }

  void onActivate() override { name->selectAll(); }

  void onSubmit() override {
    /*
auto shortcutDb = context()->services->shortcuts();
auto toast = context()->services->toastService();
auto item = static_cast<const AppSelectorItem *>(appSelector->value());

if (!item) {
form()->setError(appSelector, "Required");
return;
}

//auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

if (!icon) {
form()->setError(iconSelector, "Required");
return;
}

if (shortcutDb->createShortcut(name->text(), icon->icon()->toString(), link->text(), item->app->id())) {
toast->setToast("Created shortcut");
popSelf();
} else {
toast->setToast("Failed to create shortcut", ToastStyle::Danger);
}
  */
  }
};

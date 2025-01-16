#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "builtin_icon.hpp"
#include "favicon-fetcher.hpp"
#include "image-fetcher.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/form.hpp"
#include "view.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qtypes.h>

class CallbackAction : public AbstractAction {
  using SubmitHandler = std::function<void(AppWindow &app)>;
  SubmitHandler handler;

public:
  CallbackAction(const SubmitHandler &localHandler)
      : AbstractAction("Submit", {.iconName = "submit"}), handler(localHandler) {}

  void execute(AppWindow &app) override { handler(app); }
};

class AppSelectorItem : public AbstractFormDropdownItem {
public:
  std::shared_ptr<DesktopExecutable> app;
  bool isDefault;

  QIcon icon() const override {
    auto icon = QIcon::fromTheme(app->iconName());

    if (icon.isNull()) return QIcon::fromTheme("application-x-executable");

    return icon;
  }
  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    if (isDefault) name += " (Default)";

    return name;
  }

  AppSelectorItem(const std::shared_ptr<DesktopExecutable> &app, bool isDefault = false)
      : app(app), isDefault(isDefault) {}
};

class AbstractIconSelectorItem : public AbstractFormDropdownItem {
  virtual QString iconName() const = 0;
};

class IconSelectorItem : public AbstractIconSelectorItem {
public:
  QString name;
  QString dname;

  QIcon icon() const override {
    auto icon = QIcon(name);

    if (icon.isNull()) return QIcon::fromTheme("application-x-executable");

    return icon;
  }
  QString displayName() const override {
    if (!dname.isEmpty()) return dname;
    auto ss = name.split('/');

    return ss.at(ss.size() - 1).split('.').at(0);
  }
  QString iconName() const override { return name; }

  IconSelectorItem(const QString &iconName, const QString &displayName = "")
      : name(iconName), dname(displayName) {}
};

class IconSelectorFaviconItem : public AbstractIconSelectorItem {
public:
  QPixmap pixmap;
  QString dname;

  QIcon icon() const override { return pixmap; }
  QString displayName() const override { return dname; }
  QString iconName() const override { return dname; }

  IconSelectorFaviconItem(QPixmap pixmap, const QString &displayName) : pixmap(pixmap), dname(displayName) {}
};

class CreateQuicklinkCommandView : public View {
  Service<AppDatabase> appDb;
  Service<QuicklistDatabase> quicklinkDb;

  FormWidget *form;
  FormInputWidget *name;
  FormInputWidget *link;
  FormDropdown *appSelector;
  FormDropdown *iconSelector;

  std::shared_ptr<AbstractIconSelectorItem> defaultIcon;
  std::shared_ptr<AppSelectorItem> defaultOpener;

  void handleAppSelectorTextChanged(const QString &text) {
    appSelector->model()->beginReset();

    if (defaultOpener && defaultOpener->displayName().contains(text, Qt::CaseInsensitive)) {
      appSelector->model()->addItem(defaultOpener);
    }

    for (const auto &app : appDb.apps) {
      bool appFlag = false;

      if (app->isOpener() && app->name.contains(text, Qt::CaseInsensitive)) {
        appSelector->model()->addItem(std::make_shared<AppSelectorItem>(app));
        appFlag = true;
      }

      for (const auto &app : app->actions) {
        if (!app->isOpener()) continue;

        if (appFlag || app->name.contains(text, Qt::CaseInsensitive)) {
          appSelector->model()->addItem(std::make_shared<AppSelectorItem>(app));
        }
      }
    }

    appSelector->model()->endReset();
  }

  void iconSelectorTextChanged(const QString &text) {
    iconSelector->model()->beginReset();

    if (defaultIcon && QString("default").contains(text, Qt::CaseInsensitive)) {
      iconSelector->model()->addItem(defaultIcon);
    }

    for (const auto &name : BuiltinIconService::icons()) {
      if (name.contains(text, Qt::CaseInsensitive)) {
        iconSelector->model()->addItem(std::make_shared<IconSelectorItem>(name));
      }
    }

    iconSelector->model()->endReset();
  }

  void handleLinkChange(const QString &text) {
    QUrl url(text);

    if (!url.isValid()) return;

    if (auto app = appDb.findBestUrlOpener(url)) {
      auto opener = std::make_shared<AppSelectorItem>(app, true);

      qDebug() << "match" << app->id << "for scheme" << url.scheme();

      if (appSelector->value().get() == defaultOpener.get()) { appSelector->setValue(opener); }

      defaultOpener = opener;
      handleAppSelectorTextChanged(appSelector->searchText());
    }
  }

public:
  CreateQuicklinkCommandView(AppWindow &app)
      : View(app), appDb(service<AppDatabase>()), quicklinkDb(service<QuicklistDatabase>()),
        form(new FormWidget), name(new FormInputWidget("name")), link(new FormInputWidget("link")),
        appSelector(new FormDropdown), iconSelector(new FormDropdown) {
    name->setFocus();
    name->setName("Name");
    form->addInput(name);
    link->setName("URL");
    form->addInput(link);
    form->addInput(appSelector);
    form->addInput(iconSelector);
    appSelector->setName("Open with");
    iconSelector->setName("Icon");

    connect(link, &FormInputWidget::textChanged, this, &CreateQuicklinkCommandView::handleLinkChange);

    connect(appSelector, &FormDropdown::textChanged, this,
            &CreateQuicklinkCommandView::handleAppSelectorTextChanged);
    connect(iconSelector, &FormDropdown::textChanged, this,
            &CreateQuicklinkCommandView::iconSelectorTextChanged);

    defaultIcon = std::make_shared<IconSelectorItem>(":icons/link.svg", "Default");
    iconSelector->setValue(defaultIcon);

    widget = form;
  }

  void loadLink(const Quicklink &quicklink) {
    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.url);

    if (auto app = appDb.getById(quicklink.app)) {
      appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onMount() override {
    hideInput();
    auto submitAction = new CallbackAction([this](AppWindow &app) { submit(app); });

    submitAction->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions({submitAction});

    handleAppSelectorTextChanged("");
    iconSelectorTextChanged("");

    name->focus();
  }

  void submit(AppWindow &app) {
    auto item = std::static_pointer_cast<AppSelectorItem>(appSelector->value());

    if (!item) {
      appSelector->setError("Required");
      return;
    }

    auto icon = std::static_pointer_cast<IconSelectorItem>(iconSelector->value());

    if (!icon) {
      iconSelector->setError("Required");
      return;
    }

    quicklinkDb.insertLink(AddQuicklinkPayload{
        .name = name->text(),
        .icon = icon->iconName(),
        .link = link->text(),
        .app = item->app->id,
    });
    app.statusBar->setToast("Created new quicklink");
    pop();
  }
};

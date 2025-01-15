#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "builtin_icon.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/form.hpp"
#include "view.hpp"
#include <functional>
#include <memory>
#include <qnamespace.h>

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

  QIcon icon() const override {
    auto icon = QIcon::fromTheme(app->iconName());

    if (icon.isNull()) return QIcon::fromTheme("application-x-executable");

    return icon;
  }
  QString displayName() const override { return app->fullyQualifiedName(); }

  AppSelectorItem(const std::shared_ptr<DesktopExecutable> &app) : app(app) {}
};

class IconSelectorItem : public AbstractFormDropdownItem {
public:
  QString iconName;

  QIcon icon() const override {
    auto icon = BuiltinIconService::fromName(iconName);

    if (icon.isNull()) return QIcon::fromTheme("application-x-executable");

    return icon;
  }
  QString displayName() const override { return iconName; }

  IconSelectorItem(const QString &iconName) : iconName(iconName) {}
};

class CreateQuicklinkCommandView : public View {
  Service<AppDatabase> appDb;
  Service<QuicklistDatabase> quicklinkDb;

  FormWidget *form;
  FormInputWidget *name;
  FormInputWidget *link;
  FormDropdown *appSelector;
  FormDropdown *iconSelector;

  void handleAppSelectorTextChanged(const QString &text) {
    appSelector->model()->beginReset();
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

    for (const auto &name : BuiltinIconService::icons()) {
      if (name.contains(text, Qt::CaseInsensitive)) {
        iconSelector->model()->addItem(std::make_shared<IconSelectorItem>(name));
      }
    }

    iconSelector->model()->endReset();
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

    connect(appSelector, &FormDropdown::textChanged, this,
            &CreateQuicklinkCommandView::handleAppSelectorTextChanged);
    connect(iconSelector, &FormDropdown::textChanged, this,
            &CreateQuicklinkCommandView::iconSelectorTextChanged);

    widget = form;
  }

  void loadDuplicate(const Quicklink &quicklink) {
    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.url);

    if (auto app = appDb.getById(quicklink.app)) {
      appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }
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
        .icon = QString(":icons/%1").arg(icon->iconName),
        .link = link->text(),
        .app = item->app->id,
    });
    app.statusBar->setToast("Created new quicklink");
    pop();
  }
};

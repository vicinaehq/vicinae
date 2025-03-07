#pragma once
#include "app-database.hpp"
#include "builtin_icon.hpp"
#include "quicklist-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/form.hpp"
#include "ui/toast.hpp"
#include "view.hpp"
#include <functional>
#include <memory>
#include <numbers>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsharedpointer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

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

  QString icon() const override { return app->iconName(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    return name;
  }

  void setApp(const std::shared_ptr<DesktopExecutable> &app) { this->app = app; }

  QString id() const override { return app->id; }

  AppSelectorItem(const std::shared_ptr<DesktopExecutable> &app) : app(app) {}
};

class DefaultAppItem : public AppSelectorItem {
  QString id() const override { return "default"; }
  QString displayName() const override { return AppSelectorItem::displayName() + " (Default)"; }

public:
  DefaultAppItem(const std::shared_ptr<DesktopExecutable> &app) : AppSelectorItem(app) {}
};

class IconSelectorItem : public AbstractFormDropdownItem {
public:
  QString name;
  QString dname;

  QString icon() const override { return name; }

  QString displayName() const override {
    if (!dname.isEmpty()) return dname;
    auto ss = name.split('/');

    return ss.at(ss.size() - 1).split('.').at(0);
  }

  QString id() const override { return name; }

  void setDisplayName(const QString &name) { this->dname = name; }

  void setIcon(const QString &icon) {
    qDebug() << "set icon to " << icon << "from" << this->name;
    this->name = icon;
  }

  IconSelectorItem(const QString &iconName, const QString &displayName = "")
      : name(iconName), dname(displayName) {}
};

class DefaultIconSelectorItem : public IconSelectorItem {
  QString id() const override { return "default"; }
  QString displayName() const override { return "Default"; }

public:
  DefaultIconSelectorItem(const QString &iconName, const QString &displayName = "")
      : IconSelectorItem(iconName, displayName) {}
};

class QuicklinkCommandView : public View {
  void handleAppSelectorTextChanged(const QString &text) {}

  void iconSelectorTextChanged(const QString &text) {}

  void handleLinkChange(const QString &text) {
    QUrl url(text);

    if (!url.isValid()) return;

    if (auto app = appDb.findBestUrlOpener(url)) {
      appSelector->updateItem("default", [&app](AbstractFormDropdownItem *item) {
        static_cast<AppSelectorItem *>(item)->setApp(app);
      });
    }

    if (!url.scheme().startsWith("http")) return;

    iconSelector->updateItem("default", [&url](AbstractFormDropdownItem *item) {
      auto icon = QString("favicon:%1").arg(url.host());
      auto iconItem = static_cast<IconSelectorItem *>(item);

      iconItem->setIcon(icon);
      iconItem->setDisplayName(url.host());
    });
  }

  void appSelectionChanged(const AbstractFormDropdownItem &item) {
    auto &appItem = static_cast<const AppSelectorItem &>(item);

    iconSelector->updateItem("default", [appItem](AbstractFormDropdownItem *item) {
      auto icon = static_cast<IconSelectorItem *>(item);

      icon->setIcon(appItem.icon());
      icon->setDisplayName(appItem.displayName());
    });
  }

protected:
  Service<AppDatabase> appDb;
  Service<QuicklistDatabase> quicklinkDb;

  FormWidget *form;
  FormInputWidget *name;
  FormInputWidget *link;
  FormDropdown *appSelector;
  FormDropdown *iconSelector;

public:
  QuicklinkCommandView(AppWindow &app)
      : View(app), appDb(service<AppDatabase>()), quicklinkDb(service<QuicklistDatabase>()),
        form(new FormWidget), name(new FormInputWidget("name")), link(new FormInputWidget("link")),
        appSelector(new FormDropdown), iconSelector(new FormDropdown) {
    name->setFocus();
    name->setName("Name");
    name->setPlaceholderText("Quicklink name");
    form->addInput(name);
    link->setName("URL");
    link->setPlaceholderText("https://google.com/search?q={argument}");
    form->addInput(link);
    form->addInput(appSelector);
    form->addInput(iconSelector);
    appSelector->setName("Open with");
    iconSelector->setName("Icon");

    connect(link, &FormInputWidget::textChanged, this, &QuicklinkCommandView::handleLinkChange);

    connect(appSelector, &FormDropdown::textChanged, this,
            &QuicklinkCommandView::handleAppSelectorTextChanged);
    connect(iconSelector, &FormDropdown::textChanged, this, &QuicklinkCommandView::iconSelectorTextChanged);
    connect(appSelector, &FormDropdown::selectionChanged, this, &QuicklinkCommandView::appSelectionChanged);

    appSelector->beginUpdate();
    appSelector->addSection("Apps");

    if (auto browser = appDb.defaultBrowser()) {
      appSelector->addItem(std::make_unique<DefaultAppItem>(browser));
    }

    for (const auto &app : appDb.apps) {
      appSelector->addItem(std::make_unique<AppSelectorItem>(app));

      for (const auto &action : app->actions) {
        appSelector->addItem(std::make_unique<AppSelectorItem>(action));
      }
    }

    appSelector->commitUpdate();
    appSelector->setValue("default");

    iconSelector->beginUpdate();
    iconSelector->addItem(std::make_unique<DefaultIconSelectorItem>(":icons/link.svg", "Default"));

    for (const auto &name : BuiltinIconService::icons()) {
      iconSelector->addItem(std::make_unique<IconSelectorItem>(name));
    }

    iconSelector->commitUpdate();
    iconSelector->setValue("default");

    form->setContentsMargins(0, 10, 0, 0);
    widget = form;
  }

  void loadLink(const Quicklink &quicklink) {
    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.rawUrl);

    if (auto app = appDb.getById(quicklink.app)) {
      // appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onMount() override {
    hideInput();
    auto submitAction = new CallbackAction([this](AppWindow &app) { submit(app); });

    submitAction->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions({submitAction});

    name->focus();
  }

  virtual void submit(AppWindow &app) {
    auto item = static_cast<const AppSelectorItem *>(appSelector->value());

    if (!item) {
      appSelector->setError("Required");
      return;
    }

    auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

    if (!icon) {
      iconSelector->setError("Required");
      return;
    }

    quicklinkDb.insertLink(AddQuicklinkPayload{
        .name = name->text(),
        .icon = icon->icon(),
        .link = link->text(),
        .app = item->app->id,
    });
    app.statusBar->setToast("Created new quicklink");
    pop();
  }
};

class EditCommandQuicklinkView : public QuicklinkCommandView {
  Q_OBJECT

  const Quicklink &quicklink;

public:
  EditCommandQuicklinkView(AppWindow &app, const Quicklink &quicklink)
      : QuicklinkCommandView(app), quicklink(quicklink) {
    name->setText(quicklink.name);
    link->setText(quicklink.rawUrl);

    if (auto app = appDb.getById(quicklink.app)) {
      // appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName, quicklink.iconName));
  }

  void submit(AppWindow &app) override {
    auto item = static_cast<const AppSelectorItem *>(appSelector->value());

    if (!item) {
      appSelector->setError("Required");
      return;
    }

    auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

    if (!icon) {
      iconSelector->setError("Required");
      return;
    }

    bool updateResult = quicklinkDb.updateLink(UpdateQuicklinkPayload{
        .id = quicklink.id,
        .name = name->text(),
        .icon = icon->icon(),
        .link = link->text(),
        .app = item->app->id,
    });

    if (!updateResult) {
      app.statusBar->setToast("Failed to update result", ToastPriority::Danger);
      return;
    }

    pop();
    emit quicklinkEdited();
  }

signals:
  void quicklinkEdited();
};

class DuplicateQuicklinkCommandView : public QuicklinkCommandView {
  Q_OBJECT

public:
  DuplicateQuicklinkCommandView(AppWindow &app, const Quicklink &quicklink) : QuicklinkCommandView(app) {
    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.rawUrl);

    if (auto app = appDb.getById(quicklink.app)) {
      // appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onMount() override {
    QuicklinkCommandView::onMount();
    name->selectAll();
  }

  void submit(AppWindow &app) override {
    auto item = static_cast<const AppSelectorItem *>(appSelector->value());

    if (!item) {
      appSelector->setError("Required");
      return;
    }

    auto icon = static_cast<const IconSelectorItem *>(iconSelector->value());

    if (!icon) {
      iconSelector->setError("Required");
      return;
    }

    bool insertResult = quicklinkDb.insertLink(AddQuicklinkPayload{
        .name = name->text(),
        .icon = icon->icon(),
        .link = link->text(),
        .app = item->app->id,
    });

    if (!insertResult) {
      app.statusBar->setToast("Failed to create quicklink", ToastPriority::Danger);
      return;
    }

    pop();

    emit duplicated();
  }

signals:
  void duplicated();
};

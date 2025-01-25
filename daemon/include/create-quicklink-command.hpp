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
#include <qnamespace.h>
#include <qpixmap.h>
#include <qtmetamacros.h>
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

  QString icon() const override { return app->iconName(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    if (isDefault) name += " (Default)";

    return name;
  }

  int role() const override { return 0; }

  size_t id() const override { return qHash(app->id); }

  AppSelectorItem(const std::shared_ptr<DesktopExecutable> &app, bool isDefault = false)
      : app(app), isDefault(isDefault) {}
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

  size_t id() const override { return qHash(name); }

  int role() const override { return 0; }

  IconSelectorItem(const QString &iconName, const QString &displayName = "")
      : name(iconName), dname(displayName) {}
};

class QuicklinkCommandView : public View {
  void handleAppSelectorTextChanged(const QString &text) {
    auto model = appSelector->model();

    model->beginReset();

    if (defaultOpener && defaultOpener->displayName().contains(text, Qt::CaseInsensitive)) {
      model->addItem(defaultOpener);
    }

    for (const auto &app : appDb.apps) {
      bool appFlag = false;

      if (app->isOpener() && app->name.contains(text, Qt::CaseInsensitive)) {
        model->addItem(std::make_shared<AppSelectorItem>(app));
        appFlag = true;
      }

      for (const auto &app : app->actions) {
        if (!app->isOpener()) continue;

        if (appFlag || app->name.contains(text, Qt::CaseInsensitive)) {
          model->addItem(std::make_shared<AppSelectorItem>(app));
        }
      }
    }

    model->endReset();
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

      if (appSelector->value().get() == defaultOpener.get()) {
        appSelector->setValue(opener);
        appSelectionChanged(opener);
      }

      defaultOpener = opener;
      handleAppSelectorTextChanged(appSelector->searchText());
    }

    auto icon = std::make_shared<IconSelectorItem>(QString("favicon:%1").arg(url.host()), url.host());

    if (iconSelector->value()->id() == defaultIcon->id()) {
      defaultIcon = icon;
      iconSelectorTextChanged(iconSelector->searchText());
      iconSelector->setValue(defaultIcon);
    }
  }

  void appSelectionChanged(const std::shared_ptr<AbstractFormDropdownItem> &item) {
    auto appItem = std::static_pointer_cast<AppSelectorItem>(item);
    bool isCurrentDefault = iconSelector->value()->id() == defaultIcon->id();

    defaultIcon = std::make_shared<IconSelectorItem>(appItem->app->iconName(),
                                                     QString("%1 (Default)").arg(appItem->app->name));

    iconSelectorTextChanged(iconSelector->searchText());

    if (isCurrentDefault) { iconSelector->setValue(defaultIcon); }
  }

protected:
  Service<AppDatabase> appDb;
  Service<QuicklistDatabase> quicklinkDb;

  FormWidget *form;
  FormInputWidget *name;
  FormInputWidget *link;
  FormDropdown *appSelector;
  FormDropdown *iconSelector;

  std::shared_ptr<IconSelectorItem> defaultIcon;
  std::shared_ptr<AppSelectorItem> defaultOpener;

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

    defaultIcon = std::make_shared<IconSelectorItem>(":icons/link.svg", "Default");
    iconSelector->setValue(defaultIcon);

    if (auto browser = appDb.defaultBrowser()) {
      auto opener = std::make_shared<AppSelectorItem>(browser, true);

      appSelector->setValue(opener);
      defaultOpener = opener;
    }

    handleAppSelectorTextChanged("");
    iconSelectorTextChanged("");

    form->setContentsMargins(0, 10, 0, 0);
    widget = form;
  }

  void loadLink(const Quicklink &quicklink) {
    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.rawUrl);

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

    name->focus();
  }

  virtual void submit(AppWindow &app) {
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
      appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName, quicklink.iconName));
  }

  void submit(AppWindow &app) override {
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
      appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onMount() override {
    QuicklinkCommandView::onMount();
    name->selectAll();
  }

  void submit(AppWindow &app) override {
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

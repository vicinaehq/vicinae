#pragma once
#include "omni-icon.hpp"
#include "quicklist-database.hpp"
#include "service-registry.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/selector-input.hpp"
#include "ui/toast.hpp"
#include "view.hpp"
#include "ui/form/form.hpp"
#include <functional>
#include <memory>
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsharedpointer.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>
#include <sched.h>

class CallbackAction : public AbstractAction {
  using SubmitHandler = std::function<void(AppWindow &app)>;
  SubmitHandler handler;

public:
  CallbackAction(const SubmitHandler &localHandler)
      : AbstractAction("Submit", BuiltinOmniIconUrl("submit")), handler(localHandler) {}

  void execute(AppWindow &app) override { handler(app); }
};

class AppSelectorItem : public SelectorInput::AbstractItem {
public:
  std::shared_ptr<Application> app;
  bool isDefault;

  OmniIconUrl icon() const override { return app->iconUrl(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    return name;
  }

  AbstractItem *clone() const override { return new AppSelectorItem(*this); }

  void setApp(const std::shared_ptr<Application> &app) { this->app = app; }

  QString id() const override { return app->id(); }

  AppSelectorItem(const std::shared_ptr<Application> &app) : app(app) {}
};

class DefaultAppItem : public AppSelectorItem {
  QString id() const override { return "default"; }
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

  OmniIconUrl icon() const override { return iconUrl; }

  QString displayName() const override { return iconUrl.name(); }

  QString id() const override { return iconUrl.toString(); }

  void setDisplayName(const QString &name) { this->dname = name; }

  AbstractItem *clone() const override { return new IconSelectorItem(*this); }

  void setIcon(const OmniIconUrl &url) { this->iconUrl = url; }

  IconSelectorItem(const OmniIconUrl &url, const QString &displayName = "")
      : dname(displayName), iconUrl(url) {}
};

class DefaultIconSelectorItem : public IconSelectorItem {
  QString id() const override { return "default"; }
  QString displayName() const override { return "Default"; }
  AbstractItem *clone() const override { return new DefaultIconSelectorItem(*this); }

public:
  DefaultIconSelectorItem(const OmniIconUrl &url, const QString &displayName = "")
      : IconSelectorItem(url, displayName) {}
};

class QuicklinkCommandView : public View {
  void handleAppSelectorTextChanged(const QString &text) {}

  void iconSelectorTextChanged(const QString &text) {}

  void handleLinkChange(const QString &text) {
    auto appDb = ServiceRegistry::instance()->appDb();
    QUrl url(text);

    if (auto app = appDb->findBestOpener(text)) {
      appSelector->updateItem("default", [&app](SelectorInput::AbstractItem *item) {
        static_cast<AppSelectorItem *>(item)->setApp(app);
      });
    }

    if (!url.scheme().startsWith("http")) return;

    iconSelector->updateItem("default", [&url](SelectorInput::AbstractItem *item) {
      auto icon = FaviconOmniIconUrl(url.host()).withFallback(BuiltinOmniIconUrl("image"));
      auto iconItem = static_cast<IconSelectorItem *>(item);

      iconItem->setIcon(icon);
      iconItem->setDisplayName(url.host());
    });
  }

  void appSelectionChanged(const SelectorInput::AbstractItem &item) {
    auto &appItem = static_cast<const AppSelectorItem &>(item);

    iconSelector->updateItem("default", [appItem](SelectorInput::AbstractItem *item) {
      auto icon = static_cast<IconSelectorItem *>(item);

      icon->setIcon(appItem.icon());
      icon->setDisplayName(appItem.displayName());
    });
  }

protected:
  FormWidget *form;
  BaseInput *name;
  BaseInput *link;
  SelectorInput *appSelector;
  SelectorInput *iconSelector;

public:
  QuicklinkCommandView(AppWindow &app)
      : View(app), form(new FormWidget), name(new BaseInput), link(new BaseInput),
        appSelector(new SelectorInput), iconSelector(new SelectorInput) {
    auto appDb = ServiceRegistry::instance()->appDb();
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();

    name->setPlaceholderText("Quicklink name");
    link->setPlaceholderText("https://google.com/search?q={argument}");

    auto nameField = new FormField;
    auto linkField = new FormField;
    auto openField = new FormField;
    auto iconField = new FormField;

    nameField->setName("Name");
    nameField->setWidget(name, name->focusNotifier());
    linkField->setName("URL");
    linkField->setWidget(link, link->focusNotifier());
    linkField->setInfo("This is a rather long description talking about what you can do with this wonderful "
                       "item. And wait, there is even `more`. **Amazing**, isn't it.");
    openField->setName("Open with");
    openField->setWidget(appSelector, appSelector->focusNotifier());
    openField->setInfo("The *application* with which the link will be opened");
    iconField->setName("Icon");
    iconField->setWidget(iconSelector, iconSelector->focusNotifier());

    form->addField(nameField);
    form->addField(linkField);
    form->addField(openField);
    form->addField(iconField);

    connect(link, &BaseInput::textChanged, this, &QuicklinkCommandView::handleLinkChange);
    connect(appSelector, &SelectorInput::textChanged, this,
            &QuicklinkCommandView::handleAppSelectorTextChanged);
    connect(iconSelector, &SelectorInput::textChanged, this, &QuicklinkCommandView::iconSelectorTextChanged);
    connect(appSelector, &SelectorInput::selectionChanged, this, &QuicklinkCommandView::appSelectionChanged);

    appSelector->beginUpdate();

    if (auto browser = appDb->webBrowser()) {
      appSelector->addItem(std::make_unique<DefaultAppItem>(browser));
    }

    for (const auto &app : appDb->list()) {
      appSelector->addItem(std::make_unique<AppSelectorItem>(app));

      for (const auto &action : app->actions()) {
        appSelector->addItem(std::make_unique<AppSelectorItem>(action));
      }
    }

    appSelector->commitUpdate();
    appSelector->setValue("default");

    iconSelector->beginUpdate();
    iconSelector->addItem(std::make_unique<DefaultIconSelectorItem>(BuiltinOmniIconUrl("link"), "Default"));

    for (const auto &name : BuiltinIconService::icons()) {
      iconSelector->addItem(std::make_unique<IconSelectorItem>(BuiltinOmniIconUrl(name)));
    }

    iconSelector->commitUpdate();
    iconSelector->setValue("default");

    form->setContentsMargins(0, 10, 0, 0);

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(form);
    setLayout(layout);
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

  void onMount() override {
    hideInput();
    auto submitAction = new CallbackAction([this](AppWindow &app) { submit(app); });

    submitAction->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions({submitAction});

    form->focusFirst();
  }

  virtual void submit(AppWindow &app) {
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();

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

    quicklinkDb->insertLink(AddQuicklinkPayload{
        .name = name->text(),
        .icon = icon->icon().toString(),
        .link = link->text(),
        .app = item->app->id(),
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
    auto appDb = ServiceRegistry::instance()->appDb();

    name->setText(quicklink.name);
    link->setText(quicklink.rawUrl);

    if (auto app = appDb->findById(quicklink.app)) {
      // appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName, quicklink.iconName));
  }

  void submit(AppWindow &app) override {
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
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

    bool updateResult = quicklinkDb->updateLink(UpdateQuicklinkPayload{
        .id = quicklink.id,
        .name = name->text(),
        .icon = icon->icon().toString(),
        .link = link->text(),
        .app = item->app->id(),
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
    auto appDb = ServiceRegistry::instance()->appDb();

    name->setText(QString("Copy of %1").arg(quicklink.name));
    link->setText(quicklink.rawUrl);

    if (auto app = appDb->findById(quicklink.app)) {
      // appSelector->setValue(std::make_shared<AppSelectorItem>(app));
    }

    // iconSelector->setValue(std::make_shared<IconSelectorItem>(quicklink.iconName));
  }

  void onMount() override {
    QuicklinkCommandView::onMount();
    name->selectAll();
  }

  void submit(AppWindow &app) override {
    auto quicklinkDb = ServiceRegistry::instance()->quicklinks();
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

    bool insertResult = quicklinkDb->insertLink(AddQuicklinkPayload{
        .name = name->text(),
        .icon = icon->icon().toString(),
        .link = link->text(),
        .app = item->app->id(),
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

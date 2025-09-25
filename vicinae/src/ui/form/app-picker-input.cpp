#include "ui/form/app-picker-input.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/omni-list/omni-list.hpp"

class AppItem : public SelectorInput::AbstractItem {
public:
  std::shared_ptr<AbstractApplication> app;
  bool isDefault;

  std::optional<ImageURL> icon() const override { return app->iconUrl(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    return name;
  }

  AbstractItem *clone() const override { return new AppItem(*this); }

  void setApp(const std::shared_ptr<AbstractApplication> &app) { this->app = app; }

  QString generateId() const override { return app->id(); }

  AppItem(const std::shared_ptr<AbstractApplication> &app) : app(app) {}
};

AppPickerInput::AppPickerInput(const AbstractAppDatabase *appDb) : m_appDb(appDb) {
  auto filter = [](auto &&app) { return app->displayable(); };

  list()->updateModel([&]() {
    auto &section = list()->addSection();

    for (const auto &app : m_appDb->list()) {
      if (!app->displayable()) continue;

      section.addItem(std::make_unique<AppItem>(app));
    }
  });
}

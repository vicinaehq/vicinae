#include "ui/form/app-picker-input.hpp"
#include "app/app-database.hpp"

class AppItem : public SelectorInput::AbstractItem {
public:
  std::shared_ptr<Application> app;
  bool isDefault;

  OmniIconUrl icon() const override { return app->iconUrl(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    return name;
  }

  AbstractItem *clone() const override { return new AppItem(*this); }

  void setApp(const std::shared_ptr<Application> &app) { this->app = app; }

  QString id() const override { return app->id(); }

  AppItem(const std::shared_ptr<Application> &app) : app(app) {}
};

AppPickerInput::AppPickerInput(const AbstractAppDatabase *appDb) : m_appDb(appDb) {
  beginUpdate();

  for (const auto &app : m_appDb->list()) {
    addItem(std::make_unique<AppItem>(app));
  }

  commitUpdate();
}

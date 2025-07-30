#include "ui/form/app-picker-input.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <ranges>

class AppItem : public SelectorInput::AbstractItem {
public:
  std::shared_ptr<Application> app;
  bool isDefault;

  std::optional<OmniIconUrl> icon() const override { return app->iconUrl(); }

  QString displayName() const override {
    QString name = app->fullyQualifiedName();

    return name;
  }

  AbstractItem *clone() const override { return new AppItem(*this); }

  void setApp(const std::shared_ptr<Application> &app) { this->app = app; }

  QString generateId() const override { return app->id(); }

  AppItem(const std::shared_ptr<Application> &app) : app(app) {}
};

AppPickerInput::AppPickerInput(const AbstractAppDatabase *appDb) : m_appDb(appDb) {
  auto filter = [](auto &&app) { return app->displayable(); };
  auto map = [](auto &&app) -> std::unique_ptr<OmniList::AbstractVirtualItem> {
    return std::make_unique<AppItem>(app);
  };

  list()->updateModel([&]() {
    auto items = m_appDb->list() | std::views::filter(filter) | std::views::transform(map) |
                 std::ranges::to<std::vector>();
    auto &section = list()->addSection();

    section.addItems(std::move(items));
  });
}

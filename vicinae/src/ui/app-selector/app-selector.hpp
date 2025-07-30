#include "services/app-service/abstract-app-db.hpp"
#include "ui/form/selector-input.hpp"
#include <qwidget.h>

class AppSelectorItem : public SelectorInput::AbstractItem {
  std::shared_ptr<Application> m_app;

  QString displayName() const override { return m_app->name(); }

  QString generateId() const override { return m_app->id(); }

  AbstractItem *clone() const override { return new AppSelectorItem(*this); }

public:
  Application *app() const { return m_app.get(); }
  AppSelectorItem(const std::shared_ptr<Application> &app) : m_app(app) {}
};

class AppSelector : public SelectorInput {
public:
  AppSelector(QWidget *parent = nullptr);
  void setApps(const std::vector<std::shared_ptr<Application>> &apps);
  AppSelectorItem const *value() const;
};

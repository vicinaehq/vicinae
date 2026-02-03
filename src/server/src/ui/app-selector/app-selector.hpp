#include "services/app-service/abstract-app-db.hpp"
#include "ui/form/selector-input.hpp"
#include <qwidget.h>

class AppSelectorItem2 : public SelectorInput::AbstractItem {
  std::shared_ptr<AbstractApplication> m_app;

  QString displayName() const override { return m_app->displayName(); }

  QString generateId() const override { return m_app->id(); }

  AbstractItem *clone() const override { return new AppSelectorItem2(*this); }

public:
  AbstractApplication *app() const { return m_app.get(); }
  AppSelectorItem2(const std::shared_ptr<AbstractApplication> &app) : m_app(app) {}
};

class AppSelector : public SelectorInput {
public:
  AppSelector(QWidget *parent = nullptr);
  void setApps(const std::vector<std::shared_ptr<AbstractApplication>> &apps);
  AppSelectorItem2 const *value() const;
};

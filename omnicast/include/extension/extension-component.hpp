#pragma once
#include "app.hpp"
#include "base-view.hpp"
#include "extend/action-model.hpp"
#include "extend/model-parser.hpp"
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ActionPannelWidget;

template <typename T, typename Variant>
concept VariantMember = requires { std::get<T>(std::declval<Variant>()); };

template <typename T>
  requires VariantMember<T, RenderModel>
class RenderableExtensionView {
  virtual void render(const T &model) = 0;
  virtual int modelIndex() const { return RenderModel(T{}).index(); }
};

template <typename T>
concept RequiresView = std::derived_from<T, BaseView>;

template <RequiresView T> class ExtensionRootView : public T {
  void hello() {
    BaseView *e = new T();

    ExtensionRootView *extended = e;
  }
};

class AbstractExtensionRootComponent : public QWidget {
  Q_OBJECT
  AppWindow &app;

  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    emit componentShown();
    qCritical() << "show event component";
  }

public:
  AbstractExtensionRootComponent(AppWindow &app) : app(app) {}

  virtual bool inputFilter(QKeyEvent *event) { return false; }

  virtual void onMount() {}

  QString searchText() const { return ""; }
  void setSearchText(const QString &text) {}

  virtual void onSearchChanged(const QString &text) {}

  ActionPannelWidget *actionPannel() const { return nullptr; }

  virtual void render(const RenderModel &model) = 0;

signals:
  void notifyEvent(const QString &handler, const QJsonArray &args) const;
  void updateActionPannel(const ActionPannelModel &model) const;
  void componentShown() const;
  void setNavigationTitle(const QString &text);
  void setSearchPlaceholderText(const QString &text);
  void setSearchAccessory(QWidget *accessory);
  void selectPrimaryAction();
  void setLoading(bool loading);
};

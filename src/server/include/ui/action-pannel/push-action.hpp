#pragma once
#include "ui/action-pannel/action.hpp"
#include "navigation-controller.hpp"
#include "ui/image/url.hpp"

template <typename T, typename... Args> class PushAction : public AbstractAction {
  std::tuple<Args...> m_viewArgs;

  void execute(ApplicationContext *ctx) override {
    std::apply(
        [&](auto &&...args) { ctx->navigation->pushView(new T(std::forward<decltype(args)>(args)...)); },
        m_viewArgs);
    ctx->navigation->setNavigationTitle(navigationTitle());
    ctx->navigation->setNavigationIcon(navigationIcon());
  }

  virtual QString title() const override = 0;
  virtual std::optional<ImageURL> icon() const override = 0;

  virtual QString navigationTitle() const { return title(); }
  virtual QString navigationIcon() const {
    if (auto i = icon()) { return i.value(); }
    return QString();
  }

public:
  PushAction(Args... args) : m_viewArgs(std::make_tuple(args...)) {}
};

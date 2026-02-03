#pragma once
#include "ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/base-view.hpp"
#include "navigation-controller.hpp"

class PushViewAction : public AbstractAction {
  BaseView *m_view;
  ImageURL m_icon;

public:
  void execute(ApplicationContext *ctx) override {
    ctx->navigation->pushView(m_view);
    ctx->navigation->setNavigationTitle(title());
    ctx->navigation->setNavigationIcon(m_icon);
  }

  PushViewAction(const QString &title, BaseView *view, const ImageURL &icon)
      : AbstractAction(title, icon), m_view(view), m_icon(icon) {}
};

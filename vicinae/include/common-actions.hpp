#pragma once
#include "common.hpp"
#include "../src/ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include <qobjectdefs.h>

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

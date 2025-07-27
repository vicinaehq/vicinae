#pragma once
#include "common.hpp"
#include "omni-icon.hpp"
#include "ui/action-pannel/action.hpp"
#include <qobjectdefs.h>

class PushViewAction : public AbstractAction {
  BaseView *m_view;
  OmniIconUrl m_icon;

public:
  void execute(ApplicationContext *ctx) override {
    ctx->navigation->pushView(m_view);
    ctx->navigation->setNavigationTitle(title());
    ctx->navigation->setNavigationIcon(m_icon);
  }

  PushViewAction(const QString &title, BaseView *view, const OmniIconUrl &icon)
      : AbstractAction(title, icon), m_view(view), m_icon(icon) {}
};

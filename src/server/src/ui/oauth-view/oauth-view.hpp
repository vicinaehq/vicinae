#pragma once
#include "ui/views/base-view.hpp"
#include "common.hpp"
#include "extension/manager/extension-manager.hpp"
#include "proto/oauth.pb.h"
#include "ui/overlay/overlay.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <qwidget.h>

class OAuthView : public OverlayView {
public:
  OAuthView(const ApplicationContext *ctx, const proto::ext::oauth::AuthorizeRequest &reqData);
  void showSuccess();

private:
  void handleError(const QString &error);
  void abort();
  void keyPressEvent(QKeyEvent *key) override;

  ImageURL m_providerIcon;
  QStackedWidget *m_view = new QStackedWidget;
  QWidget *m_initView = nullptr;
  QWidget *m_successView = nullptr;
  ExtensionRequest *m_request;
  const ApplicationContext *m_ctx = nullptr;
};

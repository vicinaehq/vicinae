#pragma once
#include "ui/views/base-view.hpp"
#include "common.hpp"
#include "extension/manager/extension-manager.hpp"
#include "image/url.hpp"
#include "vicinae.hpp"
#include "proto/oauth.pb.h"
#include "services/oauth/oauth-service.hpp"
#include "theme.hpp"
#include "navigation-controller.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/overlay/overlay.hpp"
#include "ui/toast/toast.hpp"
#include "utils/layout.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "services/app-service/app-service.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>

class OAuthView : public OverlayView {
  QFutureWatcher<OAuthResponse> m_watcher;
  ExtensionRequest *m_request;
  proto::ext::oauth::AuthorizeRequest m_reqData;
  ApplicationContext *m_ctx = nullptr;

  void handleError(const QString &error) {
    auto toast = m_ctx->services->toastService();

    toast->setToast(error, ToastStyle::Danger);
    m_request->respondWithError(error);
  }

  void handleFinished() {
    if (m_watcher.isCanceled()) { return; }

    auto result = m_watcher.result();

    auto toast = m_ctx->services->toastService();

    if (!result) {
      handleError(result.error());
    } else {
      auto res = new proto::ext::extension::Response;
      auto resData = new proto::ext::extension::ResponseData;
      auto oauthRes = new proto::ext::oauth::Response;
      auto authorizeRes = new proto::ext::oauth::AuthorizeResponse;

      res->set_allocated_data(resData);
      resData->set_allocated_oauth(oauthRes);
      oauthRes->set_allocated_authorize(authorizeRes);
      authorizeRes->set_code(result.value().code.toStdString());
      m_request->respond(res);
    }
  }

  void abort() {
    m_ctx->navigation->popToRoot();
    dismiss();
  }

  void keyPressEvent(QKeyEvent *key) override {
    if (key->key() == Qt::Key_Escape) {
      abort();
      return;
    }

    QWidget::keyPressEvent(key);
  }

public:
  OAuthView(ApplicationContext *ctx, ExtensionRequest *request,
            const proto::ext::oauth::AuthorizeRequest &reqData)
      : m_ctx(ctx), m_request(request), m_reqData(reqData) {
    setFocusPolicy(Qt::StrongFocus);

    auto oauth = m_ctx->services->oauthService();
    auto data = OAuthRequestData::fromUrl(QUrl(m_reqData.url().c_str()));

    m_watcher.setFuture(oauth->request(OAuthClient::fromProto(m_reqData.client()), data));

    connect(&m_watcher, &QFutureWatcher<OAuthResponse>::finished, this, &OAuthView::handleFinished);

    auto backButton = new IconButton;

    backButton->setFixedSize(25, 25);
    backButton->setUrl(ImageURL::builtin("arrow-left"));
    backButton->setBackgroundColor(SemanticColor::ListItemSelectionBackground);

    connect(backButton, &IconButton::clicked, this, &OAuthView::abort);

    auto header = HStack().add(backButton).addStretch().margins(15, 5, 15, 5).buildWidget();

    header->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);

    auto &client = reqData.client();

    QString url = reqData.url().c_str();

    auto helpText = QString("Need to open in another browser? <a href=\"%1\">Copy authorization link</a>")
                        .arg(reqData.url().c_str());

    auto clicked = [this, url]() {
      // m_ctx->navigation->popToRoot();
      // dismiss();
      m_ctx->services->appDb()->openTarget(url);
    };
    ImageURL iconUrl;

    if (reqData.client().has_icon()) { iconUrl = reqData.client().icon(); }

    auto content = HStack()
                       .add(VStack()
                                .add(UI::Icon(iconUrl).size({40, 40}), 0, Qt::AlignCenter)
                                .add(UI::Text(client.name().c_str()).title().align(Qt::AlignHCenter))
                                .add(UI::Text(client.description().c_str()).align(Qt::AlignHCenter))
                                .add(UI::Button(QString("OAuth is not supported yet"))
                                         .danger()
                                         .disabled()
                                         .onClick(clicked),
                                     0, Qt::AlignHCenter)
                                .addStretch()
                                .add(UI::Text(helpText).secondary().align(Qt::AlignHCenter).autoEllide(false))
                                .spacing(20))
                       .margins(40, 40, 40, 40);

    VStack().add(header).add(content).imbue(this);
  }
};

#pragma once
#include "base-view.hpp"
#include "common.hpp"
#include "extension/manager/extension-manager.hpp"
#include "omni-icon.hpp"
#include "omnicast.hpp"
#include "proto/oauth.pb.h"
#include "services/oauth/oauth-service.hpp"
#include "theme.hpp"
#include "ui/dialog.hpp"
#include "ui/icon-button.hpp"
#include "ui/omni-button.hpp"
#include "ui/overlay/overlay.hpp"
#include "ui/toast.hpp"
#include "ui/typography/typography.hpp"
#include "utils/layout.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include <qfuture.h>
#include <qfuturewatcher.h>
#include <qlogging.h>
#include <qnamespace.h>

class OAuthView : public OverlayView {
  QFutureWatcher<OAuthResponse> m_watcher;
  ExtensionRequest *m_request;
  proto::ext::oauth::AuthorizeRequest m_reqData;
  ApplicationContext *m_ctx = nullptr;

  void handleError(const QString &error) {
    auto toast = m_ctx->services->toastService();

    toast->setToast(error, ToastPriority::Danger);
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

  void keyPressEvent(QKeyEvent *key) override {
    qDebug() << "OAUTH KEY!!!";
    if (key->key() == Qt::Key_Escape) {
      m_ctx->navigation->popToRoot();
      dismiss();
      return;
    }

    QWidget::keyPressEvent(key);
  }

  /*
  void paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    painter.fillRect(rect(), QColor("red"));

    QWidget::paintEvent(event);
  }
  */

public:
  OAuthView(ApplicationContext *ctx, ExtensionRequest *request,
            const proto::ext::oauth::AuthorizeRequest &reqData)
      : m_ctx(ctx), m_request(request), m_reqData(reqData) {
    setMouseTracking(true);

    setFocusPolicy(Qt::StrongFocus);

    auto oauth = m_ctx->services->oauthService();
    auto data = OAuthRequestData::fromUrl(QUrl(m_reqData.url().c_str()));

    qDebug() << "url" << m_reqData.url().c_str();

    m_watcher.setFuture(oauth->request(OAuthClient::fromProto(m_reqData.client()), data));

    connect(&m_watcher, &QFutureWatcher<OAuthResponse>::finished, this, &OAuthView::handleFinished);

    auto backButton = new IconButton;

    backButton->setFixedSize(25, 25);
    backButton->setUrl(BuiltinOmniIconUrl("arrow-left"));
    backButton->setBackgroundColor(SemanticColor::MainSelectedBackground);

    auto header = HStack().add(backButton).addStretch().margins(15, 5, 15, 5).buildWidget();

    header->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);

    auto &client = reqData.client();

    auto need = new TypographyWidget;
    QString url = reqData.url().c_str();

    need->setColor(SemanticColor::TextSecondary);
    need->setText(QString("Need to open in another browser? <a href=\"%1\">Copy authorization link</a>")
                      .arg(reqData.url().c_str()));
    need->setAutoEllide(false);

    auto btn = new OmniButtonWidget;

    btn->setText(QString("Sign in with %1").arg(client.name().c_str()));
    btn->setBackgroundColor(SemanticColor::MainSelectedBackground);
    btn->setHoverBackgroundColor(SemanticColor::MainHoverBackground);

    connect(btn, &OmniButtonWidget::clicked, this,
            [this, url]() { m_ctx->services->appDb()->openTarget(url); });

    connect(backButton, &IconButton::clicked, this, [this]() {
      m_ctx->navigation->popToRoot();
      dismiss();
    });

    auto content = HStack()
                       .add(VStack()
                                .addIcon(BuiltinOmniIconUrl("omnicast"), {40, 40}, Qt::AlignHCenter)
                                .addTitle(client.name().c_str(), SemanticColor::TextPrimary, Qt::AlignHCenter)
                                .addText("Connect to your account", SemanticColor::TextPrimary,
                                         TextSize::TextRegular, Qt::AlignHCenter)
                                .add(btn, 0, Qt::AlignHCenter)
                                .addStretch()
                                .add(HStack().addStretch().add(need).addStretch())
                                .spacing(20))
                       .margins(40, 40, 40, 40);

    VStack().add(header).add(content).imbue(this);
  }
};

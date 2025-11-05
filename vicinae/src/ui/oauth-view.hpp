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
  const ApplicationContext *m_ctx = nullptr;

  void handleError(const QString &error) {
    auto toast = m_ctx->services->toastService();

    toast->setToast(error, ToastStyle::Danger);
    m_request->respondWithError(error);
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
  OAuthView(const ApplicationContext *ctx, const proto::ext::oauth::AuthorizeRequest &reqData)
      : m_ctx(ctx), m_reqData(reqData) {
    setFocusPolicy(Qt::StrongFocus);

    auto oauth = m_ctx->services->oauthService();
    auto data = OAuthRequestData::fromUrl(QUrl(m_reqData.url().c_str()));

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

  void showSuccess() {
    using namespace std::chrono_literals;

    auto &client = m_reqData.client();
    auto content =
        HStack()
            .add(VStack()
                     .add(UI::Icon(ImageURL::builtin("check")).size({40, 40}), 0, Qt::AlignCenter)
                     .add(UI::Text(client.name().c_str()).title().align(Qt::AlignHCenter))
                     .add(UI::Text(QString("Successfully connected to %1").arg(client.name().c_str()))
                              .align(Qt::AlignHCenter))
                     .addStretch()
                     .spacing(20))
            .margins(40, 40, 40, 40);

    VStack().add(content).imbue(this);

    QTimer::singleShot(2s, [this]() { dismiss(); });
  }
};

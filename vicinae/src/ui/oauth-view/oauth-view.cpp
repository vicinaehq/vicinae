#include "oauth-view.hpp"
#include "services/oauth/oauth-service.hpp"
#include "theme.hpp"
#include "navigation-controller.hpp"
#include "ui/icon-button/icon-button.hpp"
#include "ui/toast/toast.hpp"
#include "utils/layout.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "services/app-service/app-service.hpp"
#include "environment.hpp"
#include "ui/image/url.hpp"
#include "theme/colors.hpp"
#include "ui/button/button.hpp"
#include "services/window-manager/window-manager.hpp"

OAuthView::OAuthView(const ApplicationContext *ctx, const proto::ext::oauth::AuthorizeRequest &reqData)
    : m_ctx(ctx) {
  auto oauth = m_ctx->services->oauthService();
  auto data = OAuthRequestData::fromUrl(QUrl(reqData.url().c_str()));
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
    if (Environment::isLayerShellEnabled()) {
      m_ctx->navigation->closeWindow({.popToRootType = PopToRootType::Suspended});
    }
    m_ctx->services->appDb()->openTarget(url);
  };
  ImageURL iconUrl;

  auto m_continueBtn = new ButtonWidget;

  connect(m_continueBtn, &ButtonWidget::clicked, this, [this, url]() {
    if (Environment::isLayerShellEnabled()) {
      m_ctx->navigation->closeWindow({.popToRootType = PopToRootType::Suspended});
    }
    auto appDb = m_ctx->services->appDb();
    auto wm = m_ctx->services->windowManager();

    if (auto browser = appDb->webBrowser()) {
      appDb->openTarget(url);
      wm->focusApp(*browser);
    }
  });

  m_continueBtn->setText(QString("Continue with %1").arg(client.name().c_str()));
  QTimer::singleShot(0, this, [m_continueBtn]() { m_continueBtn->setFocus(); });

  if (reqData.client().has_icon()) { m_providerIcon = reqData.client().icon(); }

  m_initView = HStack()
                   .add(VStack()
                            .add(UI::Icon(m_providerIcon).size({40, 40}), 0, Qt::AlignCenter)
                            .add(UI::Text(client.name().c_str()).title().align(Qt::AlignHCenter))
                            .add(UI::Text(client.description().c_str()).align(Qt::AlignHCenter))
                            .add(m_continueBtn, 0, Qt::AlignHCenter)
                            .addStretch()
                            .add(UI::Text(helpText).secondary().align(Qt::AlignHCenter).autoEllide(false))
                            .spacing(20))
                   .margins(40, 40, 40, 40)
                   .buildWidget();

  m_successView =
      HStack()
          .add(VStack()
                   .add(HStack()
                            .center()
                            .spacing(10)
                            .add(UI::Icon(m_providerIcon).size({40, 40}))
                            .add(UI::Icon(ImageURL::builtin("check-circle").setFill(SemanticColor::Green))
                                     .size({40, 40})))
                   .add(UI::Text("You're in!").title().align(Qt::AlignHCenter))
                   .add(UI::Text(QString("Successfully connected to %1.\nBack to command in an instant...")
                                     .arg(client.name().c_str()))
                            .paragraph()
                            .align(Qt::AlignHCenter))
                   .addStretch()
                   .spacing(20))
          .margins(40, 40, 40, 40)
          .buildWidget();

  m_view->addWidget(m_initView);
  m_view->addWidget(m_successView);
  m_view->setCurrentWidget(m_initView);

  VStack().add(header).add(m_view).imbue(this);
}

void OAuthView::showSuccess() {
  using namespace std::chrono_literals;
  m_ctx->navigation->showWindow();
  m_view->setCurrentWidget(m_successView);
  QTimer::singleShot(2s, [this]() { dismiss(); });
}

void OAuthView::handleError(const QString &error) {
  auto toast = m_ctx->services->toastService();

  toast->setToast(error, ToastStyle::Danger);
  m_request->respondWithError(error);
}

void OAuthView::abort() {
  m_ctx->navigation->popToRoot();
  dismiss();
}

void OAuthView::keyPressEvent(QKeyEvent *key) {
  if (key->key() == Qt::Key_Escape) {
    abort();
    return;
  }

  QWidget::keyPressEvent(key);
}

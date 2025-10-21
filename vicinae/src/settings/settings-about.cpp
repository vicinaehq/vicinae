#include "settings-about.hpp"
#include "../ui/image/url.hpp"
#include "contribs/contribs.hpp"
#include "service-registry.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "utils/layout.hpp"
#include <qgraphicseffect.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "services/app-service/app-service.hpp"
#include "vicinae.hpp"
#include "version.h"

class ContributorWidget : public QWidget {
  Contributor::Contributor m_contrib;
  QGraphicsOpacityEffect *m_opacity = new QGraphicsOpacityEffect(this);

  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::MouseButton::LeftButton) {
      auto profileUrl = QString("https://github.com/%1").arg(m_contrib.login);

      ServiceRegistry::instance()->appDb()->openTarget(profileUrl);
      return;
    }

    QWidget::mousePressEvent(event);
  }

  void paintEvent(QPaintEvent *event) override {
    m_opacity->setOpacity(underMouse() ? 0.8 : 1);
    QWidget::paintEvent(event);
  }

  void setupUI() {
    setAttribute(Qt::WA_Hover);
    setGraphicsEffect(m_opacity);
    HStack()
        .spacing(10)
        .add(UI::Icon(ImageURL::local(m_contrib.resource)).size({35, 35}))
        .add(VStack()
                 .spacing(5)
                 .add(UI::Text(m_contrib.login))
                 .add(UI::Text(QString("%1 contribution%2")
                                   .arg(m_contrib.contribs)
                                   .arg(m_contrib.contribs > 0 ? "s" : ""))
                          .secondary()
                          .smaller())
                 .justifyBetween())
        .imbue(this);
  }

public:
  ContributorWidget(const Contributor::Contributor &contrib) : m_contrib(contrib) { setupUI(); }
};

void SettingsAbout::setupUI() {
  auto makeLinkOpener = [](const QString &link) {
    return [link]() { ServiceRegistry::instance()->appDb()->openTarget(link); };
  };

  auto contribs = Contributor::getList();

  auto contrib =
      VStack()
          .spacing(15)
          .map(contribs,
               [](const Contributor::Contributor &contrib) { return new ContributorWidget(contrib); })
          .buildWidget();

  auto aboutPage = VStack()
                       .spacing(12)
                       .margins(16)
                       .addIcon(ImageURL::builtin("vicinae"), QSize(64, 64), Qt::AlignCenter)
                       .addTitle("Vicinae", SemanticColor::Foreground, Qt::AlignCenter)
                       .add(UI::Text(Omnicast::HEADLINE).align(Qt::AlignCenter).paragraph())
                       .add(UI::Text(QString("Version %1 - Commit %2\n(%3)")
                                         .arg(VICINAE_GIT_TAG)
                                         .arg(VICINAE_GIT_COMMIT_HASH)
                                         .arg(BUILD_INFO))
                                .secondary()
                                .paragraph()
                                .smaller()
                                .align(Qt::AlignCenter))
                       .addSpacer(10)
                       .add(UI::Button("GitHub")
                                .leftIcon(ImageURL::builtin("github"))
                                .onClick(makeLinkOpener(Omnicast::GH_REPO)))
                       .add(UI::Button("Documentation")
                                .leftIcon(ImageURL::builtin("book"))
                                .onClick(makeLinkOpener(Omnicast::DOC_URL)))
                       .add(UI::Button("Report a Bug")
                                .leftIcon(ImageURL::builtin("bug"))
                                .onClick(makeLinkOpener(Omnicast::GH_REPO_CREATE_ISSUE)))
                       .addSpacer(20)
                       .add(UI::Text(QString("Brought to you by our %1 contributors:").arg(contribs.size())))
                       .add(contrib)
                       .addStretch();

  auto about = aboutPage.buildWidget();
  auto scrollArea = new VerticalScrollArea;

  scrollArea->setWidgetResizable(true);
  scrollArea->setAttribute(Qt::WA_TranslucentBackground);

  about->setMaximumWidth(800);
  about->setMinimumWidth(400);

  auto page = VStack().margins(0, 20, 0, 20).add(about, 0, Qt::AlignHCenter).buildWidget();

  setWidget(page);
}

SettingsAbout::SettingsAbout() { setupUI(); }

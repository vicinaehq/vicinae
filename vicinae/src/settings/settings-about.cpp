#include "settings-about.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "utils/layout.hpp"
#include <qgraphicseffect.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "services/app-service/app-service.hpp"
#include "vicinae.hpp"
#include "version.h"
#include <QMouseEvent>

void SettingsAbout::setupUI() {
  auto makeLinkOpener = [](const QString &link) {
    return [link]() { ServiceRegistry::instance()->appDb()->openTarget(link); };
  };

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

#include "settings-about.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "utils/layout.hpp"
#include <qnamespace.h>
#include "services/app-service/app-service.hpp"
#include "vicinae.hpp"

void SettingsAbout::setupUI() {
  auto makeLinkOpener = [](const QString &link) {
    return [link]() { ServiceRegistry::instance()->appDb()->openTarget(link); };
  };

  auto aboutPage =
      VStack()
          .spacing(12)
          .margins(16)
          .addIcon(BuiltinOmniIconUrl("vicinae"), QSize(64, 64), Qt::AlignCenter)
          .addTitle("Vicinae", SemanticColor::TextPrimary, Qt::AlignCenter)
          .add(UI::Text("A Raycast-style launcher for Linux — native, fast, extensible.")
                   .align(Qt::AlignCenter)
                   .paragraph())
          .add(UI::Text(QString("Version %1 - Commit %2").arg(VICINAE_GIT_TAG).arg(VICINAE_GIT_COMMIT_HASH))
                   .secondary()
                   .smaller()
                   .align(Qt::AlignCenter))
          .addSpacer(10)
          .add(UI::Button("GitHub")
                   .leftIcon(BuiltinOmniIconUrl("github"))
                   .onClick(makeLinkOpener(Omnicast::GH_REPO)))
          .add(UI::Button("Documentation")
                   .leftIcon(BuiltinOmniIconUrl("book"))
                   .onClick(makeLinkOpener(Omnicast::DOC_URL)))
          .add(UI::Button("Report a Bug")
                   .leftIcon(BuiltinOmniIconUrl("warning"))
                   .onClick(makeLinkOpener(Omnicast::GH_REPO_CREATE_ISSUE)))
          .add(UI::Button("License")
                   .leftIcon(BuiltinOmniIconUrl("quote-block"))
                   .onClick(makeLinkOpener(Omnicast::GH_REPO_LICENSE)))
          .addStretch();

  auto about = aboutPage.buildWidget();

  about->setMaximumWidth(800);
  about->setMinimumWidth(400);

  VStack().margins(0, 20, 0, 20).add(about, 0, Qt::AlignHCenter).imbue(this);
}

SettingsAbout::SettingsAbout() { setupUI(); }

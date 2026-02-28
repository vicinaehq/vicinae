#include "store-intro-view-host.hpp"
#include "navigation-controller.hpp"
#include "view-utils.hpp"
#include "ui/action-pannel/action.hpp"

StoreIntroViewHost::StoreIntroViewHost(const QString &markdown, const ImageURL &icon,
                                       const QString &actionLabel, std::function<void()> continueAction)
    : m_icon(icon), m_actionLabel(actionLabel), m_continueAction(std::move(continueAction)) {
  auto iconUrl = qml::imageSourceFor(icon);
  iconUrl += QStringLiteral("&raycast-width=32&raycast-height=32");
  m_markdown = QStringLiteral("![icon](%1)\n\n").arg(iconUrl) + markdown;
}

QUrl StoreIntroViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreIntroView.qml"));
}

QVariantMap StoreIntroViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void StoreIntroViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  section->addAction(new StaticAction(m_actionLabel, m_icon, m_continueAction));
  setActions(std::move(panel));
}

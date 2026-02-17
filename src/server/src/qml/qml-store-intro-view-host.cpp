#include "qml-store-intro-view-host.hpp"
#include "navigation-controller.hpp"
#include "qml-utils.hpp"
#include "ui/action-pannel/action.hpp"

QmlStoreIntroViewHost::QmlStoreIntroViewHost(const QString &markdown, const ImageURL &icon,
                                             const QString &actionLabel,
                                             std::function<void()> continueAction)
    : m_icon(icon), m_actionLabel(actionLabel), m_continueAction(std::move(continueAction)) {
  auto iconUrl = qml::imageSourceFor(icon);
  iconUrl += QStringLiteral("&raycast-width=32&raycast-height=32");
  m_markdown = QStringLiteral("![icon](%1)\n\n").arg(iconUrl) + markdown;
}

QUrl QmlStoreIntroViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/StoreIntroView.qml"));
}

QVariantMap QmlStoreIntroViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<QmlStoreIntroViewHost *>(this))}};
}

void QmlStoreIntroViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  section->addAction(new StaticAction(m_actionLabel, m_icon, m_continueAction));
  setActions(std::move(panel));
}

#include "ui/views/intro-view-host.hpp"
#include "navigation-controller.hpp"
#include "ui/views/view-utils.hpp"
#include "ui/action-panel/action.hpp"

IntroViewHost::IntroViewHost(const QString &markdown, const ImageURL &icon, const QString &actionLabel,
                             std::function<void()> continueAction)
    : m_icon(icon), m_actionLabel(actionLabel), m_continueAction(std::move(continueAction)) {
  auto iconUrl = qml::imageSourceFor(icon);
  iconUrl += QStringLiteral("&raycast-width=50&raycast-height=50");
  m_markdown = QStringLiteral("![](%1)\n\n").arg(iconUrl) + markdown;
}

IntroViewHost::IntroViewHost(const QString &title, const QString &description, const ImageURL &icon,
                             const QString &actionLabel, std::function<void()> continueAction)
    : m_centered(true), m_title(title), m_description(description), m_icon(icon), m_actionLabel(actionLabel),
      m_continueAction(std::move(continueAction)) {}

QUrl IntroViewHost::qmlComponentUrl() const { return qml::componentUrl(u"IntroView"); }

QVariantMap IntroViewHost::qmlProperties() { return {{QStringLiteral("host"), QVariant::fromValue(this)}}; }

void IntroViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<ListActionPanelState>();
  auto section = panel->createSection();
  auto action = new StaticAction(m_actionLabel, m_icon, m_continueAction);
  action->setPrimary(true);
  section->addAction(action);
  for (const auto &secondary : m_secondaryActions) {
    section->addAction(new StaticAction(secondary.label, secondary.icon, secondary.action));
  }
  setActions(std::move(panel));
}

void IntroViewHost::addSecondaryAction(const QString &label, const ImageURL &icon,
                                       std::function<void()> action) {
  m_secondaryActions.emplace_back(SecondaryAction{.label = label, .icon = icon, .action = std::move(action)});
}

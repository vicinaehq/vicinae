#include "qml-extension-error-view-host.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"

QmlExtensionErrorViewHost::QmlExtensionErrorViewHost(const QString &errorText)
    : m_errorText(errorText.trimmed()) {
  m_markdown = QStringLiteral(
      "# Extension crashed 💥!\n\n"
      "This extension threw an uncaught exception and crashed as a result.\n\n"
      "Find the full stacktrace below. You can also directly copy it from the action menu.\n\n"
      "```\n%1\n```")
                    .arg(m_errorText);
}

QUrl QmlExtensionErrorViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/MarkdownDetailView.qml"));
}

QVariantMap QmlExtensionErrorViewHost::qmlProperties() const {
  return {{QStringLiteral("markdown"), m_markdown}};
}

void QmlExtensionErrorViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  auto copy = new CopyToClipboardAction(Clipboard::Text(m_errorText));
  copy->setPrimary(true);
  section->addAction(copy);
  setActions(std::move(panel));
}

QString QmlExtensionErrorViewHost::markdown() const { return m_markdown; }

#include "create-extension-success-view-host.hpp"
#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

static const QString MARKDOWN = R"(
# Extension successfully created

Your new extension %1 has been succesfully created at `%2`.

For commands from this extension to be picked up by Vicinae, you need to run your extension in development mode at least once:

```bash
cd %2
npm install
npm run dev
```

You can learn more about extension development in the [Vicinae documentation](https://docs.vicinae.com/).
)";

CreateExtensionSuccessViewHost::CreateExtensionSuccessViewHost(const ExtensionBoilerplateConfig &cfg,
                                                               const std::filesystem::path &location)
    : m_markdown(MARKDOWN.arg(cfg.title).arg(QString::fromStdString(location.string()))), m_path(location) {}

QUrl CreateExtensionSuccessViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/MarkdownDetailView.qml"));
}

QVariantMap CreateExtensionSuccessViewHost::qmlProperties() {
  return {{QStringLiteral("markdown"), m_markdown}};
}

void CreateExtensionSuccessViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<ListActionPanelState>();
  auto appDb = context()->services->appDb();
  auto *section = panel->createSection();

  for (const auto &opener : appDb->findOpeners("inode/directory")) {
    auto open = new OpenAppAction(opener, QString("Open in %1").arg(opener->displayName()),
                                  {QString::fromStdString(m_path.string())});
    section->addAction(open);
  }

  setActions(std::move(panel));
}

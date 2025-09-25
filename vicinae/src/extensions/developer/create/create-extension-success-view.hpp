#pragma once
#include <memory>
#include "actions/app/app-actions.hpp"
#include "navigation-controller.hpp"
#include "services/extension-boilerplate-generator/extension-boilerplate-generator.hpp"
#include "ui/views/detail-view.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"

static const QString MARKDOWN = R"(
# Extension successfully created 🥳

Your new extension %1 has been succesfully created at `%2`.

For commands from this extension to be picked up by Vicinae, you need to run your extension in development mode at least once:

```bash
cd %2
npm install
npm run dev
```

You can learn more about extension development in the [Vicinae documentation](https://docs.vicinae.com/).
)";

class CreateExtensionSuccessView : public DetailView {
  std::filesystem::path m_path;

public:
  CreateExtensionSuccessView(const ExtensionBoilerplateConfig &cfg, const std::filesystem::path &location)
      : m_path(location) {
    m_md = MARKDOWN.arg(cfg.title).arg(location.c_str());
  }

protected:
  QString markdown() const override { return m_md; }

  std::unique_ptr<ActionPanelState> actionPanel() const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto appDb = context()->services->appDb();
    auto section = panel->createSection();

    for (const auto &opener : appDb->findOpeners("inode/directory")) {
      auto open =
          new OpenAppAction(opener, QString("Open in %1").arg(opener->displayName()), {m_path.c_str()});
      section->addAction(open);
    }

    return panel;
  }

private:
  QString m_md;
};

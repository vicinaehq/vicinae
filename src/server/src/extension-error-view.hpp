#include "ui/views/base-view.hpp"
#include "clipboard-actions.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include <qboxlayout.h>

class ExtensionErrorView : public BaseView {
  MarkdownRenderer *m_renderer = new MarkdownRenderer;
  QString m_errorText;

  bool supportsSearch() const override { return false; }

  void setupUI() {
    auto layout = new QVBoxLayout;
    QString markdown = QString::fromStdString(R"(# Extension crashed 💥!

This extension threw an uncaught exception and crashed as a result.

Find the full stacktrace below. You can also directly copy it from the action menu.

```
%1
```
)");

    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(m_renderer);
    m_renderer->setMarkdown(markdown.arg(m_errorText.trimmed()).trimmed());

    setLayout(layout);
  }

  std::unique_ptr<ActionPanelState> generateActions() {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();
    auto copy = new CopyToClipboardAction(Clipboard::Text(m_errorText.trimmed()));

    copy->setPrimary(true);
    section->addAction(copy);

    return panel;
  }

  void initialize() override { setActions(generateActions()); }

public:
  ExtensionErrorView(const QString &errorText) : m_errorText(errorText) { setupUI(); }
};

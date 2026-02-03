#include "navigation-controller.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/views/base-view.hpp"
#include "utils/layout.hpp"

class DetailView : public BaseView {
public:
  DetailView() { VStack().add(m_renderer).imbue(this); }

protected:
  virtual QString markdown() const = 0;
  virtual std::unique_ptr<ActionPanelState> actionPanel() const {
    return std::make_unique<ActionPanelState>();
  }

  bool supportsSearch() const override { return false; }

  void initialize() override {
    m_renderer->setMarkdown(markdown());
    setActions(actionPanel());
  }

private:
  MarkdownRenderer *m_renderer = new MarkdownRenderer;
};

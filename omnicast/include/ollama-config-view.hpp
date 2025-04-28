#pragma once
#include "ai/ollama-ai-provider.hpp"
#include "app.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "view.hpp"

class OllamaConfigView : public View {
  class SubmitAction : public AbstractAction {
  public:
    void execute(AppWindow &app) override {}

    SubmitAction() : AbstractAction("Submit", BuiltinOmniIconUrl("enter-key")) {
      setShortcut({.key = "return", .modifiers = {"shift"}});
    }
  };

  BaseInput *m_input = new BaseInput(this);
  FormWidget *m_form = new FormWidget(this);

  void handleSubmit() {
    auto aiManager = ServiceRegistry::instance()->AI();
    QUrl instanceUrl(m_input->text());

    m_form->clearAllErrors();

    if (!instanceUrl.isValid()) { m_form->setError(m_input, "Invalid URL"); }
    if (!instanceUrl.scheme().startsWith("http")) { m_form->setError(m_input, "Protocol should be HTTP"); }

    // XXX - Test connectivity here?

    if (!m_form->isValid()) return;

    QJsonObject data;

    data["instanceUrl"] = instanceUrl.toString();

    bool configSaved = aiManager->setProviderConfig("ollama", data);

    if (!configSaved) {
      app.statusBar->setToast("Failed to save provider config", ToastPriority::Danger);
      return;
    }

    pop();
  }

public:
  void onMount() override {
    hideInput();

    auto action = std::make_unique<SubmitAction>();
    std::vector<ActionItem> items;

    connect(action.get(), &SubmitAction::didExecute, this, &OllamaConfigView::handleSubmit);
    items.emplace_back(std::move(action));
    setActionPannel(std::move(items));
  }

  OllamaConfigView(AppWindow &app) : View(app) {
    auto aiManager = ServiceRegistry::instance()->AI();
    auto layout = new QVBoxLayout;

    if (auto existingConfig = aiManager->getProviderConfig("ollama")) {
      m_input->setText(existingConfig->data.value("instanceUrl").toString());
    }

    m_input->setPlaceholderText("http://localhost:11434");
    m_form->addField(new FormField(m_input, "Instance URL"));
    m_form->focusFirst();
    layout->addWidget(m_form);
    setLayout(layout);
  }
};

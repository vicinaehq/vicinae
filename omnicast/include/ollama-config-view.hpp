#pragma once
#include "action-panel/action-panel.hpp"
#include "ai/ollama-ai-provider.hpp"
#include "app.hpp"
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/form/base-input.hpp"
#include "ui/form/form-field.hpp"
#include "ui/form/form.hpp"
#include "view.hpp"

class OllamaConfigView : public FormView {
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
    auto ui = ServiceRegistry::instance()->UI();
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
      ui->setToast("Failed to save provider config", ToastPriority::Danger);
      return;
    }

    ui->popView();
  }

public:
  void onActivate() override { m_form->focusFirst(); }

  void initialize() override {
    auto panel = new ActionPanelStaticListView;

    panel->addAction(new StaticAction("Submit", BuiltinOmniIconUrl("return"), [this]() { handleSubmit(); }));
    m_actionPannelV2->setView(panel);
  }

  OllamaConfigView() {
    auto aiManager = ServiceRegistry::instance()->AI();
    auto layout = new QVBoxLayout;

    if (auto existingConfig = aiManager->getProviderConfig("ollama")) {
      m_input->setText(existingConfig->data.value("instanceUrl").toString());
    }

    m_input->setPlaceholderText("http://localhost:11434");
    m_form->addField(new FormField(m_input, "Instance URL"));
    setupUI(m_form);
  }
};

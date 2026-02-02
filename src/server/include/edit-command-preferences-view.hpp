#pragma once
#include "ui/views/base-view.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/form/form.hpp"
#include "ui/form/preference-field.hpp"
#include "command-database.hpp"
#include "ui/toast/toast.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qjsonvalue.h>

class EditCommandPreferencesView : public BaseView {
  class SubmitAction : public AbstractAction {
  public:
    void execute(AppWindow &app) override {}

    SubmitAction() : AbstractAction("Save preferences", ImageURL::builtin("save-document")) {}
  };

  QVBoxLayout *m_layout = new QVBoxLayout;
  FormWidget *m_form = new FormWidget;
  std::shared_ptr<AbstractCmd> m_command;
  std::vector<PreferenceField *> m_fields;

  void handleSubmit() {
    QJsonObject values;

    for (const auto &field : m_fields) {
      auto &pref = field->preference();
      auto value = field->asJsonValue();

      values[pref->name()] = value;

      if (value.isString()) {
        auto s = value.toString();

        if (s.isEmpty() && pref->isRequired()) {
          field->setError("Must not be empty");
          continue;
        }
      }

      field->clearError();
    }

    if (!m_form->isValid()) {
      app.statusBar->setToast("Validation error", ToastPriority::Danger);
      return;
    }

    auto commandDb = ServiceRegistry::instance()->commandDb();
    QJsonDocument doc;

    doc.setObject(values);
    commandDb->setPreferenceValues(m_command->uniqueId(), values);
    app.statusBar->setToast("Preferences changed", ToastPriority::Success);
    pop();
  }

public:
  void onMount() override {
    hideInput();

    std::vector<ActionItem> items;
    auto action = std::make_shared<SubmitAction>();

    action->setShortcut({.key = "return", .modifiers = {"shift"}});

    connect(action.get(), &AbstractAction::didExecute, this, &EditCommandPreferencesView::handleSubmit);

    items.push_back(std::move(action));
    setActionPannel(items);
  }

  EditCommandPreferencesView(AppWindow &app, const std::shared_ptr<AbstractCmd> &command)
      : View(app), m_command(command) {
    auto commandDb = ServiceRegistry::instance()->commandDb();
    auto preferences = m_command->preferences();
    auto preferenceValues = commandDb->getPreferenceValues(m_command->uniqueId());

    m_layout->setContentsMargins(0, 20, 0, 20);
    m_layout->addWidget(m_form);
    m_fields.reserve(preferences.size());

    for (const auto &preference : m_command->preferences()) {
      auto field = new PreferenceField(preference);
      auto value = preferenceValues.value(preference->name());

      if (auto value = preferenceValues.value(preference->name()); !value.isNull()) {
        field->setValueAsJson(value);
      } else if (auto dflt = preference->defaultValueAsJson(); !dflt.isNull()) {
        field->setValueAsJson(dflt);
      }

      m_fields.push_back(field);
      m_form->addField(field);
    }

    setLayout(m_layout);
  }
};

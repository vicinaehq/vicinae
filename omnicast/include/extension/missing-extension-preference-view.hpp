#include "action-panel/action-panel.hpp"
#include "app.hpp"
#include "base-view.hpp"
#include "extension/extension-command.hpp"
#include "omni-command-db.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/form/preference-field.hpp"
#include "ui/form/form.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

class MissingExtensionPreferenceView : public FormView {
  FormWidget *m_form = new FormWidget;

  QJsonObject m_existingPreferenceValues;
  QVBoxLayout *m_layout = new QVBoxLayout;
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<PreferenceField *> m_preferenceFields;

public:
  MissingExtensionPreferenceView(AppWindow &app, const std::shared_ptr<ExtensionCommand> &command)
      : m_command(command) {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    QWidget *widget = new QWidget;

    m_existingPreferenceValues =
        manager->getPreferenceValues(QString("extension.%1").arg(command->uniqueId()));
    auto icon = new OmniIcon();

    icon->setFixedSize(32, 32);
    icon->setUrl(command->iconUrl());

    auto title = new TypographyWidget;

    title->setSize(TextSize::TextTitle);
    title->setText(QString("Welcome to %1").arg(m_command->repositoryName()));
    title->setFontWeight(QFont::Weight::DemiBold);
    title->setAlignment(Qt::AlignCenter);

    auto paragraph = new TypographyWidget;

    paragraph->setWordWrap(true);
    paragraph->setMaximumWidth(800);
    paragraph->setAlignment(Qt::AlignCenter);
    paragraph->setColor(ColorTint::TextSecondary);
    paragraph->setText("Before you can start using this command, you will need to add a few things to the "
                       "settings, listed below");

    // paragraph->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setAlignment(Qt::AlignTop);
    m_layout->setSpacing(10);
    m_layout->addWidget(icon, 0, Qt::AlignHCenter);
    m_layout->addWidget(title);
    m_layout->addWidget(paragraph, 0, Qt::AlignHCenter);

    for (const auto &preference : command->preferences()) {
      if (preference->isRequired() && preference->defaultValueAsJson().isNull() &&
          !m_existingPreferenceValues.contains(preference->name())) {
        auto field = new PreferenceField(preference);

        m_form->addField(field);
        m_preferenceFields.push_back(field);
      }
    }

    m_layout->addWidget(m_form);
    m_layout->addStretch();
    widget->setLayout(m_layout);
    setupUI(widget);
  }

  void handleSubmit() {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto ui = ServiceRegistry::instance()->UI();
    bool validated = true;
    QJsonObject obj(m_existingPreferenceValues);

    for (const auto &field : m_preferenceFields) {
      auto value = field->asJsonValue();

      if (value.isNull() || value.toString().isEmpty()) {
        field->setError("Should not be empty");
        validated = false;
        continue;
      }

      field->clearError();
      obj[field->preference()->name()] = value;
    }

    QJsonDocument doc;

    doc.setObject(obj);
    qDebug() << "preference object:" << doc.toJson();

    if (!validated) return;

    manager->setPreferenceValues(QString("extension.%1").arg(m_command->uniqueId()), obj);
    ui->popView();
    ui->launchCommand(m_command);
  }

  void onActivate() override { m_form->focusFirst(); }

  void initialize() override {
    auto panel = new ActionPanelStaticListView;
    auto continueAction =
        new StaticAction("Save preferences", m_command->iconUrl(), [this]() { handleSubmit(); });

    continueAction->setShortcut({.key = "return", .modifiers = {"shift"}});
    continueAction->setPrimary(true);
    panel->addAction(continueAction);
    m_actionPannelV2->setView(panel);
  }
};

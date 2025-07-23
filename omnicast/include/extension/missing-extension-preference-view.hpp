#include "action-panel/action-panel.hpp"
#include "base-view.hpp"
#include "extension/extension-command.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/form/preference-field.hpp"
#include "ui/form/form.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qwidget.h>

class MissingExtensionPreferenceView : public FormView {
  FormWidget *m_form = new FormWidget;

  QJsonObject m_existingPreferenceValues;
  QVBoxLayout *m_layout = new QVBoxLayout;
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<PreferenceField *> m_preferenceFields;

public:
  MissingExtensionPreferenceView(const std::shared_ptr<ExtensionCommand> &command,
                                 const std::vector<Preference> &preferences,
                                 const QJsonObject &preferenceValues)
      : m_command(command), m_existingPreferenceValues(preferenceValues) {
    QWidget *widget = new QWidget;

    auto centeringLayout = new QHBoxLayout;

    auto icon = new OmniIcon();

    icon->setFixedSize(32, 32);
    icon->setUrl(command->iconUrl());

    auto title = new TypographyWidget;

    title->setSize(TextSize::TextTitle);
    title->setText(QString("Welcome to %1").arg(m_command->repositoryName()));
    title->setFontWeight(QFont::Weight::DemiBold);
    title->setAlignment(Qt::AlignCenter);

    auto paragraph = new TypographyWidget;

    // paragraph->setAlignment(Qt::AlignCenter);
    paragraph->setColor(SemanticColor::TextSecondary);
    paragraph->setText(
        "Before you can use this command, you need to fill in the required preference fields below.");
    paragraph->setWordWrap(true);

    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->addWidget(icon, 0, Qt::AlignHCenter);
    m_layout->addWidget(title);
    m_layout->addWidget(paragraph);
    m_layout->addWidget(m_form);
    m_layout->addStretch();

    for (const auto &preference : preferences) {
      if (preference.required() && !preference.hasDefaultValue() &&
          !m_existingPreferenceValues.contains(preference.name())) {
        auto field = new PreferenceField(preference);

        m_form->addField(field);
        m_preferenceFields.push_back(field);
      }
    }

    QWidget *m_centeredInfo = new QWidget;
    QWidget *m_centeredContainer = new QWidget;

    m_centeredInfo->setLayout(m_layout);
    centeringLayout->setContentsMargins(0, 0, 0, 0);
    centeringLayout->addStretch();
    centeringLayout->addWidget(m_centeredInfo);
    centeringLayout->addStretch();
    m_centeredContainer->setLayout(centeringLayout);

    QWidget *w = new QWidget;
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_centeredContainer);
    layout->addWidget(m_form, 1);
    w->setLayout(layout);

    setupUI(w);
  }

  void handleSubmit() {
    auto manager = ServiceRegistry::instance()->rootItemManager();
    auto ui = ServiceRegistry::instance()->UI();
    bool validated = true;
    QJsonObject obj(m_existingPreferenceValues);

    for (const auto &field : m_preferenceFields) {
      auto value = field->widget()->asJsonValue();

      if (value.isNull() || value.toString().isEmpty()) {
        field->setError("Should not be empty");
        validated = false;
        continue;
      }

      field->clearError();
      obj[field->preference().name()] = value;
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

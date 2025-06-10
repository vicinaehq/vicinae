#include "app.hpp"
#include "extension/extension-command.hpp"
#include "omni-command-db.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/form/preference-field.hpp"
#include "ui/form/form.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/typography.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>

/*
class MissingExtensionPreferenceView : public View {

  class ContinueAction : public AbstractAction {
    std::shared_ptr<ExtensionCommand> m_command;

    void execute(AppWindow &app) override {}

  public:
    ContinueAction(const std::shared_ptr<ExtensionCommand> &command)
        : m_command(command), AbstractAction("Continue", command->iconUrl()) {}
  };

  QJsonObject m_existingPreferenceValues;
  QVBoxLayout *m_layout = new QVBoxLayout;
  std::shared_ptr<ExtensionCommand> m_command;
  std::vector<PreferenceField *> m_preferenceFields;

public:
  MissingExtensionPreferenceView(AppWindow &app, const std::shared_ptr<ExtensionCommand> &command)
      : View(app), m_command(command) {
    auto db = ServiceRegistry::instance()->commandDb();

    m_existingPreferenceValues = db->getPreferenceValues(command->uniqueId());
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

    auto form = new FormWidget();

    for (const auto &preference : command->preferences()) {
      if (preference->isRequired() && preference->defaultValueAsJson().isNull() &&
          !m_existingPreferenceValues.contains(preference->name())) {
        auto field = new PreferenceField(preference);

        form->addField(field);
        m_preferenceFields.push_back(field);
      }
    }

    m_layout->addWidget(form);
    m_layout->addStretch();

    setLayout(m_layout);
  }

  void handleSubmit() {
    auto db = ServiceRegistry::instance()->commandDb();
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

    db->setPreferenceValues(m_command->uniqueId(), obj);

    pop();
    app.launchCommand(m_command, {});
  }

  void onMount() override {
    hideInput();

    std::vector<ActionItem> items;
    auto continueAction = std::make_unique<ContinueAction>(m_command);

    connect(continueAction.get(), &ContinueAction::didExecute, this,
            &MissingExtensionPreferenceView::handleSubmit);

    continueAction->setShortcut({.key = "return", .modifiers = {"shift"}});

    items.push_back(std::move(continueAction));
    setActionPannel(items);
  }
};
*/

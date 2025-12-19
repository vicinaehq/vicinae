#include "ui/views/base-view.hpp"
#include "extension/extension-command.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/form/preference-field.hpp"
#include "ui/form/form.hpp"
#include "ui/typography/typography.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include "services/root-item-manager/root-item-manager.hpp"
#include <qnamespace.h>
#include <qwidget.h>
#include "ui/views/form-view.hpp"

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

    auto icon = new ImageWidget();

    icon->setFixedSize(32, 32);
    icon->setUrl(command->iconUrl());

    auto title = new TypographyWidget;

    title->setSize(TextSize::TextTitle);
    title->setText(QString("Welcome to %1").arg(m_command->repositoryDisplayName()));
    title->setFontWeight(QFont::Weight::DemiBold);
    title->setAlignment(Qt::AlignCenter);

    auto paragraph = new TypographyWidget;

    // paragraph->setAlignment(Qt::AlignCenter);
    paragraph->setColor(SemanticColor::TextMuted);
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
      QJsonValue value = m_existingPreferenceValues.value(preference.name());
      bool hasValue = !(value.isUndefined() || value.isNull());
      bool hasDefault = !preference.defaultValue().isUndefined();
      bool isMissing = preference.required() && !hasValue && !hasDefault;

      if (isMissing) {
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

  void onSubmit() override {
    auto manager = context()->services->rootItemManager();
    bool validated = true;
    QJsonObject patch;

    for (const auto &field : m_preferenceFields) {
      auto value = field->widget()->asJsonValue();

      if (value.isNull() || value.toString().isEmpty()) {
        field->setError("Should not be empty");
        validated = false;
        continue;
      }

      field->clearError();
      patch[field->preference().name()] = value;
    }

    if (!validated) return;

    manager->setPreferenceValues(m_command->uniqueId(), patch);
    context()->navigation->popCurrentView();
    context()->navigation->launch(m_command);
  }

  void onActivate() override {
    QTimer::singleShot(0, this, [this]() { m_form->focusFirst(); });
  }

  QString submitTitle() const override { return "Save preferences"; }
};

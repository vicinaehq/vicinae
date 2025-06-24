#include "service-registry.hpp"
#include "app-metadata-settings-detail.hpp"

void CommandMetadataSettingsDetailWidget::setupUI() {
  if (auto description = m_command->description(); !description.isEmpty()) {
    MetadataRowWidget *descriptionLabel = new MetadataRowWidget(this);
    TypographyWidget *descriptionText = new TypographyWidget;

    descriptionLabel->setLabel("Description");
    descriptionText->setText(description);

    m_layout->addWidget(descriptionLabel);
    m_layout->addWidget(descriptionText);
  }

  for (const auto &preference : m_command->preferences()) {
    auto field = new PreferenceField(preference);
    QJsonValue defaultValue = preference->defaultValueAsJson();

    field->setVerticalDirection(!preference->isCheckboxType());

    if (m_preferenceValues.contains(preference->name())) {
      field->setValueAsJson(m_preferenceValues.value(preference->name()));
    } else {
      field->setValueAsJson(defaultValue);
    }

    field->setInfo(preference->description());

    m_form->addField(field);
    m_preferenceFields.push_back(field);
  }

  m_layout->addWidget(m_form);
  m_layout->addStretch();
  setLayout(m_layout);
}

CommandMetadataSettingsDetailWidget::CommandMetadataSettingsDetailWidget(
    const QString &rootItemId, const std::shared_ptr<AbstractCmd> &cmd)
    : m_rootItemId(rootItemId), m_command(cmd) {
  auto manager = ServiceRegistry::instance()->rootItemManager();

  m_preferenceValues = manager->getItemPreferenceValues(rootItemId);
  setupUI();
}

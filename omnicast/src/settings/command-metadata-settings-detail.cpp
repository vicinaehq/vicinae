#include "settings/command-metadata-settings-detail.hpp"
#include "service-registry.hpp"
#include "app-metadata-settings-detail.hpp"
#include <qboxlayout.h>
#include <qlogging.h>
#include <qwidget.h>

void CommandMetadataSettingsDetailWidget::setupUI() {
  if (auto description = m_command->description(); !description.isEmpty()) {
    MetadataRowWidget *descriptionLabel = new MetadataRowWidget(this);
    TypographyWidget *descriptionText = new TypographyWidget;

    descriptionLabel->setLabel("Description");
    descriptionText->setText(description);
    descriptionText->setWordWrap(true);

    m_layout->addWidget(descriptionLabel);
    m_layout->addSpacing(5);
    m_layout->addWidget(descriptionText);
    m_layout->addSpacing(20);
  }

  QWidget *m_formContainer = new QWidget;
  QVBoxLayout *m_formLayout = new QVBoxLayout;

  m_formContainer->setLayout(m_formLayout);
  m_formLayout->setSpacing(20);
  m_formLayout->setContentsMargins(0, 0, 0, 0);

  for (const auto &preference : m_command->preferences()) {
    PreferenceWidgetVisitor visitor(preference);
    auto widget = std::visit(visitor, preference.data());

    if (!widget) continue;

    QJsonValue defaultValue = preference.defaultValue();

    if (m_preferenceValues.contains(preference.name())) {
      widget->formItem()->setValueAsJson(m_preferenceValues.value(preference.name()));
    } else {
      widget->formItem()->setValueAsJson(defaultValue);
    }

    connect(widget->formItem()->focusNotifier(), &FocusNotifier::focusChanged, this, [this](bool focused) {
      if (!focused) savePendingPreferences();
    });

    m_preferenceFields[preference.name()] = widget;
    m_formLayout->addWidget(widget);
  }

  m_layout->addWidget(m_formContainer);
  m_layout->addStretch();
  setLayout(m_layout);
}

void CommandMetadataSettingsDetailWidget::savePendingPreferences() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  QJsonObject obj;

  for (const auto &[preferenceId, widget] : m_preferenceFields) {
    QJsonValue value = widget->formItem()->asJsonValue();

    qDebug() << "set preference" << preferenceId;

    obj[preferenceId] = value;
  }

  manager->setItemPreferenceValues(m_rootItemId, obj);
  qCritical() << "set preference values";
}

CommandMetadataSettingsDetailWidget::CommandMetadataSettingsDetailWidget(
    const QString &rootItemId, const std::shared_ptr<AbstractCmd> &cmd)
    : m_rootItemId(rootItemId), m_command(cmd) {
  auto manager = ServiceRegistry::instance()->rootItemManager();

  m_preferenceValues = manager->getItemPreferenceValues(rootItemId);
  setupUI();
}

CommandMetadataSettingsDetailWidget::~CommandMetadataSettingsDetailWidget() { savePendingPreferences(); }

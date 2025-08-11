#include "common.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "service-registry.hpp"
#include "app-metadata-settings-detail.hpp"
#include "settings/extension-settings-detail.hpp"
#include <qboxlayout.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "services/root-item-manager/root-item-manager.hpp"

void ExtensionSettingsDetail::handleFocusChanged(bool focused) {
  if (!focused) { savePendingPreferences(); }
}

void ExtensionSettingsDetail::setupUI() {
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

    connect(widget->formItem()->focusNotifier(), &FocusNotifier::focusChanged, this,
            &ExtensionSettingsDetail::handleFocusChanged, Qt::DirectConnection);

    m_preferenceFields[preference.name()] = widget;
    m_formLayout->addWidget(widget);
  }

  m_layout->addWidget(m_formContainer);
  m_layout->addStretch();
  setLayout(m_layout);
}

void ExtensionSettingsDetail::savePendingPreferences() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  QJsonObject obj;

  for (const auto &[preferenceId, widget] : m_preferenceFields) {
    obj[preferenceId] = widget->formItem()->asJsonValue();
  }

  manager->setProviderPreferenceValues(m_rootItemId, obj);
}

ExtensionSettingsDetail::ExtensionSettingsDetail(const QString &providerId,
                                                 const std::shared_ptr<AbstractCommandRepository> &cmd)
    : m_rootItemId(providerId), m_command(cmd) {
  auto manager = ServiceRegistry::instance()->rootItemManager();

  m_preferenceValues = manager->getProviderPreferenceValues(providerId);
  setupUI();
}

ExtensionSettingsDetail::~ExtensionSettingsDetail() {
  qCritical() << "~ExtensionSettingsDetail" << this;
  savePendingPreferences();
}

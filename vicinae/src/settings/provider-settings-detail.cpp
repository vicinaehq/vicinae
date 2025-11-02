#include "common.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "service-registry.hpp"
#include <qboxlayout.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "provider-settings-detail.hpp"

void ProviderSettingsDetail::handleFocusChanged(bool focused) {
  if (!focused) { savePendingPreferences(); }
}

void ProviderSettingsDetail::setupUI(const RootProvider &provider) {
  /*
if (auto description = item.description(); !description.isEmpty()) {
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
*/

  QWidget *m_formContainer = new QWidget;
  QVBoxLayout *m_formLayout = new QVBoxLayout;

  m_formContainer->setLayout(m_formLayout);
  m_formLayout->setSpacing(20);
  m_formLayout->setContentsMargins(0, 0, 0, 0);

  for (const auto &preference : provider.preferences()) {
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
            &ProviderSettingsDetail::handleFocusChanged, Qt::DirectConnection);

    m_preferenceFields[preference.name()] = widget;
    m_formLayout->addWidget(widget);
  }

  m_layout->addWidget(m_formContainer);
  m_layout->addStretch();
  setLayout(m_layout);
}

void ProviderSettingsDetail::savePendingPreferences() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  QJsonObject obj;

  for (const auto &[preferenceId, widget] : m_preferenceFields) {
    auto value = widget->formItem()->asJsonValue();

    // we do not store null or undefined values, so that the default value
    // is used instead.
    if (value.isNull() || value.isUndefined()) continue;

    obj[preferenceId] = value;
  }

  manager->setProviderPreferenceValues(m_rootItemId, obj);
}

ProviderSettingsDetail::ProviderSettingsDetail(const RootProvider &provider)
    : m_rootItemId(provider.uniqueId()) {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  m_preferenceValues = manager->getProviderPreferenceValues(m_rootItemId);
  setupUI(provider);
}

ProviderSettingsDetail::~ProviderSettingsDetail() { savePendingPreferences(); }

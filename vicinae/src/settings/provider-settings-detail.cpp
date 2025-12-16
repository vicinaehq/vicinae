#include "common.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings/command-metadata-settings-detail.hpp"
#include "service-registry.hpp"
#include <qboxlayout.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "provider-settings-detail.hpp"

void ProviderSettingsDetail::setupUI(const RootProvider &provider) {
  using namespace std::chrono_literals;

  m_timer.setInterval(1s);
  m_timer.start();

  connect(&m_timer, &QTimer::timeout, this, [this]() { savePendingPreferences(); });

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

    m_preferenceFields[preference.name()] = widget;
    m_formLayout->addWidget(widget);
  }

  m_layout->addWidget(m_formContainer);
  m_layout->addStretch();
  setLayout(m_layout);
}

void ProviderSettingsDetail::savePendingPreferences() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  QJsonObject patch;

  for (const auto &[name, w] : m_preferenceFields) {
    QJsonValue currentValue = w->formItem()->asJsonValue();
    if (currentValue != m_preferenceValues.value(name)) {
      m_preferenceValues[name] = currentValue;
      patch[name] = currentValue;
    }
  }

  if (!patch.empty()) { manager->setProviderPreferenceValues(m_rootItemId, patch); }
}

ProviderSettingsDetail::ProviderSettingsDetail(const RootProvider &provider)
    : m_rootItemId(provider.uniqueId()) {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  m_preferenceValues = manager->getProviderPreferenceValues(m_rootItemId);
  setupUI(provider);
}

ProviderSettingsDetail::~ProviderSettingsDetail() { savePendingPreferences(); }

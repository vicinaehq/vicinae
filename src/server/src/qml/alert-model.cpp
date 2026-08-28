#include "alert-model.hpp"
#include "view-utils.hpp"
#include "navigation-controller.hpp"
#include "ui/alert/alert.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"

AlertModel::AlertModel(NavigationController &nav, QObject *parent) : QObject(parent) {
  connect(&nav, &NavigationController::confirmAlertRequested, this, &AlertModel::handleAlertRequested);

  connect(&nav, &NavigationController::currentViewChanged, this, [this]() { dismiss(); });
}

void AlertModel::handleAlertRequested(DialogContentWidget *widget) {
  auto *alert = dynamic_cast<AlertWidget *>(widget);
  if (!alert) return;

  if (m_widget) {
    m_widget->triggerCancel();
    m_widget->deleteLater();
  }

  m_widget = alert;
  m_title = alert->titleText();
  m_message = alert->messageText();
  m_confirmText = alert->confirmButtonText();
  m_cancelText = alert->cancelButtonText();
  m_confirmColor = ThemeService::instance().theme().resolve(alert->confirmColor());
  m_cancelColor = ThemeService::instance().theme().resolve(alert->cancelColor());

  if (auto icon = alert->iconUrl()) {
    m_iconSource = qml::imageSourceFor(*icon);
    m_iconBadged = icon->isBuiltin();
  } else {
    m_iconSource.clear();
    m_iconBadged = false;
  }

  emit changed();

  m_visible = true;
  emit visibleChanged();
}

void AlertModel::confirm() {
  if (!m_widget) return;

  auto *w = m_widget;
  m_widget = nullptr;
  m_visible = false;
  emit visibleChanged();

  w->triggerConfirm();
  w->deleteLater();
}

void AlertModel::cancel() {
  if (!m_widget) return;

  auto *w = m_widget;
  m_widget = nullptr;
  m_visible = false;
  emit visibleChanged();

  w->triggerCancel();
  w->deleteLater();
}

void AlertModel::dismiss() {
  if (!m_widget) return;

  auto *w = m_widget;
  m_widget = nullptr;
  m_visible = false;
  emit visibleChanged();

  w->triggerCancel();
  w->deleteLater();
}

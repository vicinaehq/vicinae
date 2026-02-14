#include "qml-alert-model.hpp"
#include "qml-utils.hpp"
#include "navigation-controller.hpp"
#include "ui/alert/alert.hpp"
#include "theme.hpp"

static QColor resolveColorLike(const ColorLike &color) {
  auto &theme = ThemeService::instance().theme();
  if (auto *sc = std::get_if<SemanticColor>(&color)) return theme.resolve(*sc);
  if (auto *qc = std::get_if<QColor>(&color)) return *qc;
  if (auto *str = std::get_if<QString>(&color)) return QColor(*str);
  if (auto *dc = std::get_if<DynamicColor>(&color))
    return theme.isLight() ? QColor(dc->light) : QColor(dc->dark);
  return {};
}

QmlAlertModel::QmlAlertModel(NavigationController &nav, QObject *parent) : QObject(parent) {
  connect(&nav, &NavigationController::confirmAlertRequested,
          this, &QmlAlertModel::handleAlertRequested);

  connect(&nav, &NavigationController::currentViewChanged,
          this, [this]() { dismiss(); });
}

void QmlAlertModel::handleAlertRequested(DialogContentWidget *widget) {
  auto *alert = dynamic_cast<AlertWidget *>(widget);
  if (!alert) return;

  // If a previous alert was pending, cancel it
  if (m_widget) {
    m_widget->triggerCancel();
    m_widget->deleteLater();
  }

  m_widget = alert;
  m_title = alert->titleText();
  m_message = alert->messageText();
  m_confirmText = alert->confirmButtonText();
  m_cancelText = alert->cancelButtonText();
  m_confirmColor = resolveColorLike(alert->confirmColor());
  m_cancelColor = resolveColorLike(alert->cancelColor());

  if (auto icon = alert->iconUrl()) {
    m_iconSource = qml::imageSourceFor(*icon);
  } else {
    m_iconSource.clear();
  }

  emit changed();

  m_visible = true;
  emit visibleChanged();
}

void QmlAlertModel::confirm() {
  if (!m_widget) return;

  auto *w = m_widget;
  m_widget = nullptr;
  m_visible = false;
  emit visibleChanged();

  w->triggerConfirm();
  w->deleteLater();
}

void QmlAlertModel::cancel() {
  if (!m_widget) return;

  auto *w = m_widget;
  m_widget = nullptr;
  m_visible = false;
  emit visibleChanged();

  w->triggerCancel();
  w->deleteLater();
}

void QmlAlertModel::dismiss() {
  if (!m_widget) return;

  auto *w = m_widget;
  m_widget = nullptr;
  m_visible = false;
  emit visibleChanged();

  w->triggerCancel();
  w->deleteLater();
}

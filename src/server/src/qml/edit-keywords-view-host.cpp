#include "edit-keywords-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

EditKeywordsViewHost::EditKeywordsViewHost(LoadFn loadKeywords, SaveFn saveKeywords,
                                                 const QString &infoText)
    : FormViewBase(), m_loadKeywords(std::move(loadKeywords)), m_saveKeywords(std::move(saveKeywords)),
      m_infoText(infoText) {}

QUrl EditKeywordsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/EditKeywordsFormView.qml"));
}

QVariantMap EditKeywordsViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(const_cast<EditKeywordsViewHost *>(this))}};
}

void EditKeywordsViewHost::initialize() {
  BaseView::initialize();

  m_keywords = m_loadKeywords();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  auto submitAction =
      new StaticAction(QStringLiteral("Submit"), ImageURL::builtin("enter-key"), [this]() { submit(); });
  section->addAction(submitAction);
  setActions(std::move(panel));

  emit formChanged();
}

void EditKeywordsViewHost::submit() {
  auto toast = context()->services->toastService();

  if (m_saveKeywords(m_keywords)) {
    toast->setToast("Keywords edited", ToastStyle::Success);
    popSelf();
  } else {
    toast->setToast("Failed to edit keywords", ToastStyle::Danger);
  }
}

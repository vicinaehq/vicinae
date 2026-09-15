#include "vocabulary-form-view-host.hpp"
#include <memory>
#include <QUrl>
#include "builtins/dictation/dictation.hpp"
#include "service-registry.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/dictation/dictation-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action-panel-state.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/view-utils.hpp"

QUrl VocabularyFormViewHost::qmlComponentUrl() const { return qml::componentUrl(u"VocabularyFormView"); }

QVariantMap VocabularyFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

QString VocabularyFormViewHost::initialNavigationTitle() const { return tr("Add Word to Vocabulary"); }

ImageURL VocabularyFormViewHost::initialNavigationIcon() const {
  return ImageURL(Dictation::VOCABULARY_ICON).setBadge(BuiltinIcon::Plus);
}

void VocabularyFormViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  section->addAction(
      new StaticAction(tr("Add Word"), ImageURL::builtin(BuiltinIcon::Plus), [this]() { submit(); }));
  setActions(std::move(panel));
}

void VocabularyFormViewHost::submit() {
  const auto toast = context()->services->toastService();
  auto *vocabulary = context()->services->dictation()->vocabulary();
  const auto word = m_word.trimmed();

  m_wordError.clear();

  if (word.isEmpty()) {
    m_wordError = tr("Enter a word");
  } else if (vocabulary->contains(word.toStdString())) {
    m_wordError = tr("Already in your vocabulary");
  }

  emit errorsChanged();
  if (!m_wordError.isEmpty()) return;

  if (!vocabulary->add(word.toStdString())) {
    toast->failure(tr("Failed to add word"));
    return;
  }

  toast->success(tr("Added to vocabulary"));
  popSelf();
}

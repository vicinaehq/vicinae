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

namespace {

constexpr qsizetype MAX_HINT_LENGTH = 200;

} // namespace

QUrl VocabularyFormViewHost::qmlComponentUrl() const { return qml::componentUrl(u"VocabularyFormView"); }

QVariantMap VocabularyFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

QString VocabularyFormViewHost::initialNavigationTitle() const {
  return m_original ? tr("Edit Word") : tr("Add Word to Vocabulary");
}

ImageURL VocabularyFormViewHost::initialNavigationIcon() const {
  return ImageURL(Dictation::VOCABULARY_ICON).setBadge(BuiltinIcon::Plus);
}

void VocabularyFormViewHost::initialize() {
  BaseView::initialize();

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  section->addAction(new StaticAction(m_original ? tr("Save Word") : tr("Add Word"),
                                      ImageURL::builtin(BuiltinIcon::Plus), [this]() { submit(); }));
  setActions(std::move(panel));
}

void VocabularyFormViewHost::submit() {
  const auto toast = context()->services->toastService();
  auto *vocabulary = context()->services->dictation()->vocabulary();
  const auto word = m_word.trimmed();
  const auto hint = m_hint.simplified();
  const bool renamed = !m_original || word.compare(*m_original, Qt::CaseInsensitive) != 0;

  m_wordError.clear();
  m_hintError.clear();

  if (word.isEmpty()) {
    m_wordError = tr("Enter a word");
  } else if (renamed && vocabulary->contains(word.toStdString())) {
    m_wordError = tr("Already in your vocabulary");
  }
  if (hint.size() > MAX_HINT_LENGTH) {
    m_hintError = tr("Keep the hint under %n character(s)", nullptr, MAX_HINT_LENGTH);
  }

  emit errorsChanged();
  if (!m_wordError.isEmpty() || !m_hintError.isEmpty()) return;

  const auto storedHint = hint.isEmpty() ? std::nullopt : std::optional(hint.toStdString());
  const bool saved = m_original
                         ? vocabulary->update(m_original->toStdString(), word.toStdString(), storedHint)
                         : vocabulary->add(word.toStdString(), storedHint);
  if (!saved) {
    toast->failure(m_original ? tr("Failed to save word") : tr("Failed to add word"));
    return;
  }

  toast->success(m_original ? tr("Word saved") : tr("Added to vocabulary"));
  popSelf();
}

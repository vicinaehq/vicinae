#include "builtins/snippet/snippet-form-view-host.hpp"
#include "navigation-controller.hpp"
#include "utils/placeholder.hpp"
#include "service-registry.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-panel/action.hpp"
#include "ui/views/view-utils.hpp"
#include <QUrl>

SnippetFormViewHost::SnippetFormViewHost() : FormViewBase() {}

SnippetFormViewHost::SnippetFormViewHost(snippet::SerializedSnippet snippet, Mode mode)
    : FormViewBase(), m_mode(mode), m_initialSnippet(std::move(snippet)) {}

QUrl SnippetFormViewHost::qmlComponentUrl() const { return qml::componentUrl(u"SnippetFormView"); }

QVariantMap SnippetFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void SnippetFormViewHost::initialize() {
  BaseView::initialize();

  m_service = context()->services->snippetService();
  buildContentCompletions();

  m_appSelectorModel = new AppSelectorModel(this);

  auto panel = std::make_unique<FormActionPanelState>();
  auto section2 = panel->createSection();
  auto submitAction =
      new StaticAction(tr("Submit"), ImageURL::builtin(BuiltinIcon::EnterKey), [this]() { submit(); });
  section2->addAction(submitAction);
  setActions(std::move(panel));

  if (m_initialSnippet) {
    const auto &snippet = *m_initialSnippet;

    if (m_mode == Mode::Duplicate) {
      m_name = tr("Copy of %1").arg(QString::fromStdString(snippet.name));
    } else {
      m_name = QString::fromStdString(snippet.name);
    }

    const auto visitor = overloads{
        [this](const snippet::TextSnippet &text) { m_content = QString::fromStdString(text.text); },
        [this](const snippet::FileSnippet &file) { m_content = QString::fromStdString(file.file); }};
    std::visit(visitor, snippet.data);

    if (snippet.expansion) {
      m_keyword = QString::fromStdString(snippet.expansion->keyword);
      m_expandAsWord = snippet.expansion->word;
    }

    if (snippet.voicePhrase) m_voicePhrase = QString::fromStdString(*snippet.voicePhrase);

    for (const auto &wmClass : snippet.apps) {
      m_apps.append(QString::fromStdString(wmClass));
    }

    emit formChanged();
  }

  if (m_initialSnippet) {
    if (m_mode == Mode::Edit) {
      setNavigationTitle(tr("Edit \"%1\"").arg(QString::fromStdString(m_initialSnippet->name)));
    } else if (m_mode == Mode::Duplicate) {
      setNavigationTitle(tr("Duplicate \"%1\"").arg(QString::fromStdString(m_initialSnippet->name)));
    }
  }
}

void SnippetFormViewHost::submit() {
  const auto toast = context()->services->toastService();

  m_nameError.clear();
  m_contentError.clear();
  m_keywordError.clear();
  m_voicePhraseError.clear();

  bool valid = true;

  if (m_name.size() < 2) {
    m_nameError = tr("2 chars min.");
    valid = false;
  }
  if (m_content.isEmpty()) {
    m_contentError = tr("Content should not be empty");
    valid = false;
  } else {
    const auto parsed = PlaceholderString::parseSnippetText(m_content);
    auto cursorCount = std::ranges::count_if(
        parsed.placeholders(), [](const auto &ph) { return ph.id == QStringLiteral("cursor"); });
    if (cursorCount > 1) {
      m_contentError = tr("Only one {cursor} placeholder is allowed");
      valid = false;
    }
  }
  if (!m_keyword.isEmpty()) {
    if (auto err = snippet::Expansion::validateKeyword(m_keyword.toStdString())) {
      m_keywordError = QString::fromStdString(*err);
      valid = false;
    }
  }

  const auto voicePhrase = m_voicePhrase.trimmed().toStdString();
  if (!voicePhrase.empty()) {
    const auto *existing = m_service->database()->findByVoicePhrase(voicePhrase);
    if (snippet::normalizeVoicePhrase(voicePhrase).empty()) {
      m_voicePhraseError = tr("Must contain a letter or a number");
      valid = false;
    } else if (existing &&
               (m_mode != Mode::Edit || !m_initialSnippet || existing->id != m_initialSnippet->id)) {
      m_voicePhraseError = tr("Already used by \"%1\"").arg(QString::fromStdString(existing->name));
      valid = false;
    }
  }

  emit errorsChanged();

  if (!valid) {
    toast->failure(tr("Validation failed"));
    return;
  }

  snippet::SnippetPayload payload;
  payload.name = m_name.toStdString();
  payload.data = snippet::TextSnippet(m_content.toStdString());

  if (!m_keyword.isEmpty()) {
    snippet::Expansion expansion;
    expansion.keyword = m_keyword.toStdString();
    expansion.word = m_expandAsWord;
    payload.expansion = expansion;
  }

  if (!voicePhrase.empty()) payload.voicePhrase = voicePhrase;

  if (payload.expansion || payload.voicePhrase) {
    payload.apps.reserve(m_apps.size());
    for (const auto &wmClass : m_apps) {
      payload.apps.emplace_back(wmClass.toStdString());
    }
  }

  if (m_mode == Mode::Edit && m_initialSnippet) {
    const auto result = m_service->updateSnippet(m_initialSnippet->id, payload);
    if (!result) {
      toast->failure(result.error().c_str());
      return;
    }
    toast->success(tr("Snippet updated"));
  } else {
    const auto result = m_service->createSnippet(payload);
    if (!result) {
      toast->failure(result.error().c_str());
      return;
    }
    toast->success(tr("Snippet successfully created"));
  }

  popSelf();
}

bool SnippetFormViewHost::serverRunning() const { return m_service && m_service->isServerRunning(); }

void SnippetFormViewHost::buildContentCompletions() {
  m_contentCompletions = QVariantList{
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::TextCursor))},
          {QStringLiteral("title"), tr("Cursor Position")},
          {QStringLiteral("value"), QStringLiteral("cursor")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::CopyClipboard))},
          {QStringLiteral("title"), tr("Clipboard Text")},
          {QStringLiteral("value"), QStringLiteral("clipboard")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Fingerprint))},
          {QStringLiteral("title"), QStringLiteral("UUID")},
          {QStringLiteral("value"), QStringLiteral("uuid")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Calendar))},
          {QStringLiteral("title"), tr("Date")},
          {QStringLiteral("value"), QStringLiteral("date")},
          {QStringLiteral("template"), QStringLiteral("{date format=\"yyyy-MM-dd hh:mm\"}")},
          {QStringLiteral("cursorOffset"), 30},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::TextCursor))},
          {QStringLiteral("title"), tr("Argument")},
          {QStringLiteral("value"), QStringLiteral("argument")},
          {QStringLiteral("template"), QStringLiteral("{argument name=\"\"}")},
          {QStringLiteral("cursorOffset"), 16},
      },
      QVariantMap{
#ifdef Q_OS_WIN
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Powershell))},
          {QStringLiteral("title"), tr("PowerShell Command")},
#else
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin(BuiltinIcon::Terminal))},
          {QStringLiteral("title"), tr("Shell Command")},
#endif
          {QStringLiteral("value"), QStringLiteral("shell")},
          {QStringLiteral("template"), QStringLiteral("{shell code=\"\"}")},
          {QStringLiteral("cursorOffset"), 13},
      },
  };
}

#include "snippet-form-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/snippet/snippet-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "view-utils.hpp"
#include <QUrl>

SnippetFormViewHost::SnippetFormViewHost() : FormViewBase() {}

SnippetFormViewHost::SnippetFormViewHost(snippet::SerializedSnippet snippet, Mode mode)
    : FormViewBase(), m_mode(mode), m_initialSnippet(std::move(snippet)) {}

QUrl SnippetFormViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/SnippetFormView.qml"));
}

QVariantMap SnippetFormViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void SnippetFormViewHost::initialize() {
  BaseView::initialize();

  m_service = context()->services->snippetService();
  buildContentCompletions();

  QVariantList allApps;
  const auto *appDb = context()->services->appDb();
  for (const auto &app : appDb->list({.sortAlphabetically = true})) {
    if (!app->displayable() || !app->windowClass()) continue;
    QVariantMap entry;
    entry[QStringLiteral("id")] = app->id();
    entry[QStringLiteral("displayName")] = app->displayName();
    entry[QStringLiteral("iconSource")] = qml::imageSourceFor(app->iconUrl());
    allApps.append(entry);
  }

  QVariantMap section;
  section[QStringLiteral("title")] = QString();
  section[QStringLiteral("items")] = allApps;
  m_availableApps.append(section);

  auto panel = std::make_unique<FormActionPanelState>();
  auto section2 = panel->createSection();
  auto submitAction =
      new StaticAction(QStringLiteral("Submit"), ImageURL::builtin("enter-key"), [this]() { submit(); });
  section2->addAction(submitAction);
  setActions(std::move(panel));

  if (m_initialSnippet) {
    const auto &snippet = *m_initialSnippet;

    if (m_mode == Mode::Duplicate) {
      m_name = QString::fromStdString(std::format("Copy of {}", snippet.name));
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

      for (const auto &wmClass : snippet.expansion->apps) {
        m_apps.append(QString::fromStdString(wmClass));
      }
    }

    emit formChanged();
  }

  if (m_initialSnippet) {
    if (m_mode == Mode::Edit) {
      setNavigationTitle(QString("Edit \"%1\"").arg(QString::fromStdString(m_initialSnippet->name)));
    } else if (m_mode == Mode::Duplicate) {
      setNavigationTitle(QString("Duplicate \"%1\"").arg(QString::fromStdString(m_initialSnippet->name)));
    }
  }
}

void SnippetFormViewHost::submit() {
  const auto toast = context()->services->toastService();

  m_nameError.clear();
  m_contentError.clear();
  m_keywordError.clear();

  bool valid = true;

  if (m_name.size() < 2) {
    m_nameError = QStringLiteral("2 chars min.");
    valid = false;
  }
  if (m_content.isEmpty()) {
    m_contentError = QStringLiteral("Content should not be empty");
    valid = false;
  }
  if (!m_keyword.isEmpty()) {
    const bool hasSpaces = std::ranges::any_of(m_keyword, [](QChar c) { return c.isSpace(); });
    if (hasSpaces) {
      m_keywordError = QStringLiteral("No spaces");
      valid = false;
    }
  }

  emit errorsChanged();

  if (!valid) {
    toast->failure("Validation failed");
    return;
  }

  snippet::SnippetPayload payload;
  payload.name = m_name.toStdString();
  payload.data = snippet::TextSnippet(m_content.toStdString());

  if (!m_keyword.isEmpty()) {
    snippet::Expansion expansion;
    expansion.keyword = m_keyword.toStdString();
    expansion.word = m_expandAsWord;

    for (const auto &wmClass : m_apps) {
      expansion.apps.emplace_back(wmClass.toStdString());
    }

    payload.expansion = expansion;
  }

  if (m_mode == Mode::Edit && m_initialSnippet) {
    const auto result = m_service->updateSnippet(m_initialSnippet->id, payload);
    if (!result) {
      toast->failure(result.error().c_str());
      return;
    }
    toast->success("Snippet updated");
  } else {
    const auto result = m_service->createSnippet(payload);
    if (!result) {
      toast->failure(result.error().c_str());
      return;
    }
    toast->success("Snippet successfully created");
  }

  popSelf();
}

bool SnippetFormViewHost::serverRunning() const { return m_service && m_service->isServerRunning(); }

void SnippetFormViewHost::buildContentCompletions() {
  m_contentCompletions = QVariantList{
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("text-cursor"))},
          {QStringLiteral("title"), QStringLiteral("Cursor Position")},
          {QStringLiteral("value"), QStringLiteral("cursor")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("copy-clipboard"))},
          {QStringLiteral("title"), QStringLiteral("Clipboard Text")},
          {QStringLiteral("value"), QStringLiteral("clipboard")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("fingerprint"))},
          {QStringLiteral("title"), QStringLiteral("UUID")},
          {QStringLiteral("value"), QStringLiteral("uuid")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("calendar"))},
          {QStringLiteral("title"), QStringLiteral("Date")},
          {QStringLiteral("value"), QStringLiteral("date")},
      },
      QVariantMap{
          {QStringLiteral("iconSource"), qml::imageSourceFor(ImageURL::builtin("text-cursor"))},
          {QStringLiteral("title"), QStringLiteral("Argument")},
          {QStringLiteral("value"), QStringLiteral("argument")},
          {QStringLiteral("template"), QStringLiteral("{argument name=\"\"}")},
          {QStringLiteral("cursorOffset"), 16},
      },
  };
}

#include "snippet-form-view-host.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
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

  auto panel = std::make_unique<FormActionPanelState>();
  auto section = panel->createSection();
  auto submitAction =
      new StaticAction(QStringLiteral("Submit"), ImageURL::builtin("enter-key"), [this]() { submit(); });
  section->addAction(submitAction);
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

#include "manage-snippets-view-host.hpp"
#include "manage-snippets-model.hpp"
#include "placeholder.hpp"
#include "service-registry.hpp"
#include "services/snippet/snippet-service.hpp"
#include <QDateTime>
#include <ranges>

QUrl ManageSnippetsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/DetailListView.qml"));
}

QVariantMap ManageSnippetsViewHost::qmlProperties() const {
  return {{QStringLiteral("host"),
           QVariant::fromValue(const_cast<ManageSnippetsViewHost *>(this))}};
}

void ManageSnippetsViewHost::initialize() {
  BaseView::initialize();

  m_snippetService = context()->services->snippetService();
  m_model = new ManageSnippetsModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  setSearchPlaceholderText("Search by snippet name, contents or keyword...");

  connect(m_snippetService, &SnippetService::snippetsChanged, this,
          &ManageSnippetsViewHost::reload);

  connect(m_model, &ManageSnippetsModel::snippetSelected, this,
          &ManageSnippetsViewHost::loadDetail);

  connect(context()->navigation.get(), &NavigationController::completionValuesChanged, this,
          [this](const ArgumentValues &) { updateExpandedText(); });

  connect(m_model, &QAbstractItemModel::modelReset, this, [this]() {
    if (m_model->rowCount() == 0) clearDetail();
  });
}

void ManageSnippetsViewHost::loadInitialData() { reload(); }

void ManageSnippetsViewHost::textChanged(const QString &text) {
  m_model->setFilter(text);
}

void ManageSnippetsViewHost::onReactivated() {
  m_model->refreshActionPanel();
}

void ManageSnippetsViewHost::beforePop() {
  clearDetail();
  m_model->beforePop();
}

QObject *ManageSnippetsViewHost::listModel() const { return m_model; }

void ManageSnippetsViewHost::loadDetail(const snippet::SerializedSnippet &snippet) {
  m_currentSnippet = snippet;

  QVariantList meta;

  const auto typeStr = std::visit(
      overloads{[](const snippet::TextSnippet &) -> QString { return QStringLiteral("Text"); },
                [](const snippet::FileSnippet &) -> QString { return QStringLiteral("File"); }},
      snippet.data);

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Type")},
      {QStringLiteral("value"), typeStr},
  });

  meta.append(QVariantMap{
      {QStringLiteral("label"), QStringLiteral("Created at")},
      {QStringLiteral("value"), QDateTime::fromSecsSinceEpoch(snippet.createdAt).toString()},
  });

  if (snippet.updatedAt) {
    meta.append(QVariantMap{
        {QStringLiteral("label"), QStringLiteral("Updated at")},
        {QStringLiteral("value"), QDateTime::fromSecsSinceEpoch(*snippet.updatedAt).toString()},
    });
  }

  if (snippet.expansion) {
    meta.append(QVariantMap{
        {QStringLiteral("label"), QStringLiteral("Keyword")},
        {QStringLiteral("value"), QString::fromStdString(snippet.expansion->keyword)},
    });
  }

  m_detailMetadata = meta;

  // Create completer from parsed placeholders if it's a text snippet
  if (const auto text = std::get_if<snippet::TextSnippet>(&snippet.data)) {
    const auto parsed = PlaceholderString::parseSnippetText(text->text.c_str());
    const auto args = parsed.arguments();

    if (!args.empty()) {
      ArgumentList completerArgs;
      for (const auto &arg : args) {
        completerArgs.emplace_back(CommandArgument{
            .name = arg.name,
            .type = CommandArgument::Text,
            .placeholder = arg.name,
            .required = arg.defaultValue.isEmpty(),
        });
      }
      auto icon = std::visit(
          overloads{[](const snippet::TextSnippet &) { return ImageURL(BuiltinIcon::TextInput); },
                    [](const auto &) { return ImageURL(BuiltinIcon::BlankDocument); }},
          snippet.data);
      context()->navigation->createCompletion(completerArgs, icon);
    } else {
      context()->navigation->destroyCurrentCompletion();
    }
  } else {
    context()->navigation->destroyCurrentCompletion();
  }

  updateExpandedText();
}

void ManageSnippetsViewHost::updateExpandedText() {
  if (!m_currentSnippet) return;

  const auto visitor = overloads{
      [this](const snippet::TextSnippet &text) {
        const auto values = context()->navigation->completionValues();
        const auto result = m_expander.expand(text.text.c_str(), values);
        m_detailContent = result.parts |
                          std::views::transform([](auto &&r) { return r.text; }) |
                          std::views::join | std::ranges::to<QString>();
      },
      [this](const snippet::FileSnippet &file) {
        m_detailContent = QString::fromStdString(file.file);
      }};

  std::visit(visitor, m_currentSnippet->data);
  m_hasDetail = true;
  emit detailChanged();
}

void ManageSnippetsViewHost::clearDetail() {
  if (!m_hasDetail && !m_currentSnippet) return;
  m_currentSnippet.reset();
  m_hasDetail = false;
  m_detailContent.clear();
  m_detailMetadata.clear();
  context()->navigation->destroyCurrentCompletion();
  emit detailChanged();
}

void ManageSnippetsViewHost::reload() {
  m_model->setItems(m_snippetService->database()->snippets());
}

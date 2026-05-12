#include "manage-snippets-view-host.hpp"
#include "placeholder.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/snippet/snippet-service.hpp"
#include "snippet-form-view-host.hpp"
#include "view-utils.hpp"
#include <QDateTime>
#include <ranges>

QUrl ManageSnippetsViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/DetailListView.qml"));
}

QVariantMap ManageSnippetsViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void ManageSnippetsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  m_snippetService = context()->services->snippetService();

  m_section.setOnSnippetSelected([this](const snippet::SerializedSnippet &s) { loadDetail(s); });
  model()->addSource(&m_section);

  setSearchPlaceholderText("Search for snippets...");

  connect(m_snippetService, &SnippetService::snippetsChanged, this, &ManageSnippetsViewHost::reload);

  connect(context()->navigation.get(), &NavigationController::completionValuesChanged, this,
          [this](const ArgumentValues &) { updateExpandedText(); });

  connect(model(), &QAbstractItemModel::modelReset, this, [this]() {
    if (model()->rowCount() == 0) clearDetail();
  });
}

void ManageSnippetsViewHost::loadInitialData() { reload(); }

void ManageSnippetsViewHost::beforePop() {
  clearDetail();
  ListViewHost::beforePop();
}

void ManageSnippetsViewHost::loadDetail(const snippet::SerializedSnippet &snippet) {
  m_currentSnippet = snippet;

  QVariantList meta;

  const auto typeStr =
      std::visit(overloads{[](const snippet::TextSnippet &) -> QString { return QStringLiteral("Text"); },
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

    if (!snippet.expansion->apps.empty()) {
      const auto *appDb = context()->services->appDb();
      QVariantList icons;
      for (const auto &appId : snippet.expansion->apps) {
        const auto app = appDb->findById(QString::fromStdString(appId));
        QVariantMap entry;
        entry[QStringLiteral("icon")] = app ? qml::imageSourceFor(app->iconUrl()) : QString();
        entry[QStringLiteral("tooltip")] = app ? app->displayName() : QString::fromStdString(appId);
        icons.append(entry);
      }
      meta.append(QVariantMap{
          {QStringLiteral("type"), QStringLiteral("icons")},
          {QStringLiteral("label"), QStringLiteral("Apps")},
          {QStringLiteral("icons"), icons},
      });
    }
  }

  m_detailMetadata = meta;

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
      auto icon =
          std::visit(overloads{[](const snippet::TextSnippet &) { return ImageURL(BuiltinIcon::TextInput); },
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

  m_detailContent.clear();

  if (const auto *text = std::get_if<snippet::TextSnippet>(&m_currentSnippet->data)) {
    const auto values = context()->navigation->completionValues();
    const auto result = m_expander.expand(text->text.c_str(), values, {.executeShell = false});
    m_detailContent = result.parts | std::views::transform([](auto &&r) { return r.text; }) |
                      std::views::join | std::ranges::to<QString>();
  }

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

void ManageSnippetsViewHost::createSnippet() { context()->navigation->pushView(new SnippetFormViewHost()); }

std::unique_ptr<ActionPanelState> ManageSnippetsViewHost::emptyActionPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  auto *create = new StaticAction("Create snippet", BuiltinIcon::Plus,
                                  [this](ApplicationContext *) { createSnippet(); });
  create->setPrimary(true);
  section->addAction(create);
  return panel;
}

void ManageSnippetsViewHost::reload() { m_section.setItems(m_snippetService->database()->snippets()); }

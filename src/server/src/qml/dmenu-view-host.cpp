#include "dmenu-view-host.hpp"
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "utils/utils.hpp"
#include "view-utils.hpp"
#include <ranges>

namespace fs = std::filesystem;

QUrl DMenuViewHost::qmlComponentUrl() const {
  if (m_data.noQuickLook) { return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml")); }
  return QUrl(QStringLiteral("qrc:/Vicinae/DMenuView.qml"));
}

QVariantMap DMenuViewHost::qmlProperties() {
  if (m_data.noQuickLook) {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(model()))}};
  }
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void DMenuViewHost::initialize() {
  BaseView::initialize();
  initModel();

  if (m_data.noQuickLook) m_section.setNoQuickLook(true);
  if (m_data.noSection) m_section.setNoSection(true);
  if (m_data.sectionTitle) m_section.setSectionTemplate(*m_data.sectionTitle);
  if (m_data.noFooter) setStatusBarVisiblity(false);
  if (m_data.navigationTitle) setNavigationTitle(QString::fromStdString(*m_data.navigationTitle));

  setSearchPlaceholderText(m_data.placeholder.value_or("Search entries...").c_str());

  auto entries = std::views::split(m_data.rawContent, std::string_view("\n")) |
                 std::views::transform([](auto &&s) { return std::string_view(s); }) |
                 std::views::filter([](auto &&s) { return !s.empty(); }) | std::ranges::to<std::vector>();
  m_section.setRawEntries(std::move(entries));

  auto onChosen = [this](const QString &text) {
    m_selected = true;
    emit selected(text);
  };
  m_section.setOnEntryChosen(onChosen);

  if (!m_data.noQuickLook) {
    m_section.setOnFileHighlighted([this](std::string_view path) { loadDetail(path); });
  }

  model()->addSource(&m_section);
}

void DMenuViewHost::loadInitialData() {
  if (m_data.query) {
    setSearchText(m_data.query.value_or("").c_str());
  } else {
    model()->setFilter({});
  }
}

void DMenuViewHost::textChanged(const QString &text) {
  clearDetail();
  model()->setFilter(text);
}

void DMenuViewHost::beforePop() {
  if (!m_selected) { emit selected(""); }
}

void DMenuViewHost::loadDetail(std::string_view path) {
  auto fspath = fs::path(path);

  m_hasDetail = true;
  m_detailName = QString::fromStdString(getLastPathComponent(fspath));
  m_detailPath = QString::fromUtf8(path.data(), path.size());

  auto preview = qml::resolveFilePreview(fspath, m_mimeDb);
  m_detailMimeType = preview.mimeType;
  m_detailImageSource = preview.imageSource;
  m_detailTextContent = preview.textContent;

  emit detailChanged();
}

void DMenuViewHost::clearDetail() {
  if (!m_hasDetail) return;
  m_hasDetail = false;
  m_detailName.clear();
  m_detailPath.clear();
  m_detailMimeType.clear();
  m_detailImageSource.clear();
  m_detailTextContent.clear();
  emit detailChanged();
}

std::unique_ptr<ActionPanelState> DMenuViewHost::emptyActionPanel() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  auto onChosen = [this](const QString &text, ApplicationContext *ctx) {
    m_selected = true;
    emit selected(text);
    ctx->navigation->closeWindow();
  };

  auto *select = new StaticAction("Pass search text", BuiltinIcon::SaveDocument,
                                  [this, onChosen](ApplicationContext *ctx) { onChosen(searchText(), ctx); });
  select->setPrimary(true);
  section->addAction(select);

  auto *selectAndCopy = new StaticAction("Pass and copy search text", BuiltinIcon::CopyClipboard,
                                         [this, onChosen](ApplicationContext *ctx) {
                                           auto text = searchText();
                                           ctx->services->clipman()->copyText(text);
                                           onChosen(text, ctx);
                                         });
  selectAndCopy->setShortcut(Keybind::CopyAction);
  section->addAction(selectAndCopy);

  return panel;
}

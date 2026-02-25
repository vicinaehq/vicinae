#include "dmenu-view-host.hpp"
#include "dmenu-model.hpp"
#include "utils/utils.hpp"
#include "view-utils.hpp"
#include <ranges>

namespace fs = std::filesystem;

DMenuViewHost::DMenuViewHost(ipc::DMenu::Request data) : m_data(std::move(data)) {}

QUrl DMenuViewHost::qmlComponentUrl() const {
  if (m_data.noQuickLook) {
    return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
  }
  return QUrl(QStringLiteral("qrc:/Vicinae/DMenuView.qml"));
}

QVariantMap DMenuViewHost::qmlProperties() const {
  if (m_data.noQuickLook) {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
  }
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void DMenuViewHost::initialize() {
  BaseView::initialize();

  m_model = new DMenuModel(this);
  m_model->setScope(ViewScope(context(), this));
  m_model->initialize();

  if (m_data.noSection) m_model->setNoSection(true);
  if (m_data.sectionTitle) m_model->setSectionTemplate(*m_data.sectionTitle);
  if (m_data.noFooter) setStatusBarVisiblity(false);

  setSearchPlaceholderText(m_data.placeholder.value_or("Search entries...").c_str());

  auto entries = std::views::split(m_data.rawContent, std::string_view("\n")) |
                 std::views::transform([](auto &&s) { return std::string_view(s); }) |
                 std::views::filter([](auto &&s) { return !s.empty(); }) |
                 std::ranges::to<std::vector>();
  m_model->setRawEntries(std::move(entries));

  connect(m_model, &DMenuModel::entryChosen, this, [this](const QString &text) {
    m_selected = true;
    emit selected(text);
  });

  if (!m_data.noQuickLook) {
    connect(m_model, &DMenuModel::fileHighlighted, this, &DMenuViewHost::loadDetail);
  }
}

void DMenuViewHost::loadInitialData() {
  if (m_data.query) {
    setSearchText(m_data.query.value_or("").c_str());
  } else {
    m_model->setFilter({});
  }
}

void DMenuViewHost::textChanged(const QString &text) {
  clearDetail();
  m_model->setFilter(text);
}

void DMenuViewHost::beforePop() {
  if (!m_selected) {
    emit selected("");
  }
}

QObject *DMenuViewHost::listModel() const { return m_model; }

void DMenuViewHost::loadDetail(std::string_view path) {
  auto qpath = QString::fromUtf8(path.data(), path.size());
  auto fspath = fs::path(path);

  m_hasDetail = true;
  m_detailName = QString::fromStdString(getLastPathComponent(fspath));
  m_detailPath = qpath;

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

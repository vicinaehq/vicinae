#include "qml-dmenu-view-host.hpp"
#include "qml-dmenu-model.hpp"
#include "utils/utils.hpp"
#include <QFile>
#include <ranges>

namespace fs = std::filesystem;

QmlDMenuViewHost::QmlDMenuViewHost(ipc::DMenu::Request data) : m_data(std::move(data)) {}

QUrl QmlDMenuViewHost::qmlComponentUrl() const {
  if (m_data.noQuickLook) {
    return QUrl(QStringLiteral("qrc:/Vicinae/CommandListView.qml"));
  }
  return QUrl(QStringLiteral("qrc:/Vicinae/DMenuView.qml"));
}

QVariantMap QmlDMenuViewHost::qmlProperties() const {
  if (m_data.noQuickLook) {
    return {{QStringLiteral("cmdModel"), QVariant::fromValue(static_cast<QObject *>(m_model))}};
  }
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void QmlDMenuViewHost::initialize() {
  BaseView::initialize();

  m_model = new QmlDMenuModel(this);
  m_model->initialize(context());

  if (m_data.noSection) m_model->setNoSection(true);
  if (m_data.sectionTitle) m_model->setSectionTemplate(*m_data.sectionTitle);
  if (m_data.noFooter) setStatusBarVisiblity(false);

  setSearchPlaceholderText(m_data.placeholder.value_or("Search entries...").c_str());

  auto entries = std::views::split(m_data.rawContent, std::string_view("\n")) |
                 std::views::transform([](auto &&s) { return std::string_view(s); }) |
                 std::views::filter([](auto &&s) { return !s.empty(); }) |
                 std::ranges::to<std::vector>();
  m_model->setRawEntries(std::move(entries));

  connect(m_model, &QmlDMenuModel::entryChosen, this, [this](const QString &text) {
    m_selected = true;
    emit selected(text);
  });

  if (!m_data.noQuickLook) {
    connect(m_model, &QmlDMenuModel::fileHighlighted, this, &QmlDMenuViewHost::loadDetail);
  }
}

void QmlDMenuViewHost::loadInitialData() {
  if (m_data.query) {
    setSearchText(m_data.query.value_or("").c_str());
  } else {
    m_model->setFilter({});
  }
}

void QmlDMenuViewHost::textChanged(const QString &text) {
  clearDetail();
  m_model->setFilter(text);
}

void QmlDMenuViewHost::beforePop() {
  if (!m_selected) {
    emit selected("");
  }
}

QObject *QmlDMenuViewHost::listModel() const { return m_model; }

void QmlDMenuViewHost::loadDetail(std::string_view path) {
  auto qpath = QString::fromUtf8(path.data(), path.size());
  auto fspath = fs::path(path);

  m_hasDetail = true;
  m_detailName = QString::fromStdString(getLastPathComponent(fspath));
  m_detailPath = qpath;

  auto mime = m_mimeDb.mimeTypeForFile(qpath);
  m_detailMimeType = mime.name();

  m_detailImageSource.clear();
  m_detailTextContent.clear();

  if (mime.name().startsWith("image/")) {
    m_detailImageSource = qml::imageSourceFor(ImageURL::local(fspath));
  } else if (Utils::isTextMimeType(mime)) {
    QFile file(qpath);
    if (file.open(QIODevice::ReadOnly)) {
      static constexpr qint64 MAX_PREVIEW = 32 * 1024;
      m_detailTextContent = QString::fromUtf8(file.read(MAX_PREVIEW));
    }
  } else {
    m_detailImageSource =
        qml::imageSourceFor(ImageURL::system(mime.iconName()).withFallback(ImageURL::system(mime.genericIconName())));
  }

  emit detailChanged();
}

void QmlDMenuViewHost::clearDetail() {
  if (!m_hasDetail) return;
  m_hasDetail = false;
  m_detailName.clear();
  m_detailPath.clear();
  m_detailMimeType.clear();
  m_detailImageSource.clear();
  m_detailTextContent.clear();
  emit detailChanged();
}

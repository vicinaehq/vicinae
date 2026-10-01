#include <QLocale>
#include <algorithm>
#include "builtins/screenshots/screenshot-grid-model.hpp"
#include "builtins/screenshots/screenshot-actions.hpp"
#include "services/clipboard/clipboard-mime.hpp"

QString ScreenshotGridSource::itemTitle(int i) const {
  return QLocale().toString(items.at(i).createdAt, QLocale::ShortFormat);
}

QString ScreenshotGridSource::itemSubtitle(int i) const {
  const auto &item = items.at(i);
  if (item.kind == Screenshot::Kind::Recording) {
    if (!item.durationSeconds) return tr("Screen Recording");
    const auto seconds = static_cast<qint64>(*item.durationSeconds);
    const auto duration =
        seconds >= 3600
            ? QStringLiteral("%1:%2:%3")
                  .arg(seconds / 3600)
                  .arg(seconds / 60 % 60, 2, 10, QLatin1Char('0'))
                  .arg(seconds % 60, 2, 10, QLatin1Char('0'))
            : QStringLiteral("%1:%2").arg(seconds / 60).arg(seconds % 60, 2, 10, QLatin1Char('0'));
    return tr("Recording · %1").arg(duration);
  }
  const auto size = item.size;
  return QStringLiteral("%1 × %2").arg(size.width()).arg(size.height());
}

QString ScreenshotGridSource::itemTooltip(int i) const {
  return QString::fromStdString(items.at(i).path.string());
}

std::optional<ImageURL> ScreenshotGridSource::itemIcon(int i) const {
  const auto &item = items.at(i);
  if (item.kind == Screenshot::Kind::Recording) {
    return ImageURL::fileThumbnail(item.path).withFallback(ImageURL::builtin(BuiltinIcon::Video));
  }
  return ImageURL::local(item.path);
}

std::unique_ptr<QMimeData> ScreenshotGridSource::dragMimeData(int i) const {
  return Clipboard::mimeDataForContent(Clipboard::File{items.at(i).path});
}

std::unique_ptr<ActionPanelState> ScreenshotGridSource::actionPanel(int i) const {
  return ScreenshotActions::panel(items.at(i), scope().appContext());
}

ScreenshotGridModel::ScreenshotGridModel(QObject *parent) : SectionGridModel(parent) {}

void ScreenshotGridModel::initialize() {
  setColumns(3);
  setAspectRatio(16.0 / 10.0);
  setInset(0.04);
  for (auto &section : m_sections)
    addSource(&section);
}

void ScreenshotGridModel::setItems(const std::vector<Screenshot> &items) {
  m_items = items;
  rebuildResults(true);
}

void ScreenshotGridModel::setFilter(const QString &text) {
  if (m_filter == text) return;
  m_filter = text;
  rebuildResults(false);
}

void ScreenshotGridModel::rebuildResults(bool preserveSelection) {
  std::optional<std::filesystem::path> selected;
  if (preserveSelection && selectedSection() >= 0 && selectedItem() >= 0) {
    const auto &items = m_sections.at(selectedSection()).items;
    if (std::cmp_less(selectedItem(), items.size())) selected = items[selectedItem()].path;
  }
  for (auto &section : m_sections) {
    section.items.clear();
    section.items.reserve(m_items.size());
  }
  m_sections[0].name = tr("Today");
  m_sections[1].name = tr("Yesterday");
  m_sections[2].name = tr("Older");

  std::vector<Scored<int>> matches;
  fuzzy::fuzzyFilter<Screenshot>(m_items, m_filter.toStdString(), matches);
  const auto today = QDate::currentDate();
  for (const auto &match : matches) {
    const auto &item = m_items[match.data];
    const auto date = item.createdAt.toLocalTime().date();
    const auto section = !m_filter.isEmpty() || date == today ? 0 : date == today.addDays(-1) ? 1 : 2;
    m_sections[section].items.emplace_back(item);
  }
  if (!m_filter.isEmpty()) m_sections[0].name = tr("Search Results");

  setSelectFirstOnReset(true);
  rebuild();
  setSelectFirstOnReset(false);
  if (selected) {
    for (std::size_t sectionIndex = 0; sectionIndex < m_sections.size(); ++sectionIndex) {
      const auto &items = m_sections[sectionIndex].items;
      const auto found = std::ranges::find(items, *selected, &Screenshot::path);
      if (found != items.end()) {
        select(static_cast<int>(sectionIndex), static_cast<int>(std::distance(items.begin(), found)));
        return;
      }
    }
  }
  selectFirst();
}

void ScreenshotGridModel::onSelectionCleared() { scope().setActions(ScreenshotActions::emptyPanel()); }

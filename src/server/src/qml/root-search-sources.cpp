#include "root-search-sources.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "keyboard/keybind.hpp"
#include "misc/file-list-item.hpp"
#include "navigation-controller.hpp"
#include "services/news/news-service.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "utils/utils.hpp"
#include <format>

namespace root_search {

const QHash<int, QByteArray> &customRoleNames() {
  static const QHash<int, QByteArray> roles = {
      {ItemType, "itemType"},
      {Alias, "alias"},
      {IsActive, "isActive"},
      {AccessoryText, "accessoryText"},
      {AccessoryColor, "accessoryColor"},
      {IsCalculator, "isCalculator"},
      {CalcQuestion, "calcQuestion"},
      {CalcQuestionUnit, "calcQuestionUnit"},
      {CalcAnswer, "calcAnswer"},
      {CalcAnswerUnit, "calcAnswerUnit"},
      {IsFile, "isFile"},
  };
  return roles;
}

const QHash<int, QVariant> &customRoleDefaults() {
  static const QHash<int, QVariant> defaults = {
      {ItemType, QString()},
      {Alias, QString()},
      {IsActive, false},
      {AccessoryText, QString()},
      {AccessoryColor, QString()},
      {IsCalculator, false},
      {CalcQuestion, QString()},
      {CalcQuestionUnit, QString()},
      {CalcAnswer, QString()},
      {CalcAnswerUnit, QString()},
      {IsFile, false},
  };
  return defaults;
}

QString resolveAccessoryColor(const std::optional<ColorLike> &color) {
  if (!color) return {};
  const auto &theme = ThemeService::instance().theme();
  return std::visit(
      [&](auto &&c) -> QString {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, QColor>) {
          return c.name(QColor::HexArgb);
        } else if constexpr (std::is_same_v<T, QString>) {
          return c;
        } else if constexpr (std::is_same_v<T, SemanticColor>) {
          return theme.resolve(c).name(QColor::HexArgb);
        } else if constexpr (std::is_same_v<T, DynamicColor>) {
          return theme.isLight() ? c.light : c.dark;
        } else {
          return {};
        }
      },
      *color);
}

} // namespace root_search

namespace {

QVariant rootItemAccessoryData(const RootItem *item, int role) {
  if (!item) return {};
  auto acc = item->accessories();
  if (acc.empty()) return {};
  if (role == AccessoryText) return acc.front().text;
  if (role == AccessoryColor) return root_search::resolveAccessoryColor(acc.front().color);
  return {};
}

} // namespace

QString RootLinkSection::itemId(int) const { return m_link ? m_link->url : QString(); }

QString RootLinkSection::itemTitle(int) const { return m_link ? m_link->url : QString(); }

QString RootLinkSection::itemIconSource(int) const {
  return m_link ? imageSourceFor(m_link->app->iconUrl()) : QString();
}

QVariant RootLinkSection::customData(int, int role) const {
  if (role == ItemType) return QStringLiteral("link");
  return {};
}

QHash<int, QByteArray> RootLinkSection::customRoleNames() const { return root_search::customRoleNames(); }
QHash<int, QVariant> RootLinkSection::customRoleDefaults() const { return root_search::customRoleDefaults(); }

std::unique_ptr<ActionPanelState> RootLinkSection::actionPanel(int) const {
  if (!m_link) return nullptr;
  auto panel = std::make_unique<ActionPanelState>();
  auto *section = panel->createSection();
  auto *open =
      new OpenAppAction(m_link->app, QString("Open in %1").arg(m_link->app->displayName()), {m_link->url});
  open->setClearSearch(true);
  section->addAction(open);
  return panel;
}

QString RootCalculatorSection::itemId(int) const {
  return m_result ? QStringLiteral("calc:") + m_result->question.text : QString();
}

QString RootCalculatorSection::itemTitle(int) const {
  return m_result ? m_result->question.text + QStringLiteral(" = ") + m_result->answer.text : QString();
}

QString RootCalculatorSection::itemIconSource(int) const {
  return imageSourceFor(ImageURL::builtin("calculator"));
}

QVariant RootCalculatorSection::customData(int, int role) const {
  if (role == ItemType) return QStringLiteral("calculator");
  if (role == IsCalculator) return true;
  if (!m_result) return {};
  switch (role) {
  case CalcQuestion:
    return m_result->question.text;
  case CalcQuestionUnit:
    return m_result->question.unit ? m_result->question.unit->displayName : QString();
  case CalcAnswer:
    return m_result->answer.text;
  case CalcAnswerUnit:
    return m_result->answer.unit ? m_result->answer.unit->displayName : QString();
  default:
    return {};
  }
}

QHash<int, QByteArray> RootCalculatorSection::customRoleNames() const {
  return root_search::customRoleNames();
}
QHash<int, QVariant> RootCalculatorSection::customRoleDefaults() const {
  return root_search::customRoleDefaults();
}

std::unique_ptr<ActionPanelState> RootCalculatorSection::actionPanel(int) const {
  if (!m_result) return nullptr;
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();
  auto *copyAnswer = new CopyCalculatorAnswerAction(*m_result);
  copyAnswer->setPrimary(true);
  section->addAction(copyAnswer);
  section->addAction(new CopyCalculatorQuestionAndAnswerAction(*m_result));
  section->addAction(new PutCalculatorAnswerInSearchBar(*m_result));
  section->addAction(new OpenCalculatorHistoryAction());
  return panel;
}

QString RootNewsSection::itemId(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return {};
  return QStringLiteral("news:") + QString::fromStdString(m_items[i]->id);
}

QString RootNewsSection::itemTitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return {};
  return m_items[i]->title;
}

QString RootNewsSection::itemSubtitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return {};
  return m_items[i]->subtitle;
}

QString RootNewsSection::itemIconSource(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return {};
  return imageSourceFor(m_items[i]->icon);
}

QVariant RootNewsSection::customData(int, int role) const {
  if (role == ItemType) return QStringLiteral("news");
  return {};
}

QHash<int, QByteArray> RootNewsSection::customRoleNames() const { return root_search::customRoleNames(); }
QHash<int, QVariant> RootNewsSection::customRoleDefaults() const { return root_search::customRoleDefaults(); }

std::unique_ptr<ActionPanelState> RootNewsSection::actionPanel(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return nullptr;
  const auto *news = m_items[i];
  if (!news->actionFactory) return nullptr;
  auto panel = news->actionFactory(scope().appContext());
  auto *dismissAction = new DismissNewsAction(news->id);
  dismissAction->setShortcut(Keybind::RemoveAction);
  panel->createSection()->addAction(dismissAction);
  return panel;
}

QString RootFavoritesSection::itemId(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return QString::fromStdString(m_items[i]->uniqueId());
}

QString RootFavoritesSection::itemTitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return m_items[i]->title();
}

QString RootFavoritesSection::itemSubtitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return m_items[i]->subtitle();
}

QString RootFavoritesSection::itemIconSource(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return imageSourceFor(m_items[i]->iconUrl());
}

QVariant RootFavoritesSection::customData(int i, int role) const {
  if (role == ItemType) return QStringLiteral("favorite");
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  switch (role) {
  case Alias: {
    auto meta = m_manager->itemMetadata(m_items[i]->uniqueId());
    return QString::fromStdString(meta.alias.value_or(""));
  }
  case IsActive:
    return m_items[i]->isActive();
  case AccessoryText:
  case AccessoryColor:
    return rootItemAccessoryData(m_items[i].get(), role);
  default:
    return {};
  }
}

QHash<int, QByteArray> RootFavoritesSection::customRoleNames() const {
  return root_search::customRoleNames();
}
QHash<int, QVariant> RootFavoritesSection::customRoleDefaults() const {
  return root_search::customRoleDefaults();
}

std::unique_ptr<ActionPanelState> RootFavoritesSection::actionPanel(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return nullptr;
  return m_items[i]->newActionPanel(scope().appContext(), m_manager->itemMetadata(m_items[i]->uniqueId()));
}

const RootItem *RootFavoritesSection::rootItem(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return nullptr;
  return m_items[i].get();
}

QString RootResultsSection::sectionName() const {
  if (m_queryEmpty) return QStringLiteral("Suggestions");
  return QString::fromStdString(std::format("Results ({})", m_items.size()));
}

QString RootResultsSection::itemId(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i].item) return {};
  return QString::fromStdString(m_items[i].item->uniqueId());
}

QString RootResultsSection::itemTitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i].item) return {};
  return m_items[i].item->title();
}

QString RootResultsSection::itemSubtitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i].item) return {};
  return m_items[i].item->subtitle();
}

QString RootResultsSection::itemIconSource(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i].item) return {};
  return imageSourceFor(m_items[i].item->iconUrl());
}

QVariant RootResultsSection::customData(int i, int role) const {
  if (role == ItemType) return QStringLiteral("result");
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i].item) return {};
  switch (role) {
  case Alias:
    return QString::fromStdString(m_items[i].meta.alias.value_or(""));
  case IsActive:
    return m_items[i].item->isActive();
  case AccessoryText:
  case AccessoryColor:
    return rootItemAccessoryData(m_items[i].item.get(), role);
  default:
    return {};
  }
}

QHash<int, QByteArray> RootResultsSection::customRoleNames() const { return root_search::customRoleNames(); }
QHash<int, QVariant> RootResultsSection::customRoleDefaults() const {
  return root_search::customRoleDefaults();
}

std::unique_ptr<ActionPanelState> RootResultsSection::actionPanel(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i].item) return nullptr;
  return m_items[i].item->newActionPanel(scope().appContext(), m_items[i].meta);
}

const RootItem *RootResultsSection::rootItem(int i) const {
  if (std::cmp_greater_equal(i, m_items.size())) return nullptr;
  return m_items[i].item.get();
}

QString RootFilesSection::itemId(int i) const {
  if (std::cmp_greater_equal(i, m_files.size())) return {};
  return QString::fromStdString(m_files[i].path.string());
}

QString RootFilesSection::itemTitle(int i) const {
  if (std::cmp_greater_equal(i, m_files.size())) return {};
  return QString::fromStdString(m_files[i].path.filename().string());
}

QString RootFilesSection::itemSubtitle(int i) const {
  if (std::cmp_greater_equal(i, m_files.size())) return {};
  return QString::fromStdString(compressPath(m_files[i].path.parent_path()));
}

QString RootFilesSection::itemIconSource(int i) const {
  if (std::cmp_greater_equal(i, m_files.size())) return {};
  return imageSourceFor(ImageURL::fileIcon(m_files[i].path));
}

QVariant RootFilesSection::customData(int, int role) const {
  if (role == ItemType) return QStringLiteral("file");
  if (role == IsFile) return true;
  return {};
}

QHash<int, QByteArray> RootFilesSection::customRoleNames() const { return root_search::customRoleNames(); }
QHash<int, QVariant> RootFilesSection::customRoleDefaults() const {
  return root_search::customRoleDefaults();
}

std::unique_ptr<ActionPanelState> RootFilesSection::actionPanel(int i) const {
  if (std::cmp_greater_equal(i, m_files.size())) return nullptr;
  return FileActions::actionPanel(m_files[i].path, m_appDb);
}

QString RootFallbackSection::sectionName() const {
  constexpr std::size_t MAX_QUERY_LEN = 30;
  auto truncated = m_query.length() > MAX_QUERY_LEN ? m_query.substr(0, MAX_QUERY_LEN) + "..." : m_query;
  return QString::fromStdString(std::format("Use \"{}\" with...", truncated));
}

QString RootFallbackSection::itemId(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return QString::fromStdString(m_items[i]->uniqueId());
}

QString RootFallbackSection::itemTitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return m_items[i]->title();
}

QString RootFallbackSection::itemSubtitle(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return m_items[i]->subtitle();
}

QString RootFallbackSection::itemIconSource(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  return imageSourceFor(m_items[i]->iconUrl());
}

QVariant RootFallbackSection::customData(int i, int role) const {
  if (role == ItemType) return QStringLiteral("fallback");
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return {};
  switch (role) {
  case IsActive:
    return m_items[i]->isActive();
  case AccessoryText:
  case AccessoryColor:
    return rootItemAccessoryData(m_items[i].get(), role);
  default:
    return {};
  }
}

QHash<int, QByteArray> RootFallbackSection::customRoleNames() const { return root_search::customRoleNames(); }
QHash<int, QVariant> RootFallbackSection::customRoleDefaults() const {
  return root_search::customRoleDefaults();
}

std::unique_ptr<ActionPanelState> RootFallbackSection::actionPanel(int i) const {
  if (std::cmp_greater_equal(i, m_items.size()) || !m_items[i]) return nullptr;
  return m_items[i]->fallbackActionPanel(scope().appContext(),
                                         m_manager->itemMetadata(m_items[i]->uniqueId()));
}

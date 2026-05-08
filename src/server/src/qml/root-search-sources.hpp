#pragma once
#include "section-source.hpp"
#include "services/app-service/abstract-app-db.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class AppService;
class NewsService;
struct NewsItem;

enum RootSearchRole {
  ItemType = Qt::UserRole + 50,
  Alias,
  IsActive,
  AccessoryText,
  AccessoryColor,
  IsCalculator,
  CalcQuestion,
  CalcQuestionUnit,
  CalcAnswer,
  CalcAnswerUnit,
  IsFile,
};

struct OwnedResult {
  std::shared_ptr<RootItem> item;
  RootItemMetadata meta;
};

struct LinkItem {
  std::shared_ptr<AbstractApplication> app;
  QString url;
};

namespace root_search {

const QHash<int, QByteArray> &customRoleNames();
const QHash<int, QVariant> &customRoleDefaults();
QString resolveAccessoryColor(const std::optional<ColorLike> &color);

} // namespace root_search

class RootLinkSection : public SectionSource {
public:
  QString sectionName() const override { return QStringLiteral("Link"); }
  int count() const override { return m_link ? 1 : 0; }
  QString itemId(int) const override;
  QString itemTitle(int) const override;
  QString itemIconSource(int) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int) const override;

  void setLink(std::optional<LinkItem> link) { m_link = std::move(link); }
  const std::optional<LinkItem> &link() const { return m_link; }

private:
  std::optional<LinkItem> m_link;
};

class RootCalculatorSection : public SectionSource {
public:
  QString sectionName() const override { return QStringLiteral("Calculator"); }
  int count() const override { return m_result ? 1 : 0; }
  QString itemId(int) const override;
  QString itemTitle(int) const override;
  QString itemIconSource(int) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int) const override;

  void setResult(std::optional<AbstractCalculatorBackend::CalculatorResult> r) { m_result = std::move(r); }
  const auto &result() const { return m_result; }

private:
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_result;
};

class RootNewsSection : public SectionSource {
public:
  QString sectionName() const override { return QStringLiteral("What's New"); }
  int count() const override { return static_cast<int>(m_items.size()); }
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

  void setItems(std::vector<const NewsItem *> items) { m_items = std::move(items); }

private:
  std::vector<const NewsItem *> m_items;
};

class RootFavoritesSection : public SectionSource {
public:
  explicit RootFavoritesSection(RootItemManager *mgr) : m_manager(mgr) {}

  QString sectionName() const override { return QStringLiteral("Favorites"); }
  int count() const override { return static_cast<int>(m_items.size()); }
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

  void setItems(std::vector<std::shared_ptr<RootItem>> items) { m_items = std::move(items); }
  const RootItem *rootItem(int i) const;

private:
  RootItemManager *m_manager;
  std::vector<std::shared_ptr<RootItem>> m_items;
};

class RootResultsSection : public SectionSource {
public:
  explicit RootResultsSection(RootItemManager *mgr) : m_manager(mgr) {}

  QString sectionName() const override;
  int count() const override { return static_cast<int>(m_items.size()); }
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

  void setItems(std::vector<OwnedResult> items) { m_items = std::move(items); }
  void setQueryEmpty(bool empty) { m_queryEmpty = empty; }
  const RootItem *rootItem(int i) const;

private:
  RootItemManager *m_manager;
  std::vector<OwnedResult> m_items;
  bool m_queryEmpty = true;
};

class RootFilesSection : public SectionSource {
public:
  explicit RootFilesSection(AppService *appDb) : m_appDb(appDb) {}

  QString sectionName() const override { return QStringLiteral("Files"); }
  int count() const override { return static_cast<int>(m_files.size()); }
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

  void setFiles(std::vector<IndexerFileResult> files) { m_files = std::move(files); }

private:
  AppService *m_appDb;
  std::vector<IndexerFileResult> m_files;
};

class RootFallbackSection : public SectionSource {
public:
  explicit RootFallbackSection(RootItemManager *mgr) : m_manager(mgr) {}

  QString sectionName() const override;
  int count() const override { return static_cast<int>(m_items.size()); }
  QString itemId(int i) const override;
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariant customData(int i, int role) const override;
  QHash<int, QByteArray> customRoleNames() const override;
  QHash<int, QVariant> customRoleDefaults() const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

  void setItems(std::vector<std::shared_ptr<RootItem>> items) { m_items = std::move(items); }
  void setQuery(std::string query) { m_query = std::move(query); }

private:
  RootItemManager *m_manager;
  std::vector<std::shared_ptr<RootItem>> m_items;
  std::string m_query;
};

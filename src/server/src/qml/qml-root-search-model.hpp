#pragma once
#include "common/context.hpp"
#include "extensions/root/root-search-model.hpp"
#include "qml-utils.hpp"
#include "services/app-service/app-service.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <QAbstractListModel>
#include <QFutureWatcher>
#include <QTimer>

class ActionPanelState;

namespace config {
class Manager;
}

class QmlRootSearchModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(QString primaryActionTitle READ primaryActionTitle NOTIFY primaryActionChanged)
  Q_PROPERTY(QString primaryActionIcon READ primaryActionIcon NOTIFY primaryActionChanged)

public:
  enum Role {
    IsSection = Qt::UserRole + 1,
    IsSelectable,
    SectionName,
    ItemType,
    Title,
    Subtitle,
    IconSource,
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

  explicit QmlRootSearchModel(ApplicationContext &ctx, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void setFilter(const QString &text);
  Q_INVOKABLE int nextSelectableIndex(int from, int direction) const;
  Q_INVOKABLE void setSelectedIndex(int index);
  Q_INVOKABLE void activateSelected();
  Q_INVOKABLE bool tryAliasFastTrack();

  QString primaryActionTitle() const;
  QString primaryActionIcon() const;

signals:
  void primaryActionChanged();

private:
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;
  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  // Safe copy of a search result that owns the shared_ptr
  // (ScoredItem uses reference_wrapper which can dangle)
  struct OwnedResult {
    std::shared_ptr<RootItem> item;
    RootItemMetadata meta;
  };

  struct FlatItem {
    enum Kind {
      SectionHeader,
      ResultItem,
      FallbackItem,
      FavoriteItem,
      LinkItem,
      CalculatorItem,
      FileItem
    } kind;
    int dataIndex;
    SectionType section;
  };

  void rebuildFlatList();
  void addSection(SectionType section, const std::string &name, int count);
  QString imageSourceFor(const ImageURL &url) const { return qml::imageSourceFor(url); }
  QString itemTypeString(FlatItem::Kind kind) const;
  QString resolveAccessoryColor(const std::optional<ColorLike> &color) const;

  void startCalculator();
  void handleCalculatorFinished();
  void startFileSearch();
  void handleFileSearchFinished();

  static constexpr int MIN_FS_TEXT_LENGTH = 3;

  ApplicationContext *m_ctx;
  RootItemManager *m_manager;
  AppService *m_appDb;
  CalculatorService *m_calculator;
  FileService *m_fileService;
  config::Manager *m_config;

  std::string m_query;
  std::vector<FlatItem> m_flat;
  std::vector<OwnedResult> m_results;
  std::vector<std::shared_ptr<RootItem>> m_fallbackItems;
  std::vector<std::shared_ptr<RootItem>> m_favorites;
  std::optional<LinkItem> m_defaultOpener;
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calc;
  std::vector<IndexerFileResult> m_files;

  QTimer m_calculatorDebounce;
  QTimer m_fileSearchDebounce;
  CalculatorWatcher m_calcWatcher;
  FileSearchWatcher m_fileWatcher;
  std::string m_calculatorSearchQuery;
  std::string m_fileSearchQuery;
  bool m_fileSearchEnabled = false;

  std::unique_ptr<ActionPanelState> m_actionPanel;
  int m_selectedIndex = -1;
};

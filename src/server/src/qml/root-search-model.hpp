#pragma once
#include "root-search-sources.hpp"
#include "section-list-model.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <QFutureWatcher>
#include <QTimer>
#include <string>

class AppService;
class NewsService;
class RootItemManager;
class UpdateService;

namespace config {
class Manager;
}

class RootSearchModel : public SectionListModel {
  Q_OBJECT

public:
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;
  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  explicit RootSearchModel(const ViewScope &scope, QObject *parent = nullptr);

  Q_INVOKABLE void setFilter(const QString &text);
  void setSelectedIndex(int index) override;

  const RootItem *selectedRootItem() const;

private:
  void refresh();
  bool rerunSearch();
  void startCalculator();
  void handleCalculatorFinished();
  void startFileSearch();
  void handleFileSearchFinished();

  static constexpr int MIN_FS_TEXT_LENGTH = 3;

  RootItemManager *m_manager;
  AppService *m_appDb;
  NewsService *m_newsService;
  UpdateService *m_updateService;
  CalculatorService *m_calculator;
  FileService *m_fileService;
  config::Manager *m_config;

  RootUpdateSection *m_updateSource;
  RootLinkSection *m_linkSource;
  RootCalculatorSection *m_calcSource;
  RootNewsSection *m_newsSource;
  RootFavoritesSection *m_favoritesSource;
  RootResultsSection *m_resultsSource;
  RootFilesSection *m_filesSource;
  RootFallbackSection *m_fallbackSource;

  std::string m_query;
  QString m_lastCompleterItemId;

  QTimer m_calculatorDebounce;
  QTimer m_fileSearchDebounce;
  CalculatorWatcher m_calcWatcher;
  FileSearchWatcher m_fileWatcher;
  std::string m_calculatorSearchQuery;
  std::string m_fileSearchQuery;
  bool m_fileSearchEnabled = false;
};

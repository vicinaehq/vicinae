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

namespace config {
class Manager;
}

class RootSearchModel : public SectionListModel {
  Q_OBJECT
  Q_PROPERTY(QString primaryActionTitle READ primaryActionTitle NOTIFY primaryActionChanged)
  Q_PROPERTY(QString primaryActionIcon READ primaryActionIcon NOTIFY primaryActionChanged)
  Q_PROPERTY(
      QVariantList primaryActionShortcutTokens READ primaryActionShortcutTokens NOTIFY primaryActionChanged)

signals:
  void primaryActionChanged();

public:
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;
  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  explicit RootSearchModel(const ViewScope &scope, QObject *parent = nullptr);

  Q_INVOKABLE void setFilter(const QString &text);
  void setSelectedIndex(int index) override;
  Q_INVOKABLE bool tryAliasFastTrack();

  QString primaryActionTitle() const;
  QString primaryActionIcon() const;
  QVariantList primaryActionShortcutTokens() const;

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
  CalculatorService *m_calculator;
  FileService *m_fileService;
  config::Manager *m_config;

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

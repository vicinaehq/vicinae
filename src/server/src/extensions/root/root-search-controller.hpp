#pragma once
#include "services/app-service/app-service.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/files-service/file-service.hpp"
#include <vector>
#include <string>
#include <QFutureWatcher>

class RootSearchModel;

class RootSearchController : public QObject {
  Q_OBJECT

  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;

public:
  RootSearchController(RootItemManager *manager, FileService *fs, AppService *appDb,
                       CalculatorService *calculator, RootSearchModel *model, QObject *parent = nullptr);

  void setFilter(std::string_view text);
  void setFileSearch(bool enabled);
  void reloadSearch();
  void regenerateFallback();
  void regenerateFavorites();

private slots:
  void startCalculator();
  void handleCalculatorFinished();
  void startFileSearch();
  void handleFileSearchFinished();
  void handleItemsChanged();
  void handleFallbackChanged();
  void handleFavoriteChanged();

private:
  static constexpr const int MIN_FS_TEXT_LENGTH = 3;

  RootSearchModel *m_model = nullptr;
  RootItemManager *m_manager = nullptr;
  FileService *m_fs = nullptr;
  AppService *m_appDb = nullptr;
  CalculatorService *m_calculator = nullptr;

  CalculatorWatcher m_calcWatcher;
  FileSearchWatcher m_fileWatcher;

  QTimer m_fileSearchDebounce;
  QTimer m_calculatorDebounce;

  std::string m_query;
  std::string m_fileSearchQuery;
  std::string m_calculatorSearchQuery;
  bool m_isFileSearchEnabled = false;
};

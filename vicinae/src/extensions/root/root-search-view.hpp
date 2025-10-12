#pragma once
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "ui/views/list-view.hpp"
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qtimer.h>

class RootSearchView : public ListView {
  using CalculatorWatcher = QFutureWatcher<AbstractCalculatorBackend::ComputeResult>;
  using FileSearchWatcher = QFutureWatcher<std::vector<IndexerFileResult>>;

  void handleFileResults();
  void handleFileSearchTimeout();

  void renderEmpty();
  bool inputFilter(QKeyEvent *event) override;
  void render(const QString &text);
  void textChanged(const QString &text) override;
  void handleCalculatorTimeout();
  void handleFavoriteChanged(const QString &itemId, bool value);
  void handleItemChange();
  void initialize() override;
  void handleCalculationResult();

  QTimer *m_calcDebounce = new QTimer(this);
  QTimer *m_fileSearchDebounce = new QTimer(this);
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_currentCalculatorEntry;
  std::vector<IndexerFileResult> m_fileResults;
  CalculatorWatcher m_pendingCalculation;
  FileSearchWatcher m_pendingFileSearchResults;
  QString m_lastFileSearchQuery;
  QString m_searchText;
};

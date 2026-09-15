#pragma once
#include <filesystem>
#include <QObject>
#include "services/dictation/dictation-history.hpp"
#include "services/dictation/dictation-vocabulary.hpp"

/**
 * Everything dictation persists: past dictations and the user's vocabulary.
 */
class DictationService : public QObject {
  Q_OBJECT

public:
  explicit DictationService(const std::filesystem::path &dataDir, QObject *parent = nullptr);

  DictationHistory *history() { return &m_history; }
  DictationVocabulary *vocabulary() { return &m_vocabulary; }

private:
  DictationHistory m_history;
  DictationVocabulary m_vocabulary;
};

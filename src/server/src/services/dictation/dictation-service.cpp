#include "dictation-service.hpp"

DictationService::DictationService(const std::filesystem::path &dataDir, QObject *parent)
    : QObject(parent), m_history(dataDir / "dictation-history.json", this),
      m_vocabulary(dataDir / "dictation-vocabulary.json", this) {}

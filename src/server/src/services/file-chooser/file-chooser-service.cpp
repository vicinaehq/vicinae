#include "file-chooser-service.hpp"
#include "file-chooser.hpp"

FileChooserService::FileChooserService(QObject *parent) : QObject(parent) {}

bool FileChooserService::open(const FileChooserOptions &options, QObject *context, ResultCallback onChosen) {
  if (m_activeChooser) return true;

  m_context = context;
  m_onChosen = std::move(onChosen);

  m_activeChooser = new FileChooser(this);

  if (!m_activeChooser->isAvailable()) {
    delete m_activeChooser;
    m_activeChooser = nullptr;
    emit dialogOpened();
    return false;
  }

  emit dialogOpened();

  connect(m_activeChooser, &FileChooser::filesChosen, this,
          [this](const std::vector<std::filesystem::path> &paths) { finish(&paths); });

  connect(m_activeChooser, &FileChooser::rejected, this, [this]() { finish(nullptr); });

  m_activeChooser->open(options);
  return true;
}

void FileChooserService::reportFallbackResult(const std::vector<std::filesystem::path> &paths) {
  finish(&paths);
}

void FileChooserService::reportFallbackCancelled() { finish(nullptr); }

void FileChooserService::cancel() { finish(nullptr); }

bool FileChooserService::isActive() const { return m_activeChooser != nullptr || m_onChosen != nullptr; }

void FileChooserService::finish(const std::vector<std::filesystem::path> *paths) {
  if (m_activeChooser) {
    m_activeChooser->close();
    m_activeChooser->deleteLater();
    m_activeChooser = nullptr;
  }

  if (paths && m_context && m_onChosen) { m_onChosen(*paths); }

  m_onChosen = nullptr;
  m_context = nullptr;
  emit dialogClosed();
}

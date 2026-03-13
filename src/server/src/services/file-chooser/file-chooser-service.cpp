#include "file-chooser-service.hpp"
#include "file-chooser.hpp"

FileChooserService::FileChooserService(QObject *parent) : QObject(parent) {}

bool FileChooserService::openDialog(bool canChooseFiles, bool canChooseDirectories, bool multiple) {
  if (m_activeChooser || m_fallbackActive) return true;

  FileChooserOptions opts;
  opts.canChooseFiles = canChooseFiles;
  opts.canChooseDirectories = canChooseDirectories;
  opts.allowMultipleSelection = multiple;

  m_activeChooser = new FileChooser(this);

  if (!m_activeChooser->isAvailable()) {
    delete m_activeChooser;
    m_activeChooser = nullptr;
    m_fallbackActive = true;
    emit activeChanged();
    emit dialogOpened();
    return false;
  }

  emit activeChanged();
  emit dialogOpened();

  connect(m_activeChooser, &FileChooser::filesChosen, this,
          [this](const std::vector<std::filesystem::path> &paths) {
            QStringList result;
            for (const auto &p : paths) {
              result.append(QString::fromStdString(p.string()));
            }
            finish(&result);
          });

  connect(m_activeChooser, &FileChooser::rejected, this, [this]() { finish(nullptr); });

  m_activeChooser->open(opts);
  return true;
}

void FileChooserService::notifyFallbackDone() {
  m_fallbackActive = false;
  emit activeChanged();
  emit dialogClosed();
}

void FileChooserService::cancel() { finish(nullptr); }

bool FileChooserService::isActive() const { return m_activeChooser != nullptr || m_fallbackActive; }

void FileChooserService::finish(const QStringList *paths) {
  if (m_activeChooser) {
    m_activeChooser->close();
    m_activeChooser->deleteLater();
    m_activeChooser = nullptr;
  }

  m_fallbackActive = false;

  if (paths) { emit filesSelected(*paths); }

  emit activeChanged();
  emit dialogClosed();
}

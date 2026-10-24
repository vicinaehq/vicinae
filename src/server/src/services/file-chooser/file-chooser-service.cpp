#include "file-chooser-service.hpp"
#include "service-registry.hpp"
#include <QJSEngine>
#include "file-chooser.hpp"
#include <QDir>
#include <QStandardPaths>
#include <utility>

FileChooserService::FileChooserService(QObject *parent) : QObject(parent) {}

bool FileChooserService::openDialog(bool canChooseFiles, bool canChooseDirectories, bool multiple) {
  if (m_activeChooser || m_fallbackActive) return true;

  FileChooserOptions opts;
  opts.canChooseFiles = canChooseFiles;
  opts.canChooseDirectories = canChooseDirectories;
  opts.allowMultipleSelection = multiple;

  auto *chooser = createPlatformFileChooser(this);

  if (!chooser || !chooser->isAvailable()) {
    if (chooser) chooser->deleteLater();
    m_fallbackActive = true;
    emit activeChanged();
    emit dialogOpened();
    return false;
  }

  m_activeChooser = chooser;

  emit activeChanged();
  emit dialogOpened();

  connect(m_activeChooser, &AbstractFileChooser::filesChosen, this,
          [this](const std::vector<std::filesystem::path> &paths) {
            QStringList result;
            for (const auto &p : paths) {
              result.append(QString::fromStdString(p.string()));
            }
            finish(&result);
          });

  connect(m_activeChooser, &AbstractFileChooser::rejected, this, [this]() { finish(nullptr); });

  m_activeChooser->open(opts);
  return true;
}

void FileChooserService::saveFile(const SaveOptions &options, SaveCallback callback) {
  if (isActive()) {
    callback(std::nullopt);
    return;
  }

  m_saveCallback = std::move(callback);

  auto *chooser = createPlatformFileChooser(this);

  if (!chooser || !chooser->isAvailable()) {
    if (chooser) chooser->deleteLater();
    QDir const folder(options.currentFolder
                          ? QString::fromStdString(options.currentFolder->string())
                          : QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    m_fallbackActive = true;
    emit activeChanged();
    emit dialogOpened();
    emit fallbackSaveRequested(
        QUrl::fromLocalFile(folder.filePath(QString::fromStdString(options.suggestedName))));
    return;
  }

  m_activeChooser = chooser;

  emit activeChanged();
  emit dialogOpened();

  connect(m_activeChooser, &AbstractFileChooser::filesChosen, this,
          [this](const std::vector<std::filesystem::path> &paths) {
            QStringList const result{QString::fromStdString(paths.front().string())};
            finish(&result);
          });

  connect(m_activeChooser, &AbstractFileChooser::rejected, this, [this]() { finish(nullptr); });

  FileChooserOptions const opts{.mode = FileChooserMode::Save,
                                .currentFolder = options.currentFolder,
                                .suggestedName = options.suggestedName};

  if (!m_activeChooser->open(opts)) finish(nullptr);
}

void FileChooserService::acceptFallbackSave(const QUrl &file) {
  if (!m_saveCallback) return;
  QStringList const result{toLocalPath(file)};
  finish(&result);
}

void FileChooserService::rejectFallbackSave() {
  if (m_saveCallback) finish(nullptr);
}

QString FileChooserService::toLocalPath(const QUrl &url) const {
  QString path = url.toLocalFile();
  if (path.length() > 1 && path.endsWith('/') && !path.endsWith(":/")) path.chop(1);
  return QDir::toNativeSeparators(path);
}

void FileChooserService::notifyFallbackDone() {
  m_fallbackActive = false;
  emit activeChanged();
  emit dialogClosed(true);
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

  bool resumeCaller = true;

  if (auto callback = std::exchange(m_saveCallback, nullptr)) {
    bool const accepted = paths && !paths->isEmpty();
    resumeCaller = !accepted;
    callback(accepted ? std::optional<std::filesystem::path>(paths->front().toStdString()) : std::nullopt);
  } else if (paths) {
    emit filesSelected(*paths);
  }

  emit activeChanged();
  emit dialogClosed(resumeCaller);
}

FileChooserService *FileChooserService::create(QQmlEngine *, QJSEngine *) {
  auto *service = ServiceRegistry::instance()->fileChooserService();
  QJSEngine::setObjectOwnership(service, QJSEngine::CppOwnership);
  return service;
}

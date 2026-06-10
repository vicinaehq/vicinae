#include "file-indexer.hpp"
#include <cstdint>
#include <QDateTime>
#include <QJsonObject>
#include <QTimer>
#include <filesystem>
#include <utility>
#include "rang/rang.hpp"

// FileIndexerBus

void FileIndexerBus::send(std::string_view data) {
  uint32_t size = data.size();
  m_device->write(reinterpret_cast<const char *>(&size), sizeof(size));
  m_device->write(data.data(), data.size());
}

void FileIndexerBus::readyRead() {
  while (m_device->bytesAvailable() > 0) {
    auto read = m_device->readAll();
    m_message.data.append(read);

    while (std::cmp_greater_equal(m_message.data.size(), sizeof(uint32_t))) {
      uint32_t const length = *reinterpret_cast<uint32_t *>(m_message.data.data());
      bool const isComplete = m_message.data.size() - sizeof(uint32_t) >= length;

      if (!isComplete) break;

      auto packet = m_message.data.sliced(sizeof(uint32_t), length);
      emit messageReceived(packet);
      m_message.data = m_message.data.sliced(sizeof(uint32_t) + length);
    }
  }
}

FileIndexerBus::FileIndexerBus(QIODevice *device) : m_device(device) {
  connect(device, &QIODevice::readyRead, this, &FileIndexerBus::readyRead);
}

// FileIndexer

static AbstractFileIndexer::ScanKind toScanKind(file_indexer_gen::ScanKind kind) {
  switch (kind) {
  case file_indexer_gen::ScanKind::Full:
    return AbstractFileIndexer::ScanKind::Full;
  case file_indexer_gen::ScanKind::Incremental:
    return AbstractFileIndexer::ScanKind::Incremental;
  case file_indexer_gen::ScanKind::Watcher:
    return AbstractFileIndexer::ScanKind::Watcher;
  }
  return AbstractFileIndexer::ScanKind::Full;
}

static AbstractFileIndexer::ScanState toScanState(file_indexer_gen::ScanState state) {
  switch (state) {
  case file_indexer_gen::ScanState::Started:
    return AbstractFileIndexer::ScanState::Started;
  case file_indexer_gen::ScanState::Succeeded:
    return AbstractFileIndexer::ScanState::Succeeded;
  case file_indexer_gen::ScanState::Failed:
    return AbstractFileIndexer::ScanState::Failed;
  case file_indexer_gen::ScanState::Interrupted:
    return AbstractFileIndexer::ScanState::Interrupted;
  }
  return AbstractFileIndexer::ScanState::Failed;
}

FileIndexer::FileIndexer() : m_bus(&m_process), m_rpc(m_bus), m_client(m_rpc) {
  connect(&m_process, &QProcess::readyReadStandardError, this, &FileIndexer::handleStderr);
  connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError) {
    qCritical() << "file indexer process error occured" << m_process.errorString();
  });

  connect(&m_process, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
    interruptActiveScans();

    if (status == QProcess::CrashExit || exitCode != 0) {
      qWarning() << "file indexer crashed with code" << exitCode;
      handleCrash();
    } else {
      qInfo() << "file indexer exited with code" << exitCode;
    }
  });

  connect(m_client.fileindexer(), &file_indexer_gen::FileIndexerService::scanStatusChanged, this,
          [this](const file_indexer_gen::ScanStatusEvent &event) { handleScanStatus(event); });

  connect(&m_bus, &FileIndexerBus::messageReceived, this, [this](const QByteArray &msg) {
    std::string_view view{msg.constData(), static_cast<size_t>(msg.size())};
    if (auto res = m_client.route(view); !res) {
      qWarning() << "Failed to route file indexer message" << res.error();
    }
  });
}

void FileIndexer::start() {
  if (isRunning()) return;
  m_wantRunning = true;
  startProcess();
}

void FileIndexer::stopProcess() {
  m_wantRunning = false;
  m_crashCount = 0;
  m_process.close();
}

void FileIndexer::startProcess() {
  auto path = vicinae::findHelperProgram("vicinae-file-indexer");

  if (!path) {
    qWarning() << "could not find vicinae-file-indexer helper binary, file search will not work";
    return;
  }

  m_process.setProgram(path->c_str());
  m_process.start();

  if (!m_process.waitForStarted()) {
    qCritical() << "Failed to start file indexer" << m_process.error();
    return;
  }

  m_crashCount = 0;
  sendConfigure();
}

void FileIndexer::handleScanStatus(const file_indexer_gen::ScanStatusEvent &event) {
  qDebug() << "scan status" << event.scan_id << "kind" << static_cast<int>(event.kind) << "state"
           << static_cast<int>(event.state) << "processed" << event.processed_file_count << "entrypoint"
           << event.entrypoint.c_str();
  ScanStatus const status{.scanId = event.scan_id,
                          .kind = toScanKind(event.kind),
                          .state = toScanState(event.state),
                          .entrypoint = std::filesystem::path(event.entrypoint),
                          .processedFileCount = event.processed_file_count};

  if (status.isTerminal()) {
    m_activeScans.erase(status.scanId);
  } else {
    m_activeScans[status.scanId] = status;
  }

  emit scanStatusChanged(status);
}

void FileIndexer::interruptActiveScans() {
  auto scans = std::exchange(m_activeScans, {});

  for (auto &[id, status] : scans) {
    status.state = ScanState::Interrupted;
    emit scanStatusChanged(status);
  }
}

void FileIndexer::sendConfigure() {
  if (!isRunning()) return;
  m_client.fileindexer()->configure(m_config);
}

void FileIndexer::handleCrash() {
  if (!m_wantRunning) return;

  ++m_crashCount;

  if (m_crashCount > MAX_RESTART_ATTEMPTS) {
    qCritical() << "file indexer crashed" << m_crashCount << "times, giving up on restart";
    return;
  }

  const int delay = BASE_RESTART_DELAY_MS * (1 << (m_crashCount - 1));
  qWarning() << "file indexer crashed, restarting in" << delay << "ms"
             << "(attempt" << m_crashCount << "/" << MAX_RESTART_ATTEMPTS << ")";

  QTimer::singleShot(delay, this, [this]() { startProcess(); });
}

void FileIndexer::rebuildIndex() {
  if (!isRunning()) return;
  m_client.fileindexer()->rebuildIndex();
}

void FileIndexer::preferenceValuesChanged(const QJsonObject &preferences) {
  auto splitField = [&](const char *key) {
    std::vector<std::string> out;
    const auto parts = preferences.value(key).toString().split(';', Qt::SkipEmptyParts);
    out.reserve(parts.size());
    for (const auto &part : parts) {
      out.emplace_back(part.toStdString());
    }
    return out;
  };

  m_config.paths = splitField("paths");
  m_config.excluded_paths = splitField("excludedPaths");
  m_config.watcher_paths = splitField("watcherPaths");

  if (preferences.value("autoIndexing").toBool()) {
    if (isRunning()) {
      sendConfigure();
    } else {
      start();
    }
  } else {
    stopProcess();
  }
}

QFuture<std::vector<IndexerFileResult>> FileIndexer::queryAsync(std::string_view view,
                                                                const QueryParams &params) {
  if (!isRunning()) { return QtFuture::makeReadyValueFuture(std::vector<IndexerFileResult>{}); }

  file_indexer_gen::QueryRequest req{
      .text = std::string(view),
      .pagination = {.offset = params.pagination.offset, .limit = params.pagination.limit}};

  return m_client.fileindexer()->query(req).then(
      [](std::expected<file_indexer_gen::QueryResponse, std::string> result) {
        std::vector<IndexerFileResult> results;
        if (!result) { return results; }

        results.reserve(result->matches.size());
        for (const auto &match : result->matches) {
          results.emplace_back(
              IndexerFileResult{.path = std::filesystem::path(match.path), .rank = match.rank});
        }
        return results;
      });
}

void FileIndexer::handleStderr() {
  m_stderrBuf += m_process.readAllStandardError();

  int idx = 0;
  while ((idx = m_stderrBuf.indexOf('\n')) != -1) {
    QByteArray const lineBytes = m_stderrBuf.left(idx);
    m_stderrBuf = m_stderrBuf.sliced(idx + 1);

    QString const line = QString::fromUtf8(lineBytes);
    if (line.isEmpty()) continue;

    QString level;
    QString message = line;
    if (int const tab = line.indexOf('\t'); tab != -1) {
      level = line.left(tab);
      message = line.sliced(tab + 1);
    }

    rang::fg color = rang::fg::reset;
    if (level == "debug") {
      color = rang::fg::cyan;
    } else if (level == "info") {
      color = rang::fg::green;
    } else if (level == "warn") {
      color = rang::fg::yellow;
    } else if (level == "error") {
      color = rang::fg::red;
    }

    auto const ts = QDateTime::currentDateTime().toString("yyyy-MM-dd'T'hh:mm:ss");
    std::cerr << "[" << rang::fg::blue << "F" << rang::fg::reset << "] " << rang::fg::gray << ts.toStdString()
              << " " << color << level.toStdString() << rang::fg::reset << (level.isEmpty() ? "" : " - ")
              << message.toStdString() << "\n";
  }
}

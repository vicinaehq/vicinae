#include "database-key.hpp"
#include "db-encryption.hpp"
#include "vicinae.hpp"
#include <QByteArray>
#include <QEventLoop>
#include <QRandomGenerator>
#include <array>
#include <cstring>
#include <expected>
#include <qt6keychain/keychain.h>

namespace {

constexpr auto KEY_NAME = "vicinae-db-key";

// The key is needed synchronously before any DB opens, so drive the async job to completion.
template <typename Job> void runJob(Job &job) {
  QEventLoop loop;
  QObject::connect(&job, &QKeychain::Job::finished, &loop, &QEventLoop::quit);
  job.start();
  loop.exec();
}

std::expected<std::optional<db::EncryptionKey>, QString> readKey() {
  QKeychain::ReadPasswordJob job(Omnicast::APP_ID);
  job.setAutoDelete(false);
  job.setKey(KEY_NAME);
  runJob(job);

  if (job.error() == QKeychain::EntryNotFound) return std::optional<db::EncryptionKey>{};
  if (job.error() != QKeychain::NoError) return std::unexpected(job.errorString());

  const QByteArray data = job.binaryData();
  if (data.size() != static_cast<int>(db::KEY_SIZE)) return std::unexpected("stored key has unexpected size");

  db::EncryptionKey key;
  std::memcpy(key.data(), data.constData(), db::KEY_SIZE);
  return std::optional{key};
}

std::expected<db::EncryptionKey, QString> createKey() {
  std::array<quint32, db::KEY_SIZE / sizeof(quint32)> words;
  QRandomGenerator::system()->fillRange(words.data(), words.size());

  db::EncryptionKey key;
  std::memcpy(key.data(), words.data(), db::KEY_SIZE);

  QKeychain::WritePasswordJob job(Omnicast::APP_ID);
  job.setAutoDelete(false);
  job.setKey(KEY_NAME);
  job.setBinaryData(QByteArray(reinterpret_cast<const char *>(key.data()), db::KEY_SIZE));
  runJob(job);

  if (job.error() != QKeychain::NoError) return std::unexpected(job.errorString());
  return key;
}

} // namespace

namespace db {

std::optional<EncryptionKey>
prepareDatabaseEncryption(bool enabled, std::initializer_list<std::filesystem::path> databases) {
  bool anyEncrypted = false;
  for (const auto &path : databases)
    if (detectCipherState(path) == CipherState::Encrypted) anyEncrypted = true;

  // Avoid the keychain entirely when off and nothing is encrypted, so the default path
  // works on systems without a keychain daemon.
  if (!enabled && !anyEncrypted) return std::nullopt;

  auto stored = readKey();
  if (!stored) qFatal("Database keychain unavailable: %s", qPrintable(stored.error()));

  std::optional<EncryptionKey> key = *stored;

  if (enabled && !key && !anyEncrypted) {
    auto created = createKey();
    if (!created) qFatal("Failed to store database key in keychain: %s", qPrintable(created.error()));
    key = *created;
  }
  if (!key)
    qFatal("A database is encrypted but its key is missing from the keychain. If your keyring is locked "
           "or its daemon is not running, unlock/start it and retry; otherwise the affected database "
           "files must be deleted to reset.");

  for (const auto &path : databases) {
    auto state = detectCipherState(path);
    if (!state) continue;
    if ((*state == CipherState::Encrypted) == enabled) continue;
    if (auto migrated = ensureCipherState(path, enabled, *key); !migrated)
      qFatal("Failed to migrate database '%s': %s", path.string().c_str(), migrated.error().c_str());
  }

  return enabled ? key : std::nullopt;
}

} // namespace db

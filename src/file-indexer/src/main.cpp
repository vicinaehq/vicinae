#include <cstdint>
#include <iostream>
#include <mutex>
#include <sqlcipher/sqlite3.h>
#include <unistd.h>
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/indexer-service.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/migrations.hpp"
#include "file-indexer/util.hpp"

// Statically-linked fuzzy_trigram FTS5 tokenizer (vendor/fuzzy-trigram).
extern "C" int vicinaeFuzzyTrigramInit(sqlite3 *, char **, const void *);

// Statically-linked spellfix1 virtual table (vendor/spellfix).
extern "C" int vicinaeSpellfixInit(sqlite3 *, char **, const void *);

// Locked so concurrent query replies don't interleave mid-frame.
class StdoutTransport : public file_indexer_gen::AbstractTransport {
  std::mutex m_mtx;

  void send(std::string_view data) override {
    std::scoped_lock const lock(m_mtx);
    uint32_t size = data.size();
    std::cout.write(reinterpret_cast<const char *>(&size), sizeof(size));
    std::cout.write(data.data(), data.size());
    std::cout.flush();
  }
};

int main(int, char **) {
  // Register the fuzzy_trigram tokenizer and spellfix1 on every connection opened from here on.
  sqlite3_auto_extension(reinterpret_cast<void (*)()>(vicinaeFuzzyTrigramInit));
  sqlite3_auto_extension(reinterpret_cast<void (*)()>(vicinaeSpellfixInit));

  // TODO: remove this at some point
  file_indexer::removeLegacyDbFiles();

  // We made a breaking change that requires fully rebuilding the index
  // Instead of methodically updating the DB, we just remove it entirely
  // and start over fresh. Much simpler :D
  // We use temporary database connections to get the user version through `PRAGMA user_version`
  if (auto userVersion = FileIndexerDatabase{}.userVersion(); userVersion != file_indexer::SCHEMA_VERSION) {
    flog::info() << "Breaking change detected (current rev=" << file_indexer::SCHEMA_VERSION
                 << ", user version=" << userVersion << "), starting over... \n";
    file_indexer::purgeDbFiles();
    FileIndexerDatabase{}.setUserVersion(file_indexer::SCHEMA_VERSION);
  }

  StdoutTransport transport;
  file_indexer_gen::RpcTransport rpc{transport};
  file_indexer::IndexerService service{rpc};
  file_indexer_gen::Server server{rpc, service};

  service.listen(server);
}

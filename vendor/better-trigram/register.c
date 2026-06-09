/*
 * Static-linkage shim for the better_trigram FTS5 tokenizer.
 *
 * Compiling with SQLITE_CORE makes better-trigram.c expose the internal
 * `sqlite3Fts5BetterTrigramInit` (which is `static`), so we wrap it in a single
 * exported entry point usable with sqlite3_auto_extension().
 */
#define SQLITE_CORE 1
#include "better-trigram.c"

/* Matches the sqlite3_auto_extension entry signature. The error-message and
 * api-routines arguments are unused because the tokenizer is statically linked. */
int vicinaeBetterTrigramInit(sqlite3 *db, char **pzErrMsg, const void *pApiUnused) {
  (void)pzErrMsg;
  (void)pApiUnused;
  return sqlite3Fts5BetterTrigramInit(db);
}

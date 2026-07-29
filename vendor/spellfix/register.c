/*
 * Static-linkage shim for the spellfix1 virtual table (sqlite ext/misc/spellfix.c,
 * pinned to the vendored sqlite version).
 *
 * Compiling with SQLITE_CORE makes sqlite3ext.h drop the api-routines
 * indirection so spellfix.c links directly against the statically-linked
 * sqlcipher core; sqlite3_spellfix_init's SQLITE_EXTENSION_INIT2 becomes a
 * no-op and the api argument is ignored.
 */
#define SQLITE_CORE 1
#include "spellfix.c"

/* Matches the sqlite3_auto_extension entry signature. */
int vicinaeSpellfixInit(sqlite3 *db, char **pzErrMsg, const void *pApiUnused) {
  (void)pApiUnused;
  return sqlite3_spellfix_init(db, pzErrMsg, 0);
}

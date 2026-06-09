#ifndef SQLITE_BETTER_TRIGRAM_H
#define SQLITE_BETTER_TRIGRAM_H

#ifndef SQLITE_CORE
#include "sqlite3ext.h"
#else
#include "sqlite3.h"
#endif

#ifndef SQLITE_PRIVATE
#define SQLITE_PRIVATE static
#endif

#ifdef SQLITE_BETTER_TRIGRAM_STATIC
#define SQLITE_BETTER_TRIGRAM_API
#else
#ifdef _WIN32
#define SQLITE_BETTER_TRIGRAM_API __declspec(dllexport)
#else
#define SQLITE_BETTER_TRIGRAM_API
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SQLITE_CORE
SQLITE_PRIVATE int sqlite3Fts5BetterTrigramInit(sqlite3 *db);
#else
SQLITE_BETTER_TRIGRAM_API int
sqlite3_bettertrigram_init(sqlite3 *db, char **pzErrMsg,
                           const sqlite3_api_routines *pApi);

#endif

#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif /* ifndef SQLITE_BETTER_TRIGRAM_H */
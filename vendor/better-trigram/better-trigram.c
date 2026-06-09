/*
** 2024-10-21
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*/

#include "better-trigram.h"
#include "tokenizer.c"
#include <stdio.h>
#include <string.h>

#ifndef SQLITE_CORE
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#else
#include "sqlite3.h"
#endif

#define UNUSED_PARAM(x) (void)(x)

#ifndef SQLITE_PRIVATE
#define SQLITE_PRIVATE static
#endif

static fts5_api *fts5_api_from_db(sqlite3 *db) {
  fts5_api *pRet = 0;
  sqlite3_stmt *pStmt = 0;

  int version = sqlite3_libversion_number();
  if (version >= 3020000) { // current api
    if (SQLITE_OK == sqlite3_prepare(db, "SELECT fts5(?1)", -1, &pStmt, 0)) {
      sqlite3_bind_pointer(pStmt, 1, (void *)&pRet, "fts5_api_ptr", NULL);
      sqlite3_step(pStmt);
    }
    sqlite3_finalize(pStmt);
  } else { // before 3.20
    int rc = sqlite3_prepare(db, "SELECT fts5()", -1, &pStmt, 0);
    if (rc == SQLITE_OK) {
      if (SQLITE_ROW == sqlite3_step(pStmt) &&
          sizeof(fts5_api *) == sqlite3_column_bytes(pStmt, 0)) {
        memcpy(&pRet, sqlite3_column_blob(pStmt, 0), sizeof(fts5_api *));
      }
      sqlite3_finalize(pStmt);
    }
  }
  return pRet;
}

/* Tokenizer structure */
typedef struct {
  int bFold;      /* Fold case if 1 */
  int iFoldParam; /* Diacritic removal parameter */
} BetterTrigramTokenizer;

/* Free the tokenizer */
static void fts5BetterTrigramDelete(Fts5Tokenizer *p) { sqlite3_free(p); }

/* Create the tokenizer */
static int fts5BetterTrigramCreate(void *pUnused, const char **azArg, int nArg,
                                   Fts5Tokenizer **ppOut) {
  BetterTrigramTokenizer *p =
      (BetterTrigramTokenizer *)sqlite3_malloc(sizeof(BetterTrigramTokenizer));
  UNUSED_PARAM(pUnused);
  if (!p)
    return SQLITE_NOMEM;

  int rc = SQLITE_OK;
  p->bFold = 1;      /* Default case folding enabled */
  p->iFoldParam = 0; /* Default diacritic removal disabled */

  /* Parse options */
  for (int i = 0; rc == SQLITE_OK && i < nArg; i += 2) {
    const char *zArg = azArg[i + 1];
    if (0 == sqlite3_stricmp(azArg[i], "case_sensitive")) {
      if ((zArg[0] != '0' && zArg[0] != '1') || zArg[1]) {
        rc = SQLITE_ERROR;
      } else {
        p->bFold = (zArg[0] == '0');
      }
    } else if (0 == sqlite3_stricmp(azArg[i], "remove_diacritics")) {
      if ((zArg[0] != '0' && zArg[0] != '1' && zArg[0] != '2') || zArg[1]) {
        rc = SQLITE_ERROR;
      } else {
        p->iFoldParam = (zArg[0] != '0') ? 2 : 0;
      }
    } else {
      rc = SQLITE_ERROR;
    }

    if (p->iFoldParam != 0 && p->bFold == 0) {
      rc = SQLITE_ERROR;
    }

    if (rc != SQLITE_OK) {
      fts5BetterTrigramDelete((Fts5Tokenizer *)p);
      p = 0;
    }
  }

  *ppOut = (Fts5Tokenizer *)p;
  return rc;
}

static int fts5BetterTrigramTokenize(Fts5Tokenizer *pTokenizer, void *pCtx,
                                     int flags, const char *pText, int nText,
                                     int (*xToken)(void *, int, const char *,
                                                   int, int, int)) {
  UNUSED_PARAM(flags);
  BetterTrigramTokenizer *p = (BetterTrigramTokenizer *)pTokenizer;
  tokenize(pText, nText, p->bFold, p->iFoldParam, pCtx, xToken);
  return SQLITE_OK;
}

static int fts5BetterTrigramInit(sqlite3 *db) {
  fts5_api *ftsApi;

  fts5_tokenizer tokenizer = {fts5BetterTrigramCreate, fts5BetterTrigramDelete,
                              fts5BetterTrigramTokenize};

  ftsApi = fts5_api_from_db(db);

  if (ftsApi) {
    ftsApi->xCreateTokenizer(ftsApi, "better_trigram", (void *)ftsApi,
                             &tokenizer, NULL);
    return SQLITE_OK;
  } else {
    // *error = sqlite3_mprintf("Can't find fts5 extension");
    return SQLITE_ERROR;
  }
}

#ifdef SQLITE_CORE
SQLITE_PRIVATE int sqlite3Fts5BetterTrigramInit(sqlite3 *db) {
  return fts5BetterTrigramInit(db);
}
#else
SQLITE_BETTER_TRIGRAM_API int
sqlite3_bettertrigram_init(sqlite3 *db, char **error,
                           const sqlite3_api_routines *api) {
  SQLITE_EXTENSION_INIT2(api);
  UNUSED_PARAM(error);

  return fts5BetterTrigramInit(db);
}
#endif

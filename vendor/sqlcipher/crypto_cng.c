/*
** SQLCipher crypto provider backed by Windows CNG (bcrypt.dll).
** Registered at build time through SQLCIPHER_CRYPTO_CUSTOM=sqlcipher_cng_setup.
*/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>
#include "sqlite3.h"

#define SQLCIPHER_DECRYPT 0
#define SQLCIPHER_ENCRYPT 1

#define SQLCIPHER_HMAC_SHA1 0
#define SQLCIPHER_HMAC_SHA256 1
#define SQLCIPHER_HMAC_SHA512 2

#define SQLCIPHER_LOG_ERROR (1 << 0)
#define SQLCIPHER_LOG_PROVIDER (1 << 3)

/* Mirrors sqlcipher_provider from the vendored sqlite3.c (SQLCipher 4.16.0). Keep in sync on bumps. */
typedef struct sqlcipher_provider sqlcipher_provider;
struct sqlcipher_provider {
  int (*init)(void);
  void (*shutdown)(void);
  const char *(*get_provider_name)(void *ctx);
  int (*add_random)(void *ctx, const void *buffer, int length);
  int (*random)(void *ctx, void *buffer, int length);
  int (*hmac)(void *ctx, int algorithm, const unsigned char *hmac_key, int key_sz, const unsigned char *in,
              int in_sz, const unsigned char *in2, int in2_sz, unsigned char *out);
  int (*kdf)(void *ctx, int algorithm, const unsigned char *pass, int pass_sz, const unsigned char *salt,
             int salt_sz, int workfactor, int key_sz, unsigned char *key);
  int (*cipher)(void *ctx, int mode, const unsigned char *key, int key_sz, const unsigned char *iv,
                const unsigned char *in, int in_sz, unsigned char *out);
  const char *(*get_cipher)(void *ctx);
  int (*get_key_sz)(void *ctx);
  int (*get_iv_sz)(void *ctx);
  int (*get_block_sz)(void *ctx);
  int (*get_hmac_sz)(void *ctx, int algorithm);
  int (*ctx_init)(void **ctx);
  int (*ctx_free)(void **ctx);
  int (*fips_status)(void *ctx);
  const char *(*get_provider_version)(void *ctx);
  sqlcipher_provider *next;
};

void sqlcipher_log(unsigned int level, unsigned int source, const char *message, ...);

#define AES_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

static BCRYPT_ALG_HANDLE g_aes = NULL;
static BCRYPT_ALG_HANDLE g_hmac_sha1 = NULL;
static BCRYPT_ALG_HANDLE g_hmac_sha256 = NULL;
static BCRYPT_ALG_HANDLE g_hmac_sha512 = NULL;
static char g_version[32] = "unknown";

static void cng_read_version(void) {
  wchar_t path[MAX_PATH];
  HMODULE mod = GetModuleHandleW(L"bcrypt.dll");
  if (!mod || !GetModuleFileNameW(mod, path, MAX_PATH)) return;

  DWORD size = GetFileVersionInfoSizeW(path, NULL);
  if (!size) return;

  void *data = HeapAlloc(GetProcessHeap(), 0, size);
  if (!data) return;

  VS_FIXEDFILEINFO *ffi = NULL;
  UINT len = 0;
  if (GetFileVersionInfoW(path, 0, size, data) && VerQueryValueW(data, L"\\", (LPVOID *)&ffi, &len) &&
      len >= sizeof(*ffi)) {
    snprintf(g_version, sizeof(g_version), "%u.%u.%u.%u", HIWORD(ffi->dwFileVersionMS),
             LOWORD(ffi->dwFileVersionMS), HIWORD(ffi->dwFileVersionLS), LOWORD(ffi->dwFileVersionLS));
  }
  HeapFree(GetProcessHeap(), 0, data);
}

static void cng_close(BCRYPT_ALG_HANDLE *alg) {
  if (*alg) {
    BCryptCloseAlgorithmProvider(*alg, 0);
    *alg = NULL;
  }
}

static void cng_shutdown(void) {
  cng_close(&g_aes);
  cng_close(&g_hmac_sha1);
  cng_close(&g_hmac_sha256);
  cng_close(&g_hmac_sha512);
}

static int cng_open(BCRYPT_ALG_HANDLE *alg, LPCWSTR id, ULONG flags) {
  NTSTATUS status = BCryptOpenAlgorithmProvider(alg, id, NULL, flags);
  if (!BCRYPT_SUCCESS(status)) {
    sqlcipher_log(SQLCIPHER_LOG_ERROR, SQLCIPHER_LOG_PROVIDER,
                  "sqlcipher_cng_init: BCryptOpenAlgorithmProvider(%ls) failed 0x%08x", id, (unsigned)status);
    return SQLITE_ERROR;
  }
  return SQLITE_OK;
}

static int cng_init(void) {
  if (cng_open(&g_aes, BCRYPT_AES_ALGORITHM, 0) != SQLITE_OK) goto error;

  NTSTATUS status = BCryptSetProperty(g_aes, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
                                      sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
  if (!BCRYPT_SUCCESS(status)) {
    sqlcipher_log(SQLCIPHER_LOG_ERROR, SQLCIPHER_LOG_PROVIDER,
                  "sqlcipher_cng_init: BCryptSetProperty(CHAINING_MODE) failed 0x%08x", (unsigned)status);
    goto error;
  }

  if (cng_open(&g_hmac_sha1, BCRYPT_SHA1_ALGORITHM, BCRYPT_ALG_HANDLE_HMAC_FLAG) != SQLITE_OK) goto error;
  if (cng_open(&g_hmac_sha256, BCRYPT_SHA256_ALGORITHM, BCRYPT_ALG_HANDLE_HMAC_FLAG) != SQLITE_OK) goto error;
  if (cng_open(&g_hmac_sha512, BCRYPT_SHA512_ALGORITHM, BCRYPT_ALG_HANDLE_HMAC_FLAG) != SQLITE_OK) goto error;

  cng_read_version();
  return SQLITE_OK;

error:
  cng_shutdown();
  return SQLITE_ERROR;
}

static BCRYPT_ALG_HANDLE cng_hmac_alg(int algorithm) {
  switch (algorithm) {
  case SQLCIPHER_HMAC_SHA1:
    return g_hmac_sha1;
  case SQLCIPHER_HMAC_SHA256:
    return g_hmac_sha256;
  case SQLCIPHER_HMAC_SHA512:
    return g_hmac_sha512;
  default:
    return NULL;
  }
}

static int cng_get_hmac_sz(void *ctx, int algorithm) {
  switch (algorithm) {
  case SQLCIPHER_HMAC_SHA1:
    return 20;
  case SQLCIPHER_HMAC_SHA256:
    return 32;
  case SQLCIPHER_HMAC_SHA512:
    return 64;
  default:
    return 0;
  }
}

static int cng_add_random(void *ctx, const void *buffer, int length) { return SQLITE_OK; }

static int cng_random(void *ctx, void *buffer, int length) {
  NTSTATUS status = BCryptGenRandom(NULL, (PUCHAR)buffer, (ULONG)length, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  return BCRYPT_SUCCESS(status) ? SQLITE_OK : SQLITE_ERROR;
}

static int cng_hmac(void *ctx, int algorithm, const unsigned char *hmac_key, int key_sz,
                    const unsigned char *in, int in_sz, const unsigned char *in2, int in2_sz,
                    unsigned char *out) {
  BCRYPT_ALG_HANDLE alg = cng_hmac_alg(algorithm);
  int out_sz = cng_get_hmac_sz(ctx, algorithm);
  if (!alg || in == NULL) return SQLITE_ERROR;

  BCRYPT_HASH_HANDLE hash = NULL;
  int rc = SQLITE_ERROR;

  if (!BCRYPT_SUCCESS(BCryptCreateHash(alg, &hash, NULL, 0, (PUCHAR)hmac_key, (ULONG)key_sz, 0))) goto done;
  if (!BCRYPT_SUCCESS(BCryptHashData(hash, (PUCHAR)in, (ULONG)in_sz, 0))) goto done;
  if (in2 != NULL && !BCRYPT_SUCCESS(BCryptHashData(hash, (PUCHAR)in2, (ULONG)in2_sz, 0))) goto done;
  if (!BCRYPT_SUCCESS(BCryptFinishHash(hash, out, (ULONG)out_sz, 0))) goto done;
  rc = SQLITE_OK;

done:
  if (hash) BCryptDestroyHash(hash);
  return rc;
}

static int cng_kdf(void *ctx, int algorithm, const unsigned char *pass, int pass_sz,
                   const unsigned char *salt, int salt_sz, int workfactor, int key_sz, unsigned char *key) {
  BCRYPT_ALG_HANDLE alg = cng_hmac_alg(algorithm);
  if (!alg) return SQLITE_ERROR;

  NTSTATUS status = BCryptDeriveKeyPBKDF2(alg, (PUCHAR)pass, (ULONG)pass_sz, (PUCHAR)salt, (ULONG)salt_sz,
                                          (ULONGLONG)workfactor, key, (ULONG)key_sz, 0);
  if (!BCRYPT_SUCCESS(status)) {
    sqlcipher_log(SQLCIPHER_LOG_ERROR, SQLCIPHER_LOG_PROVIDER,
                  "sqlcipher_cng_kdf: BCryptDeriveKeyPBKDF2 failed 0x%08x", (unsigned)status);
    return SQLITE_ERROR;
  }
  return SQLITE_OK;
}

static int cng_cipher(void *ctx, int mode, const unsigned char *key, int key_sz, const unsigned char *iv,
                      const unsigned char *in, int in_sz, unsigned char *out) {
  if (!g_aes || key_sz != AES_KEY_SIZE) return SQLITE_ERROR;

  BCRYPT_KEY_HANDLE hkey = NULL;
  int rc = SQLITE_ERROR;
  /* CNG advances the IV in place, SQLCipher expects its copy untouched */
  UCHAR iv_copy[AES_BLOCK_SIZE];
  ULONG written = 0;
  NTSTATUS status;

  memcpy(iv_copy, iv, AES_BLOCK_SIZE);

  if (!BCRYPT_SUCCESS(BCryptGenerateSymmetricKey(g_aes, &hkey, NULL, 0, (PUCHAR)key, (ULONG)key_sz, 0)))
    goto done;

  if (mode == SQLCIPHER_ENCRYPT) {
    status = BCryptEncrypt(hkey, (PUCHAR)in, (ULONG)in_sz, NULL, iv_copy, AES_BLOCK_SIZE, out, (ULONG)in_sz,
                           &written, 0);
  } else {
    status = BCryptDecrypt(hkey, (PUCHAR)in, (ULONG)in_sz, NULL, iv_copy, AES_BLOCK_SIZE, out, (ULONG)in_sz,
                           &written, 0);
  }

  if (!BCRYPT_SUCCESS(status)) {
    sqlcipher_log(SQLCIPHER_LOG_ERROR, SQLCIPHER_LOG_PROVIDER, "sqlcipher_cng_cipher: %s failed 0x%08x",
                  mode == SQLCIPHER_ENCRYPT ? "BCryptEncrypt" : "BCryptDecrypt", (unsigned)status);
    goto done;
  }
  if (written != (ULONG)in_sz) goto done;
  rc = SQLITE_OK;

done:
  if (hkey) BCryptDestroyKey(hkey);
  return rc;
}

static const char *cng_get_provider_name(void *ctx) { return "cng"; }
static const char *cng_get_provider_version(void *ctx) { return g_version; }
static const char *cng_get_cipher(void *ctx) { return "aes-256-cbc"; }
static int cng_get_key_sz(void *ctx) { return AES_KEY_SIZE; }
static int cng_get_iv_sz(void *ctx) { return AES_BLOCK_SIZE; }
static int cng_get_block_sz(void *ctx) { return AES_BLOCK_SIZE; }
static int cng_ctx_init(void **ctx) { return SQLITE_OK; }
static int cng_ctx_free(void **ctx) { return SQLITE_OK; }
static int cng_fips_status(void *ctx) { return 0; }

int sqlcipher_cng_setup(sqlcipher_provider *p) {
  p->init = cng_init;
  p->shutdown = cng_shutdown;
  p->get_provider_name = cng_get_provider_name;
  p->add_random = cng_add_random;
  p->random = cng_random;
  p->hmac = cng_hmac;
  p->kdf = cng_kdf;
  p->cipher = cng_cipher;
  p->get_cipher = cng_get_cipher;
  p->get_key_sz = cng_get_key_sz;
  p->get_iv_sz = cng_get_iv_sz;
  p->get_block_sz = cng_get_block_sz;
  p->get_hmac_sz = cng_get_hmac_sz;
  p->ctx_init = cng_ctx_init;
  p->ctx_free = cng_ctx_free;
  p->fips_status = cng_fips_status;
  p->get_provider_version = cng_get_provider_version;
  return SQLITE_OK;
}

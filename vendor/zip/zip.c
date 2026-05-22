/*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#define __STDC_WANT_LIB_EXT1__ 1

#include <errno.h>
#include <sys/stat.h>
#include <time.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER) || defined(__MINGW32__)
/* Win32, DOS, MSVC, MSVS */
#include <direct.h>

#define HAS_DEVICE(P)                                                                                        \
  ((((P)[0] >= 'A' && (P)[0] <= 'Z') || ((P)[0] >= 'a' && (P)[0] <= 'z')) && (P)[1] == ':')
#define FILESYSTEM_PREFIX_LEN(P) (HAS_DEVICE(P) ? 2 : 0)

#else

#if ZIP_HAVE_SYMLINK
#include <unistd.h> // needed for symlink()
#endif

#endif

#ifdef __MINGW32__
#include <sys/types.h>
#include <unistd.h>
#endif

#include "miniz.h"
#include "zip.h"

#ifdef _MSC_VER
#include <io.h>

#define ftruncate(fd, sz) (-(_chsize_s((fd), (sz)) != 0))
#define fileno _fileno
#endif

#if defined(__TINYC__) && (defined(_WIN32) || defined(_WIN64))
#include <io.h>

#define ftruncate(fd, sz) (-(_chsize_s((fd), (sz)) != 0))
#define fileno _fileno
#endif

#ifndef HAS_DEVICE
#define HAS_DEVICE(P) 0
#endif

#ifndef FILESYSTEM_PREFIX_LEN
#define FILESYSTEM_PREFIX_LEN(P) 0
#endif

#ifndef ISSLASH
#define ISSLASH(C) ((C) == '/' || (C) == '\\')
#endif

#define CLEANUP(ptr)                                                                                         \
  do {                                                                                                       \
    if (ptr) {                                                                                               \
      free((void *)ptr);                                                                                     \
      ptr = NULL;                                                                                            \
    }                                                                                                        \
  } while (0)

#define UNX_IFDIR 0040000  /* Unix directory */
#define UNX_IFREG 0100000  /* Unix regular file */
#define UNX_IFSOCK 0140000 /* Unix socket (BSD, not SysV or Amiga) */
#define UNX_IFLNK 0120000  /* Unix symbolic link (not SysV, Amiga) */
#define UNX_IFBLK 0060000  /* Unix block special       (not Amiga) */
#define UNX_IFCHR 0020000  /* Unix character special   (not Amiga) */
#define UNX_IFIFO 0010000  /* Unix fifo    (BCC, not MSC or Amiga) */

#if ZIP_ENABLE_DEFLATE
/*
 * Write function for in-memory delete mode. Behaves identically to
 * mz_zip_heap_write_func but has a distinct address so
 * mz_zip_writer_end_internal won't free m_pMem (it only frees when
 * m_pWrite == mz_zip_heap_write_func). This lets the caller retain
 * ownership of the buffer passed to zip_stream_open.
 */
static size_t zip_stream_delete_write_func(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n) {
  mz_zip_archive *pZip = (mz_zip_archive *)pOpaque;
  mz_zip_internal_state *pState = pZip->m_pState;
  mz_uint64 new_size = MZ_MAX(file_ofs + n, pState->m_mem_size);

  if (!n) return 0;

  if ((sizeof(size_t) == sizeof(mz_uint32)) && (new_size > 0x7FFFFFFF)) {
    mz_zip_set_error(pZip, MZ_ZIP_FILE_TOO_LARGE);
    return 0;
  }

  if (new_size > pState->m_mem_capacity) {
    void *pNew_block;
    size_t new_capacity = MZ_MAX(64, pState->m_mem_capacity);
    while (new_capacity < new_size)
      new_capacity *= 2;
    if (NULL == (pNew_block = pZip->m_pRealloc(pZip->m_pAlloc_opaque, pState->m_pMem, 1, new_capacity))) {
      mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
      return 0;
    }
    pState->m_pMem = pNew_block;
    pState->m_mem_capacity = new_capacity;
  }
  memcpy((mz_uint8 *)pState->m_pMem + file_ofs, pBuf, n);
  pState->m_mem_size = (size_t)new_size;
  return n;
}
#endif /* ZIP_ENABLE_DEFLATE */

/* Traditional PKWARE Encryption (APPNOTE 6.1) encryption header size. */
#define ZIP_PKWARE_ENCRYPT_HEADER_SIZE 12

/*
 * Internal state for the PKWARE Traditional Encryption cipher.
 * Three 32-bit keys are maintained and updated for each byte processed.
 * See APPNOTE.TXT section "VI. Traditional PKWARE Encryption".
 */
struct zip_pkware_keys_t {
  mz_uint32 key0;
  mz_uint32 key1;
  mz_uint32 key2;
};

#if ZIP_ENABLE_INFLATE || ZIP_ENABLE_DEFLATE

/* Resets PKWARE cipher keys to their initial values (0x12345678, etc.). */
static void zip_pkware_keys_init(struct zip_pkware_keys_t *keys) {
  keys->key0 = 305419896;
  keys->key1 = 591751049;
  keys->key2 = 878082192;
}

/*
 * Computes a single-byte "raw" CRC32 update without the initial/final
 * complement that mz_crc32 applies.  PKWARE encryption requires this
 * uncomplemented variant for key derivation.
 */
static mz_uint32 zip_crc32_raw(mz_uint32 crc, mz_uint8 c) { return ~(mz_uint32)mz_crc32(~crc, &c, 1); }

/* Advances the three PKWARE cipher keys by one plaintext byte. */
static void zip_pkware_keys_update(struct zip_pkware_keys_t *keys, mz_uint8 c) {
  keys->key0 = zip_crc32_raw(keys->key0, c);
  keys->key1 = keys->key1 + (keys->key0 & 0xFF);
  keys->key1 = keys->key1 * 134775813 + 1;
  keys->key2 = zip_crc32_raw(keys->key2, (mz_uint8)(keys->key1 >> 24));
}

/* Derives the next keystream byte from the current cipher state. */
static mz_uint8 zip_pkware_decrypt_byte(const struct zip_pkware_keys_t *keys) {
  mz_uint16 temp = (mz_uint16)(keys->key2 | 2);
  return (mz_uint8)((temp * (temp ^ 1)) >> 8);
}

/* Initializes cipher keys and derives the key schedule from a password. */
static void zip_pkware_keys_init_password(struct zip_pkware_keys_t *keys, const char *password) {
  zip_pkware_keys_init(keys);
  if (password) {
    while (*password) {
      zip_pkware_keys_update(keys, (mz_uint8)*password);
      password++;
    }
  }
}

#if ZIP_ENABLE_DEFLATE
/* Encrypts a single plaintext byte and advances the cipher state. */
static mz_uint8 zip_pkware_encrypt_byte(struct zip_pkware_keys_t *keys, mz_uint8 c) {
  mz_uint8 k = zip_pkware_decrypt_byte(keys);
  mz_uint8 enc = (mz_uint8)(c ^ k);
  zip_pkware_keys_update(keys, c);
  return enc;
}
#endif /* ZIP_ENABLE_DEFLATE */

/* Decrypts a single ciphertext byte and advances the cipher state. */
static mz_uint8 zip_pkware_decrypt(struct zip_pkware_keys_t *keys, mz_uint8 c) {
  mz_uint8 k = zip_pkware_decrypt_byte(keys);
  mz_uint8 dec = (mz_uint8)(c ^ k);
  zip_pkware_keys_update(keys, dec);
  return dec;
}

#endif /* ZIP_ENABLE_INFLATE || ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_DEFLATE
/*
 * Callback state passed to tdefl for encrypting compressed output on the fly.
 * Wraps the normal mz_zip_writer_add_state with a reference to the active
 * PKWARE cipher keys.
 */
struct zip_encrypt_put_buf_state_t {
  mz_zip_writer_add_state *inner_state;
  struct zip_pkware_keys_t *keys;
};
#endif

struct zip_entry_t {
  ssize_t index;
  char *name;
  mz_uint64 uncomp_size;
  mz_uint64 comp_size;
  mz_uint32 uncomp_crc32;
  mz_uint64 dir_offset;
  mz_uint8 header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
  mz_uint64 header_offset;
  mz_uint16 method;
#if ZIP_ENABLE_DEFLATE
  mz_zip_writer_add_state state;
  tdefl_compressor comp;
#endif
  mz_uint32 external_attr;
  time_t m_time;
  struct zip_pkware_keys_t enc_keys;
#if ZIP_ENABLE_DEFLATE
  struct zip_encrypt_put_buf_state_t enc_state;
  mz_uint64 enc_header_ofs;
#endif
};

struct zip_t {
  mz_zip_archive archive;
  mz_uint level;
  struct zip_entry_t entry;
  char *password;
};

enum zip_modify_t {
  MZ_KEEP = 0,
  MZ_DELETE = 1,
  MZ_MOVE = 2,
};

struct zip_entry_mark_t {
  ssize_t file_index;
  enum zip_modify_t type;
  mz_uint64 m_local_header_ofs;
  size_t lf_length;
};

#if ZIP_ENABLE_DEFLATE
/*
 * tdefl output callback that encrypts each compressed chunk before writing
 * it to the archive.  Used in place of mz_zip_writer_add_put_buf_callback
 * when a password is set.
 */
static mz_bool zip_encrypt_put_buf_callback(const void *pBuf, int len, void *pUser) {
  struct zip_encrypt_put_buf_state_t *es = (struct zip_encrypt_put_buf_state_t *)pUser;
  mz_zip_writer_add_state *pState = es->inner_state;
  mz_uint8 *enc_buf;
  int i;

  if (len <= 0) return MZ_TRUE;

  enc_buf = (mz_uint8 *)malloc((size_t)len);
  if (!enc_buf) return MZ_FALSE;

  for (i = 0; i < len; i++) {
    enc_buf[i] = zip_pkware_encrypt_byte(es->keys, ((const mz_uint8 *)pBuf)[i]);
  }

  if ((int)pState->m_pZip->m_pWrite(pState->m_pZip->m_pIO_opaque, pState->m_cur_archive_file_ofs, enc_buf,
                                    len) != len) {
    free(enc_buf);
    return MZ_FALSE;
  }

  pState->m_cur_archive_file_ofs += len;
  pState->m_comp_size += len;
  free(enc_buf);
  return MZ_TRUE;
}
#endif /* ZIP_ENABLE_DEFLATE */

static const char *const zip_errlist[ZIP_NERRORS] = {
    NULL,
    "not initialized",
    "invalid entry name",
    "entry not found",
    "invalid zip mode",
    "invalid compression level",
    "no zip 64 support",
    "memset error",
    "cannot write data to entry",
    "cannot initialize tdefl compressor",
    "invalid index",
    "header not found",
    "cannot flush tdefl buffer",
    "cannot create entry header",
    "cannot write entry header",
    "cannot write to central dir",
    "cannot open file",
    "invalid entry type",
    "extracting data using no memory allocation",
    "file not found",
    "no permission",
    "out of memory",
    "invalid zip archive name",
    "make dir error",
    "symlink error",
    "close archive error",
    "capacity size too small",
    "fseek error",
    "fread error",
    "fwrite error",
    "cannot initialize reader",
    "cannot initialize writer",
    "cannot initialize writer from reader",
    "invalid argument",
    "cannot initialize reader iterator",
    "check dir error: path exists but is not directory",
    "wrong password or password required",
};

const char *zip_strerror(int errnum) {
  errnum = -errnum;
  if (errnum <= 0 || errnum >= ZIP_NERRORS) { return NULL; }

  return zip_errlist[errnum];
}

#if ZIP_ENABLE_DEFLATE
#ifndef MINIZ_NO_STDIO
static const char *zip_basename(const char *name) {
  char const *p;
  char const *base = name += FILESYSTEM_PREFIX_LEN(name);
  int all_slashes = 1;

  for (p = name; *p; p++) {
    if (ISSLASH(*p))
      base = p + 1;
    else
      all_slashes = 0;
  }

  /* If NAME is all slashes, arrange to return `/'. */
  if (*base == '\0' && ISSLASH(*name) && all_slashes) --base;

  return base;
}
#endif /* MINIZ_NO_STDIO */
#endif /* ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_INFLATE
#ifndef MINIZ_NO_STDIO
static int zip_mkpath(char *path, size_t pos) {
  char *p;
  char npath[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1];
  size_t len = pos;
  struct MZ_FILE_STAT_STRUCT st;

  memset(npath, 0, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1);
  strncpy(npath, path, len);

  if (MZ_FILE_STAT(npath, &st) < 0) { return ZIP_ENOFILE; }

  for (p = path + len; *p && len < MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE; p++) {
    if (ISSLASH(*p)) {
#if defined(_WIN32) || defined(__WIN32__) || defined(_MSC_VER) || defined(__MINGW32__)
      if (MZ_MKDIR(npath) == -1) {
        if (errno != EEXIST) { return ZIP_EMKDIR; }
      }
#else
      if ('\\' == *p) { *p = '/'; }

      if (lstat(npath, &st) < 0) {
        if (MZ_MKDIR(npath) == -1) {
          if (errno != EEXIST) { return ZIP_EMKDIR; }
        }
      } else {
        if (!S_ISDIR(st.st_mode)) {
          return ZIP_ECHKDIR;
        } else {
          // OK - DIR EXISTS
        }
      }
#endif
    }
    npath[len++] = *p;
  }
  return 0;
}
#endif /* MINIZ_NO_STDIO */
#endif /* ZIP_ENABLE_INFLATE */

static char *zip_strclone(const char *str, size_t n) {
  char c;
  size_t i;
  char *rpl = (char *)calloc((1 + n), sizeof(char));
  char *begin = rpl;
  if (!rpl) { return NULL; }

  for (i = 0; (i < n) && (c = *str++); ++i) {
    *rpl++ = c;
  }

  return begin;
}

#if ZIP_ENABLE_DEFLATE
static char *zip_strrpl(const char *str, size_t n, char oldchar, char newchar) {
  char c;
  size_t i;
  char *rpl = (char *)calloc((1 + n), sizeof(char));
  char *begin = rpl;
  if (!rpl) { return NULL; }

  for (i = 0; (i < n) && (c = *str++); ++i) {
    if (c == oldchar) { c = newchar; }
    *rpl++ = c;
  }

  return begin;
}
#endif /* ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_INFLATE
#ifndef MINIZ_NO_STDIO
static inline int zip_strchr_match(const char *const str, size_t len, char c) {
  size_t i;
  for (i = 0; i < len; ++i) {
    if (str[i] != c) { return 0; }
  }

  return 1;
}

static char *zip_name_normalize(char *name, char *const nname, size_t len) {
  size_t offn = 0, ncpy = 0;
  char c;

  if (name == NULL || nname == NULL || len == 0) { return NULL; }
  // skip trailing '/'
  while (ISSLASH(*name)) {
    name++;
  }

  while ((c = *name++)) {
    if (ISSLASH(c)) {
      if (ncpy > 0 && !zip_strchr_match(&nname[offn], ncpy, '.')) {
        offn += ncpy;
        nname[offn++] = c; // append '/'
      }
      ncpy = 0;
    } else {
      nname[offn + ncpy] = c;
      if (c) { ncpy++; }
    }
  }

  if (!zip_strchr_match(&nname[offn], ncpy, '.')) {
    nname[offn + ncpy] = '\0';
  } else {
    nname[offn] = '\0';
  }

  return nname;
}
#endif /* MINIZ_NO_STDIO */
#endif /* ZIP_ENABLE_INFLATE */

#if ZIP_ENABLE_DEFLATE
static int zip_archive_truncate(mz_zip_archive *pzip) {
  mz_zip_internal_state *pState = pzip->m_pState;
  mz_uint64 file_size = pzip->m_archive_size;
  if ((pzip->m_pWrite == mz_zip_heap_write_func) && (pState->m_pMem)) { return 0; }
  if (pzip->m_zip_mode == MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED) {
    if (pState->m_pFile) {
#ifndef MINIZ_NO_STDIO
      int fd = fileno(pState->m_pFile);
      return ftruncate(fd, pState->m_file_archive_start_ofs + file_size);
#else
      (void)file_size;
      return ZIP_ENOFILE;
#endif /* MINIZ_NO_STDIO */
    }
  }
  return 0;
}
#endif /* ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_INFLATE
#ifndef MINIZ_NO_STDIO

static int zip_archive_extract(mz_zip_archive *zip_archive, const char *dir,
                               int (*on_extract)(const char *filename, void *arg), void *arg) {
  int err = 0;
  mz_uint i, n;
  char path[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1];
#if ZIP_HAVE_SYMLINK
  char symlink_to[MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE + 1];
#endif
  mz_zip_archive_file_stat info;
  size_t dirlen = 0, filename_size = MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE;
  mz_uint32 xattr = 0;

  memset(path, 0, sizeof(path));
#if ZIP_HAVE_SYMLINK
  memset(symlink_to, 0, sizeof(symlink_to));
#endif

  dirlen = strlen(dir);
  if (dirlen == 0 || dirlen + 1 > MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE) { return ZIP_EINVENTNAME; }

  memset((void *)&info, 0, sizeof(mz_zip_archive_file_stat));

#if defined(_MSC_VER)
  strcpy_s(path, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE, dir);
#else
  strncpy(path, dir, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE);
#endif

  if (!ISSLASH(path[dirlen - 1])) {
#if defined(_WIN32) || defined(__WIN32__)
    path[dirlen] = '\\';
#else
    path[dirlen] = '/';
#endif
    ++dirlen;
  }

  if (filename_size > MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - dirlen) {
    filename_size = MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE - dirlen;
  }
  // Get and print information about each file in the archive.
  n = mz_zip_reader_get_num_files(zip_archive);
  for (i = 0; i < n; ++i) {
    if (!mz_zip_reader_file_stat(zip_archive, i, &info)) {
      // Cannot get information about zip archive;
      err = ZIP_ENOENT;
      goto out;
    }

    if (!zip_name_normalize(info.m_filename, info.m_filename, strlen(info.m_filename))) {
      // Cannot normalize file name;
      err = ZIP_EINVENTNAME;
      goto out;
    }

#if defined(_MSC_VER)
    strncpy_s(&path[dirlen], filename_size, info.m_filename, filename_size);
#else
    strncpy(&path[dirlen], info.m_filename, filename_size);
#endif

    err = zip_mkpath(path, dirlen);
    if (err < 0) {
      // Cannot make a path
      goto out;
    }

    if ((((info.m_version_made_by >> 8) == 3) ||
         ((info.m_version_made_by >> 8) == 19))   // if zip is produced on Unix or macOS (3 and 19 from
                                                  // section 4.4.2.2 of zip standard)
        && info.m_external_attr & (0x20 << 24)) { // and has sym link attribute (0x80 is file,
                                                  // 0x40 is directory)
#if ZIP_HAVE_SYMLINK
      if (info.m_uncomp_size > MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE ||
          !mz_zip_reader_extract_to_mem_no_alloc(zip_archive, i, symlink_to, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE,
                                                 0, NULL, 0)) {
        err = ZIP_EMEMNOALLOC;
        goto out;
      }
      symlink_to[info.m_uncomp_size] = '\0';
      if (symlink(symlink_to, path) != 0) {
        err = ZIP_ESYMLINK;
        goto out;
      }
#else
      if (!mz_zip_reader_extract_to_file(zip_archive, i, path, 0)) {
        err = ZIP_ENOFILE;
        goto out;
      }
#endif
    } else {
      if (!mz_zip_reader_is_file_a_directory(zip_archive, i)) {
        if (!mz_zip_reader_extract_to_file(zip_archive, i, path, 0)) {
          // Cannot extract zip archive to file
          err = ZIP_ENOFILE;
          goto out;
        }
      }

#if defined(_MSC_VER) || defined(PS4)
      (void)xattr; // unused
#else
      xattr = (info.m_external_attr >> 16) & 0xFFFF;
      if (xattr > 0 && xattr <= MZ_UINT16_MAX) {
        if (CHMOD(path, (mode_t)xattr) < 0) {
          err = ZIP_ENOPERM;
          goto out;
        }
      }
#endif
    }

    if (on_extract) {
      if (on_extract(path, arg) < 0) { goto out; }
    }
  }

out:
  // Close the archive, freeing any resources it was using
  if (!mz_zip_reader_end(zip_archive)) {
    // Cannot end zip reader
    err = ZIP_ECLSZIP;
  }
  return err;
}

#endif /* MINIZ_NO_STDIO */
#endif /* ZIP_ENABLE_INFLATE */

#if ZIP_ENABLE_DEFLATE

static inline void zip_archive_finalize(mz_zip_archive *pzip) {
  mz_zip_writer_finalize_archive(pzip);
  zip_archive_truncate(pzip);
}

static ssize_t zip_entry_mark(struct zip_t *zip, struct zip_entry_mark_t *entry_mark, const size_t n,
                              char *const entries[], const size_t len) {
  size_t i = 0;
  ssize_t err = 0;
  if (!zip || !entry_mark || !entries) { return ZIP_ENOINIT; }

  mz_zip_archive_file_stat file_stat;
  mz_uint64 d_pos = UINT64_MAX;
  for (i = 0; i < n; ++i) {
    if ((err = zip_entry_openbyindex(zip, i))) { return (ssize_t)err; }

    mz_bool name_matches = MZ_FALSE;
    {
      size_t j;
      for (j = 0; j < len; ++j) {
        if (strcmp(zip->entry.name, entries[j]) == 0) {
          name_matches = MZ_TRUE;
          break;
        }
      }
    }
    if (name_matches) {
      entry_mark[i].type = MZ_DELETE;
    } else {
      entry_mark[i].type = MZ_KEEP;
    }

    if (!mz_zip_reader_file_stat(&zip->archive, (mz_uint)i, &file_stat)) { return ZIP_ENOENT; }

    zip_entry_close(zip);

    entry_mark[i].m_local_header_ofs = file_stat.m_local_header_ofs;
    entry_mark[i].file_index = (ssize_t)-1;
    entry_mark[i].lf_length = 0;
    if ((entry_mark[i].type) == MZ_DELETE && (d_pos > entry_mark[i].m_local_header_ofs)) {
      d_pos = entry_mark[i].m_local_header_ofs;
    }
  }

  for (i = 0; i < n; ++i) {
    if ((entry_mark[i].m_local_header_ofs > d_pos) && (entry_mark[i].type != MZ_DELETE)) {
      entry_mark[i].type = MZ_MOVE;
    }
  }
  return err;
}

static ssize_t zip_entry_markbyindex(struct zip_t *zip, struct zip_entry_mark_t *entry_mark, const size_t n,
                                     size_t entries[], const size_t len) {
  size_t i = 0;
  ssize_t err = 0;
  if (!zip || !entry_mark || !entries) { return ZIP_ENOINIT; }

  mz_zip_archive_file_stat file_stat;
  mz_uint64 d_pos = UINT64_MAX;
  for (i = 0; i < n; ++i) {
    if ((err = zip_entry_openbyindex(zip, i))) { return (ssize_t)err; }

    mz_bool matches = MZ_FALSE;
    {
      size_t j;
      for (j = 0; j < len; ++j) {
        if (i == entries[j]) {
          matches = MZ_TRUE;
          break;
        }
      }
    }
    if (matches) {
      entry_mark[i].type = MZ_DELETE;
    } else {
      entry_mark[i].type = MZ_KEEP;
    }

    if (!mz_zip_reader_file_stat(&zip->archive, (mz_uint)i, &file_stat)) { return ZIP_ENOENT; }

    zip_entry_close(zip);

    entry_mark[i].m_local_header_ofs = file_stat.m_local_header_ofs;
    entry_mark[i].file_index = (ssize_t)-1;
    entry_mark[i].lf_length = 0;
    if ((entry_mark[i].type) == MZ_DELETE && (d_pos > entry_mark[i].m_local_header_ofs)) {
      d_pos = entry_mark[i].m_local_header_ofs;
    }
  }

  for (i = 0; i < n; ++i) {
    if ((entry_mark[i].m_local_header_ofs > d_pos) && (entry_mark[i].type != MZ_DELETE)) {
      entry_mark[i].type = MZ_MOVE;
    }
  }
  return err;
}
static ssize_t zip_index_next(mz_uint64 *local_header_ofs_array, ssize_t cur_index) {
  ssize_t new_index = 0, i;
  for (i = cur_index - 1; i >= 0; --i) {
    if (local_header_ofs_array[cur_index] > local_header_ofs_array[i]) {
      new_index = i + 1;
      return new_index;
    }
  }
  return new_index;
}

static ssize_t zip_sort(mz_uint64 *local_header_ofs_array, ssize_t cur_index) {
  ssize_t nxt_index = zip_index_next(local_header_ofs_array, cur_index);

  if (nxt_index != cur_index) {
    mz_uint64 temp = local_header_ofs_array[cur_index];
    ssize_t i;
    for (i = cur_index; i > nxt_index; i--) {
      local_header_ofs_array[i] = local_header_ofs_array[i - 1];
    }
    local_header_ofs_array[nxt_index] = temp;
  }
  return nxt_index;
}

static int zip_index_update(struct zip_entry_mark_t *entry_mark, ssize_t last_index, ssize_t nxt_index) {
  ssize_t j;
  for (j = 0; j < last_index; j++) {
    if (entry_mark[j].file_index >= nxt_index) { entry_mark[j].file_index += 1; }
  }
  entry_mark[nxt_index].file_index = last_index;
  return 0;
}

static int zip_entry_finalize(struct zip_t *zip, struct zip_entry_mark_t *entry_mark, const size_t n) {
  size_t i = 0;
  mz_uint64 *local_header_ofs_array = (mz_uint64 *)calloc(n, sizeof(mz_uint64));
  if (!local_header_ofs_array) { return ZIP_EOOMEM; }

  for (i = 0; i < n; ++i) {
    local_header_ofs_array[i] = entry_mark[i].m_local_header_ofs;
    ssize_t index = zip_sort(local_header_ofs_array, i);

    if ((size_t)index != i) { zip_index_update(entry_mark, i, index); }
    entry_mark[i].file_index = index;
  }

  size_t *length = (size_t *)calloc(n, sizeof(size_t));
  if (!length) {
    CLEANUP(local_header_ofs_array);
    return ZIP_EOOMEM;
  }
  for (i = 0; i < n - 1; i++) {
    length[i] = (size_t)(local_header_ofs_array[i + 1] - local_header_ofs_array[i]);
  }
  length[n - 1] = (size_t)(zip->archive.m_archive_size - local_header_ofs_array[n - 1]);

  for (i = 0; i < n; i++) {
    entry_mark[i].lf_length = length[entry_mark[i].file_index];
  }

  CLEANUP(length);
  CLEANUP(local_header_ofs_array);
  return 0;
}

static ssize_t zip_entry_set(struct zip_t *zip, struct zip_entry_mark_t *entry_mark, size_t n,
                             char *const entries[], const size_t len) {
  ssize_t err = 0;

  if ((err = zip_entry_mark(zip, entry_mark, n, entries, len)) < 0) { return err; }
  if ((err = zip_entry_finalize(zip, entry_mark, n)) < 0) { return err; }
  return 0;
}

static ssize_t zip_entry_setbyindex(struct zip_t *zip, struct zip_entry_mark_t *entry_mark, size_t n,
                                    size_t entries[], const size_t len) {
  ssize_t err = 0;

  if ((err = zip_entry_markbyindex(zip, entry_mark, n, entries, len)) < 0) { return err; }
  if ((err = zip_entry_finalize(zip, entry_mark, n)) < 0) { return err; }
  return 0;
}

static ssize_t zip_mem_move(void *pBuf, size_t bufSize, const mz_uint64 to, const mz_uint64 from,
                            const size_t length) {
  uint8_t *dst = NULL, *src = NULL, *end = NULL;

  if (!pBuf) { return ZIP_EINVIDX; }

  end = (uint8_t *)pBuf + bufSize;

  if (to > bufSize) { return ZIP_EINVIDX; }

  if (from > bufSize) { return ZIP_EINVIDX; }

  dst = (uint8_t *)pBuf + to;
  src = (uint8_t *)pBuf + from;

  if (((dst + length) > end) || ((src + length) > end)) { return ZIP_EINVIDX; }

  memmove(dst, src, length);
  return length;
}

#ifndef MINIZ_NO_STDIO
static ssize_t zip_file_move(MZ_FILE *m_pFile, const mz_uint64 to, const mz_uint64 from, const size_t length,
                             mz_uint8 *move_buf, const size_t capacity_size) {
  if (length > capacity_size) { return ZIP_ECAPSIZE; }
  if (MZ_FSEEK64(m_pFile, from, SEEK_SET)) { return ZIP_EFSEEK; }
  if (fread(move_buf, 1, length, m_pFile) != length) { return ZIP_EFREAD; }
  if (MZ_FSEEK64(m_pFile, to, SEEK_SET)) { return ZIP_EFSEEK; }
  if (fwrite(move_buf, 1, length, m_pFile) != length) { return ZIP_EFWRITE; }
  return (ssize_t)length;
}
#endif /* MINIZ_NO_STDIO */

static ssize_t zip_files_move(struct zip_t *zip, mz_uint64 writen_num, mz_uint64 read_num, size_t length) {
  ssize_t n = 0;
  const size_t page_size = 1 << 12; // 4K
  mz_zip_internal_state *pState = zip->archive.m_pState;

  mz_uint8 *move_buf = (mz_uint8 *)calloc(1, page_size);
  if (!move_buf) { return ZIP_EOOMEM; }

  ssize_t moved_length = 0;
  ssize_t move_count = 0;
  while ((mz_int64)length > 0) {
    move_count = (length >= page_size) ? page_size : length;

    if (pState->m_pFile) {
#ifndef MINIZ_NO_STDIO
      n = zip_file_move(pState->m_pFile, writen_num, read_num, move_count, move_buf, page_size);
#else
      CLEANUP(move_buf);
      return ZIP_ENOFILE;
#endif /* MINIZ_NO_STDIO */
    } else if (pState->m_pMem) {
      n = zip_mem_move(pState->m_pMem, pState->m_mem_size, writen_num, read_num, move_count);
    } else {
      CLEANUP(move_buf);
      return ZIP_ENOFILE;
    }

    if (n < 0) {
      moved_length = n;
      goto cleanup;
    }

    if (n != move_count) { goto cleanup; }

    writen_num += move_count;
    read_num += move_count;
    length -= move_count;
    moved_length += move_count;
  }

cleanup:
  CLEANUP(move_buf);
  return moved_length;
}

static int zip_central_dir_move(mz_zip_internal_state *pState, int begin, int end, int entry_num) {
  if (begin == entry_num) { return 0; }

  size_t l_size = 0;
  size_t r_size = 0;
  mz_uint32 d_size = 0;
  mz_uint8 *next = NULL;
  mz_uint8 *deleted =
      &MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir, mz_uint8,
                            MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, begin));
  l_size = (size_t)(deleted - (mz_uint8 *)(pState->m_central_dir.m_p));
  if (end == entry_num) {
    r_size = 0;
  } else {
    next = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir, mz_uint8,
                                 MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, end));
    r_size = pState->m_central_dir.m_size - (mz_uint32)(next - (mz_uint8 *)(pState->m_central_dir.m_p));
    d_size = (mz_uint32)(next - deleted);
  }

  if (next && l_size == 0) {
    memmove(pState->m_central_dir.m_p, next, r_size);
    pState->m_central_dir.m_p = MZ_REALLOC(pState->m_central_dir.m_p, r_size);
    {
      int i;
      for (i = end; i < entry_num; i++) {
        MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, i) -= d_size;
      }
    }
  }

  if (next && l_size * r_size != 0) {
    memmove(deleted, next, r_size);
    {
      int i;
      for (i = end; i < entry_num; i++) {
        MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, i) -= d_size;
      }
    }
  }

  pState->m_central_dir.m_size = l_size + r_size;
  return 0;
}

static int zip_central_dir_delete(mz_zip_internal_state *pState, int *deleted_entry_index_array,
                                  int entry_num) {
  int i = 0;
  int begin = 0;
  int end = 0;
  int d_num = 0;
  while (i < entry_num) {
    while ((i < entry_num) && (!deleted_entry_index_array[i])) {
      i++;
    }
    begin = i;

    while ((i < entry_num) && (deleted_entry_index_array[i])) {
      i++;
    }
    end = i;
    zip_central_dir_move(pState, begin, end, entry_num);
  }

  i = 0;
  while (i < entry_num) {
    while ((i < entry_num) && (!deleted_entry_index_array[i])) {
      i++;
    }
    begin = i;
    if (begin == entry_num) { break; }
    while ((i < entry_num) && (deleted_entry_index_array[i])) {
      i++;
    }
    end = i;
    int k = 0, j;
    for (j = end; j < entry_num; j++) {
      MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, begin + k) =
          (mz_uint32)MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, j);
      k++;
    }
    d_num += end - begin;
  }

  pState->m_central_dir_offsets.m_size = sizeof(mz_uint32) * (entry_num - d_num);
  return 0;
}

static ssize_t zip_entries_delete_mark(struct zip_t *zip, struct zip_entry_mark_t *entry_mark,
                                       int entry_num) {
  mz_uint64 writen_num = 0;
  mz_uint64 read_num = 0;
  size_t deleted_length = 0;
  size_t move_length = 0;
  int i = 0;
  size_t deleted_entry_num = 0;
  ssize_t n = 0;

  mz_bool *deleted_entry_flag_array = (mz_bool *)calloc(entry_num, sizeof(mz_bool));
  if (deleted_entry_flag_array == NULL) { return ZIP_EOOMEM; }

  mz_zip_internal_state *pState = zip->archive.m_pState;
  zip->archive.m_zip_mode = MZ_ZIP_MODE_WRITING;

  if (pState->m_pFile) {
#ifndef MINIZ_NO_STDIO
    if (MZ_FSEEK64(pState->m_pFile, 0, SEEK_SET)) {
      CLEANUP(deleted_entry_flag_array);
      return ZIP_ENOENT;
    }
#else
    CLEANUP(deleted_entry_flag_array);
    return ZIP_ENOFILE;
#endif /* MINIZ_NO_STDIO */
  }

  while (i < entry_num) {
    while ((i < entry_num) && (entry_mark[i].type == MZ_KEEP)) {
      writen_num += entry_mark[i].lf_length;
      read_num = writen_num;
      i++;
    }

    while ((i < entry_num) && (entry_mark[i].type == MZ_DELETE)) {
      deleted_entry_flag_array[i] = MZ_TRUE;
      read_num += entry_mark[i].lf_length;
      deleted_length += entry_mark[i].lf_length;
      i++;
      deleted_entry_num++;
    }

    while ((i < entry_num) && (entry_mark[i].type == MZ_MOVE)) {
      move_length += entry_mark[i].lf_length;
      mz_uint8 *p = &MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir, mz_uint8,
                                          MZ_ZIP_ARRAY_ELEMENT(&pState->m_central_dir_offsets, mz_uint32, i));
      if (!p) {
        CLEANUP(deleted_entry_flag_array);
        return ZIP_ENOENT;
      }
      mz_uint32 offset = MZ_READ_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS);
      offset -= (mz_uint32)deleted_length;
      MZ_WRITE_LE32(p + MZ_ZIP_CDH_LOCAL_HEADER_OFS, offset);
      i++;
    }

    n = zip_files_move(zip, writen_num, read_num, move_length);
    if (n != (ssize_t)move_length) {
      CLEANUP(deleted_entry_flag_array);
      return n;
    }
    writen_num += move_length;
    read_num += move_length;
  }

  zip->archive.m_archive_size -= (mz_uint64)deleted_length;
  zip->archive.m_total_files = (mz_uint32)entry_num - (mz_uint32)deleted_entry_num;

  zip_central_dir_delete(pState, deleted_entry_flag_array, entry_num);
  CLEANUP(deleted_entry_flag_array);

  return (ssize_t)deleted_entry_num;
}

#endif /* ZIP_ENABLE_DEFLATE */

static char *zip_password_clone(const char *password) {
  char *p;
  size_t len;
  if (!password) { return NULL; }
  len = strlen(password);
  if (len == 0) { return NULL; }
  p = (char *)calloc(len + 1, sizeof(char));
  if (p) { memcpy(p, password, len); }
  return p;
}

#ifndef MINIZ_NO_STDIO
struct zip_t *zip_open(const char *zipname, int level, char mode) {
  int errnum = 0;
  return zip_openwitherror(zipname, level, mode, &errnum);
}

struct zip_t *zip_openwitherror(const char *zipname, int level, char mode, int *errnum) {
  struct zip_t *zip = NULL;
#if ZIP_ENABLE_DEFLATE
  mz_uint wflags = (mode == 'w') ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0;
#endif
  *errnum = 0;

  if (!zipname || strlen(zipname) < 1) {
    // zip_t archive name is empty or NULL
    *errnum = ZIP_EINVZIPNAME;
    goto cleanup;
  }

  if (level < 0) level = MZ_DEFAULT_LEVEL;
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    // Wrong compression level
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }

  zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
  if (!zip) {
    // out of memory
    *errnum = ZIP_EOOMEM;
    goto cleanup;
  }

  zip->level = (mz_uint)level;
  zip->entry.index = -1;
  switch (mode) {
#if ZIP_ENABLE_DEFLATE
  case 'w':
  case ('w' - 64): {
    // Create a new archive.
    if (!mz_zip_writer_init_file_v2(&(zip->archive), zipname, 0, wflags)) {
      // Cannot initialize zip_archive writer
      *errnum = ZIP_EWINIT;
      goto cleanup;
    }
  } break;
#endif

#if ZIP_ENABLE_INFLATE
  case 'r':
  case ('r' - 64): {
    if (!mz_zip_reader_init_file_v2(&(zip->archive), zipname,
                                    zip->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 0, 0)) {
      // An archive file does not exist or cannot initialize
      // zip_archive reader
      *errnum = ZIP_ERINIT;
      goto cleanup;
    }
  } break;
#endif

#if ZIP_ENABLE_DEFLATE
  case 'a':
  case 'd':
  case ('a' - 64):
  case ('d' - 64): {
    MZ_FILE *fp = MZ_FOPEN(zipname, "r+b");
    if (!fp) {
      *errnum = ZIP_EOPNFILE;
      goto cleanup;
    }
    if (!mz_zip_reader_init_cfile(&(zip->archive), fp, 0,
                                  zip->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
      // An archive file does not exist or cannot initialize zip_archive
      // reader
      *errnum = ZIP_ERINIT;
      fclose(fp);
      goto cleanup;
    }
    if (!mz_zip_writer_init_from_reader_v2(&(zip->archive), zipname, 0)) {
      *errnum = ZIP_EWRINIT;
      fclose(fp);
      mz_zip_reader_end(&(zip->archive));
      goto cleanup;
    }
    // The file pointer is now owned by the archive object.
    zip->archive.m_zip_type = MZ_ZIP_TYPE_FILE;
  } break;
#endif

  default:
    *errnum = ZIP_EINVMODE;
    goto cleanup;
  }

  return zip;

cleanup:
  CLEANUP(zip);
  return NULL;
}

struct zip_t *zip_open_with_password(const char *zipname, int level, char mode, const char *password) {
  int errnum = 0;
  return zip_open_with_password_and_error(zipname, level, mode, password, &errnum);
}

struct zip_t *zip_open_with_password_and_error(const char *zipname, int level, char mode,
                                               const char *password, int *errnum) {
  struct zip_t *zip = NULL;
  int has_password = password && strlen(password) > 0;
  mz_uint wflags = 0;
  *errnum = 0;

  if (mode == 'w') {
    wflags = MZ_ZIP_FLAG_WRITE_ZIP64;
    if (has_password) wflags |= MZ_ZIP_FLAG_WRITE_ALLOW_READING;
  }

  if (!zipname || strlen(zipname) < 1) {
    *errnum = ZIP_EINVZIPNAME;
    goto cleanup;
  }

  if (level < 0) level = MZ_DEFAULT_LEVEL;
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }

  zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
  if (!zip) {
    *errnum = ZIP_EOOMEM;
    goto cleanup;
  }

  zip->level = (mz_uint)level;
  zip->entry.index = -1;

  if (has_password) {
    zip->password = zip_password_clone(password);
    if (!zip->password) {
      *errnum = ZIP_EOOMEM;
      goto cleanup;
    }
  }

  switch (mode) {
  case 'w':
  case ('w' - 64): {
    if (!mz_zip_writer_init_file_v2(&(zip->archive), zipname, 0, wflags)) {
      *errnum = ZIP_EWINIT;
      goto cleanup;
    }
    break;
  }
  case 'r': {
    if (!mz_zip_reader_init_file_v2(&(zip->archive), zipname, 0, 0, 0)) {
      *errnum = ZIP_ERINIT;
      goto cleanup;
    }
    break;
  }
  case 'a':
  case 'd':
  case ('d' - 64): {
    if (!mz_zip_reader_init_file_v2(&(zip->archive), zipname,
                                    wflags | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY, 0, 0)) {
      *errnum = ZIP_ERINIT;
      goto cleanup;
    }
    if (!mz_zip_writer_init_from_reader_v2(&(zip->archive), zipname, 0)) {
      *errnum = ZIP_EWRINIT;
      mz_zip_reader_end(&(zip->archive));
      goto cleanup;
    }
    break;
  }
  default:
    *errnum = ZIP_EINVMODE;
    goto cleanup;
  }

  return zip;

cleanup:
  if (zip) { CLEANUP(zip->password); }
  CLEANUP(zip);
  return NULL;
}
#endif /* MINIZ_NO_STDIO */

void zip_close(struct zip_t *zip) {
  if (zip) {
    mz_zip_archive *pZip = &(zip->archive);
#if ZIP_ENABLE_DEFLATE
    if (pZip->m_zip_mode == MZ_ZIP_MODE_WRITING) { mz_zip_writer_finalize_archive(pZip); }

    if (pZip->m_zip_mode == MZ_ZIP_MODE_WRITING ||
        pZip->m_zip_mode == MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED) {
      zip_archive_truncate(pZip);
      mz_zip_writer_end(pZip);
    } else
#endif
        if (pZip->m_zip_mode == MZ_ZIP_MODE_READING) {
      mz_zip_reader_end(pZip);
    }

    CLEANUP(zip->password);
    CLEANUP(zip);
  }
}

int zip_is64(struct zip_t *zip) {
  if (!zip || !zip->archive.m_pState) {
    // zip_t handler or zip state is not initialized
    return ZIP_ENOINIT;
  }

  return (int)zip->archive.m_pState->m_zip64;
}

int zip_offset(struct zip_t *zip, uint64_t *offset) {
  if (!zip || !zip->archive.m_pState) {
    // zip_t handler or zip state is not initialized
    return ZIP_ENOINIT;
  }

  *offset = mz_zip_get_archive_file_start_offset(&zip->archive);
  return 0;
}

static int _zip_entry_open(struct zip_t *zip, const char *entryname, int case_sensitive) {
  size_t entrylen = 0;
  mz_zip_archive *pzip = NULL;
  mz_zip_archive_file_stat stats;
#if ZIP_ENABLE_DEFLATE
  int err = 0;
  mz_uint num_alignment_padding_bytes, level;
  mz_uint16 dos_time = 0, dos_date = 0;
  mz_uint32 extra_size = 0;
  mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
  mz_uint64 local_dir_header_ofs = 0;
#endif

  if (!zip) { return ZIP_ENOINIT; }

#if ZIP_ENABLE_DEFLATE
  local_dir_header_ofs = zip->archive.m_archive_size;
#endif

  if (!entryname) { return ZIP_EINVENTNAME; }

  entrylen = strlen(entryname);
  if (entrylen == 0) { return ZIP_EINVENTNAME; }

  if (zip->entry.name) { CLEANUP(zip->entry.name); }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode == MZ_ZIP_MODE_READING) {
    zip->entry.name = zip_strclone(entryname, entrylen);
    if (!zip->entry.name) {
      // Cannot parse zip entry name
      return ZIP_EINVENTNAME;
    }

    zip->entry.index = (ssize_t)mz_zip_reader_locate_file(pzip, zip->entry.name, NULL,
                                                          case_sensitive ? MZ_ZIP_FLAG_CASE_SENSITIVE : 0);
    if (zip->entry.index < (ssize_t)0) {
      CLEANUP(zip->entry.name);
      return ZIP_ENOENT;
    }

    if (!mz_zip_reader_file_stat(pzip, (mz_uint)zip->entry.index, &stats)) {
      CLEANUP(zip->entry.name);
      return ZIP_ENOENT;
    }

    zip->entry.comp_size = stats.m_comp_size;
    zip->entry.uncomp_size = stats.m_uncomp_size;
    zip->entry.uncomp_crc32 = stats.m_crc32;
    zip->entry.dir_offset = stats.m_central_dir_ofs;
    zip->entry.header_offset = stats.m_local_header_ofs;
    zip->entry.method = stats.m_method;
    zip->entry.external_attr = stats.m_external_attr;
#ifndef MINIZ_NO_TIME
    zip->entry.m_time = stats.m_time;
#endif

    return 0;
  }

#if !ZIP_ENABLE_DEFLATE
  return ZIP_EINVMODE;
#else
  /*
    .ZIP File Format Specification Version: 6.3.3

    4.4.17.1 The name of the file, with optional relative path.
    The path stored MUST not contain a drive or
    device letter, or a leading slash.  All slashes
    MUST be forward slashes '/' as opposed to
    backwards slashes '\' for compatibility with Amiga
    and UNIX file systems etc.  If input came from standard
    input, there is no file name field.
  */
  zip->entry.name = zip_strrpl(entryname, entrylen, '\\', '/');
  if (!zip->entry.name) {
    // Cannot parse zip entry name
    return ZIP_EINVENTNAME;
  }

  level = zip->level & 0xF;

  zip->entry.index = (ssize_t)zip->archive.m_total_files;
  zip->entry.comp_size = 0;
  zip->entry.uncomp_size = 0;
  zip->entry.uncomp_crc32 = MZ_CRC32_INIT;
  zip->entry.dir_offset = zip->archive.m_archive_size;
  zip->entry.header_offset = zip->archive.m_archive_size;
  memset(zip->entry.header, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE * sizeof(mz_uint8));
  zip->entry.method = level ? MZ_DEFLATED : 0;

  // UNIX or APPLE
#if MZ_PLATFORM == 3 || MZ_PLATFORM == 19
  // regular file with rw-r--r-- permissions
  zip->entry.external_attr = (mz_uint32)(0100644) << 16;
#else
  zip->entry.external_attr = 0;
#endif

  num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pzip);

  if (!pzip->m_pState || (pzip->m_zip_mode != MZ_ZIP_MODE_WRITING)) {
    // Invalid zip mode
    err = ZIP_EINVMODE;
    goto cleanup;
  }
  if (zip->level & MZ_ZIP_FLAG_COMPRESSED_DATA) {
    // Invalid zip compression level
    err = ZIP_EINVLVL;
    goto cleanup;
  }

  if (!mz_zip_writer_write_zeros(pzip, zip->entry.dir_offset, num_alignment_padding_bytes)) {
    // Cannot memset zip entry header
    err = ZIP_EMEMSET;
    goto cleanup;
  }
  local_dir_header_ofs += num_alignment_padding_bytes;

  zip->entry.m_time = time(NULL);
#ifndef MINIZ_NO_TIME
  mz_zip_time_t_to_dos_time(zip->entry.m_time, &dos_time, &dos_date);
#endif

  // ZIP64 header with NULL sizes (sizes will be in the data descriptor, just
  // after file data)
  extra_size = mz_zip_writer_create_zip64_extra_data(
      extra_data, NULL, NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);

  {
    mz_uint16 gen_flags = MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8;
    if (zip->password) {
      gen_flags |= MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED;
    } else {
      gen_flags |= MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR;
    }

    if (!mz_zip_writer_create_local_dir_header(pzip, zip->entry.header, (mz_uint16)entrylen,
                                               (mz_uint16)extra_size, 0, 0, 0, zip->entry.method, gen_flags,
                                               dos_time, dos_date)) {
      err = ZIP_EMEMSET;
      goto cleanup;
    }
  }

  zip->entry.header_offset = zip->entry.dir_offset + num_alignment_padding_bytes;

  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.header_offset, zip->entry.header,
                     sizeof(zip->entry.header)) != sizeof(zip->entry.header)) {
    err = ZIP_EMEMSET;
    goto cleanup;
  }

  if (pzip->m_file_offset_alignment) {
    MZ_ASSERT((zip->entry.header_offset & (pzip->m_file_offset_alignment - 1)) == 0);
  }
  zip->entry.dir_offset += num_alignment_padding_bytes + sizeof(zip->entry.header);

  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, zip->entry.name, entrylen) != entrylen) {
    err = ZIP_EWRTENT;
    goto cleanup;
  }

  zip->entry.dir_offset += entrylen;

  if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, extra_data, extra_size) != extra_size) {
    err = ZIP_EWRTENT;
    goto cleanup;
  }
  zip->entry.dir_offset += extra_size;

  if (zip->password) {
    mz_uint8 enc_header[ZIP_PKWARE_ENCRYPT_HEADER_SIZE];
    size_t i;

    zip_pkware_keys_init_password(&zip->entry.enc_keys, zip->password);

    for (i = 0; i < ZIP_PKWARE_ENCRYPT_HEADER_SIZE - 1; i++) {
      mz_uint8 rnd = (mz_uint8)(mz_crc32(MZ_CRC32_INIT, enc_header, i) >> (i & 7));
      enc_header[i] = zip_pkware_encrypt_byte(&zip->entry.enc_keys, rnd);
    }
    enc_header[ZIP_PKWARE_ENCRYPT_HEADER_SIZE - 1] =
        zip_pkware_encrypt_byte(&zip->entry.enc_keys, (mz_uint8)(dos_time >> 8));

    zip->entry.enc_header_ofs = zip->entry.dir_offset;

    if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, enc_header,
                       ZIP_PKWARE_ENCRYPT_HEADER_SIZE) != ZIP_PKWARE_ENCRYPT_HEADER_SIZE) {
      err = ZIP_EWRTENT;
      goto cleanup;
    }
    zip->entry.dir_offset += ZIP_PKWARE_ENCRYPT_HEADER_SIZE;
    zip->entry.comp_size += ZIP_PKWARE_ENCRYPT_HEADER_SIZE;
  }

  if (level) {
    zip->entry.state.m_pZip = pzip;
    zip->entry.state.m_cur_archive_file_ofs = zip->entry.dir_offset;
    zip->entry.state.m_comp_size = 0;

    if (zip->password) {
      zip->entry.enc_state.inner_state = &(zip->entry.state);
      zip->entry.enc_state.keys = &(zip->entry.enc_keys);

      if (tdefl_init(&(zip->entry.comp), zip_encrypt_put_buf_callback, &(zip->entry.enc_state),
                     (int)tdefl_create_comp_flags_from_zip_params((int)level, -15, MZ_DEFAULT_STRATEGY)) !=
          TDEFL_STATUS_OKAY) {
        err = ZIP_ETDEFLINIT;
        goto cleanup;
      }
    } else {
      if (tdefl_init(&(zip->entry.comp), mz_zip_writer_add_put_buf_callback, &(zip->entry.state),
                     (int)tdefl_create_comp_flags_from_zip_params((int)level, -15, MZ_DEFAULT_STRATEGY)) !=
          TDEFL_STATUS_OKAY) {
        err = ZIP_ETDEFLINIT;
        goto cleanup;
      }
    }
  }

  return 0;

cleanup:
  CLEANUP(zip->entry.name);
  return err;
#endif /* ZIP_ENABLE_DEFLATE */
}

int zip_entry_open(struct zip_t *zip, const char *entryname) { return _zip_entry_open(zip, entryname, 0); }

int zip_entry_opencasesensitive(struct zip_t *zip, const char *entryname) {
  return _zip_entry_open(zip, entryname, 1);
}

int zip_entry_openbyindex(struct zip_t *zip, size_t index) {
  mz_zip_archive *pZip = NULL;
  mz_zip_archive_file_stat stats;
  mz_uint namelen;
  const mz_uint8 *pHeader;
  const char *pFilename;

  if (!zip) {
    // zip_t handler is not initialized
    return ZIP_ENOINIT;
  }

  pZip = &(zip->archive);
  if (pZip->m_zip_mode != MZ_ZIP_MODE_READING) {
    // open by index requires readonly mode
    return ZIP_EINVMODE;
  }

  if (index >= (size_t)pZip->m_total_files) {
    // index out of range
    return ZIP_EINVIDX;
  }

  if (!(pHeader = &MZ_ZIP_ARRAY_ELEMENT(
            &pZip->m_pState->m_central_dir, mz_uint8,
            MZ_ZIP_ARRAY_ELEMENT(&pZip->m_pState->m_central_dir_offsets, mz_uint32, (mz_uint)index)))) {
    // cannot find header in central directory
    return ZIP_ENOHDR;
  }

  namelen = MZ_READ_LE16(pHeader + MZ_ZIP_CDH_FILENAME_LEN_OFS);
  pFilename = (const char *)pHeader + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE;

  if (zip->entry.name) { CLEANUP(zip->entry.name); }

  zip->entry.name = zip_strclone(pFilename, namelen);
  if (!zip->entry.name) {
    // local entry name is NULL
    return ZIP_EINVENTNAME;
  }

  if (!mz_zip_reader_file_stat(pZip, (mz_uint)index, &stats)) { return ZIP_ENOENT; }

  zip->entry.index = (ssize_t)index;
  zip->entry.comp_size = stats.m_comp_size;
  zip->entry.uncomp_size = stats.m_uncomp_size;
  zip->entry.uncomp_crc32 = stats.m_crc32;
  zip->entry.dir_offset = stats.m_central_dir_ofs;
  zip->entry.header_offset = stats.m_local_header_ofs;
  zip->entry.method = stats.m_method;
  zip->entry.external_attr = stats.m_external_attr;
#ifndef MINIZ_NO_TIME
  zip->entry.m_time = stats.m_time;
#endif

  return 0;
}

int zip_entry_close(struct zip_t *zip) {
  mz_zip_archive *pzip = NULL;
  int err = 0;
#if ZIP_ENABLE_DEFLATE
  mz_uint level;
  tdefl_status done;
  mz_uint16 entrylen;
  mz_uint16 dos_time = 0, dos_date = 0;
  mz_uint8 *pExtra_data = NULL;
  mz_uint32 extra_size = 0;
  mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
  mz_uint8 local_dir_footer[MZ_ZIP_DATA_DESCRIPTER_SIZE64];
  mz_uint32 local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE64;
#endif

  if (!zip) {
    // zip_t handler is not initialized
    err = ZIP_ENOINIT;
    goto cleanup;
  }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode == MZ_ZIP_MODE_READING) { goto cleanup; }

#if !ZIP_ENABLE_DEFLATE
  (void)pzip;
  err = ZIP_EINVMODE;
  goto cleanup;
#else
  level = zip->level & 0xF;
  if (level) {
    done = tdefl_compress_buffer(&(zip->entry.comp), "", 0, TDEFL_FINISH);
    if (done != TDEFL_STATUS_DONE && done != TDEFL_STATUS_OKAY) {
      err = ZIP_ETDEFLBUF;
      goto cleanup;
    }
    zip->entry.comp_size = zip->entry.state.m_comp_size;
    if (zip->password) { zip->entry.comp_size += ZIP_PKWARE_ENCRYPT_HEADER_SIZE; }
    zip->entry.dir_offset = zip->entry.state.m_cur_archive_file_ofs;
    zip->entry.method = MZ_DEFLATED;
  }

  entrylen = (mz_uint16)strlen(zip->entry.name);
#ifndef MINIZ_NO_TIME
  mz_zip_time_t_to_dos_time(zip->entry.m_time, &dos_time, &dos_date);
#endif

  if (zip->password && zip->entry.enc_header_ofs > 0) {
    struct zip_pkware_keys_t fix_keys, orig_keys;
    mz_uint8 enc_header[ZIP_PKWARE_ENCRYPT_HEADER_SIZE];
    mz_uint8 check_byte = (mz_uint8)(zip->entry.uncomp_crc32 >> 24);
    mz_uint64 data_ofs, data_size;
    mz_zip_internal_state *pState = pzip->m_pState;
    size_t i;

    MZ_WRITE_LE32(zip->entry.header + MZ_ZIP_LDH_CRC32_OFS, zip->entry.uncomp_crc32);
    MZ_WRITE_LE32(zip->entry.header + MZ_ZIP_LDH_COMPRESSED_SIZE_OFS,
                  MZ_MIN(zip->entry.comp_size, MZ_UINT32_MAX));
    MZ_WRITE_LE32(zip->entry.header + MZ_ZIP_LDH_DECOMPRESSED_SIZE_OFS,
                  MZ_MIN(zip->entry.uncomp_size, MZ_UINT32_MAX));
    if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.header_offset, zip->entry.header,
                       sizeof(zip->entry.header)) != sizeof(zip->entry.header)) {
      err = ZIP_EWRTHDR;
      goto cleanup;
    }

    zip_pkware_keys_init_password(&fix_keys, zip->password);
    zip_pkware_keys_init_password(&orig_keys, zip->password);

    if (pState->m_pFile) {
#ifndef MINIZ_NO_STDIO
      if (MZ_FSEEK64(pState->m_pFile, (mz_int64)zip->entry.enc_header_ofs, SEEK_SET)) {
        err = ZIP_EFSEEK;
        goto cleanup;
      }
      if (fread(enc_header, 1, ZIP_PKWARE_ENCRYPT_HEADER_SIZE, pState->m_pFile) !=
          ZIP_PKWARE_ENCRYPT_HEADER_SIZE) {
        err = ZIP_EFREAD;
        goto cleanup;
      }
#else
      err = ZIP_ENOFILE;
      goto cleanup;
#endif /* MINIZ_NO_STDIO */
    } else if (pState->m_pMem) {
      memcpy(enc_header, (const mz_uint8 *)pState->m_pMem + zip->entry.enc_header_ofs,
             ZIP_PKWARE_ENCRYPT_HEADER_SIZE);
    } else {
      err = ZIP_EFREAD;
      goto cleanup;
    }

    for (i = 0; i < ZIP_PKWARE_ENCRYPT_HEADER_SIZE; i++) {
      mz_uint8 plain = zip_pkware_decrypt(&orig_keys, enc_header[i]);
      if (i == ZIP_PKWARE_ENCRYPT_HEADER_SIZE - 1) {
        enc_header[i] = zip_pkware_encrypt_byte(&fix_keys, check_byte);
      } else {
        enc_header[i] = zip_pkware_encrypt_byte(&fix_keys, plain);
      }
    }

    pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.enc_header_ofs, enc_header, ZIP_PKWARE_ENCRYPT_HEADER_SIZE);

    data_ofs = zip->entry.enc_header_ofs + ZIP_PKWARE_ENCRYPT_HEADER_SIZE;
    if (level) {
      data_size = zip->entry.state.m_comp_size;
    } else {
      data_size = zip->entry.comp_size - ZIP_PKWARE_ENCRYPT_HEADER_SIZE;
    }

    if (data_size > 0) {
      mz_uint8 re_buf[4096];
      mz_uint64 remaining = data_size;
      mz_uint64 ofs = data_ofs;
      while (remaining > 0) {
        size_t chunk = (remaining > sizeof(re_buf)) ? sizeof(re_buf) : (size_t)remaining;

        if (pState->m_pFile) {
#ifndef MINIZ_NO_STDIO
          if (MZ_FSEEK64(pState->m_pFile, (mz_int64)ofs, SEEK_SET)) {
            err = ZIP_EFSEEK;
            goto cleanup;
          }
          if (fread(re_buf, 1, chunk, pState->m_pFile) != chunk) {
            err = ZIP_EFREAD;
            goto cleanup;
          }
#else
          err = ZIP_ENOFILE;
          goto cleanup;
#endif /* MINIZ_NO_STDIO */
        } else if (pState->m_pMem) {
          memcpy(re_buf, (const mz_uint8 *)pState->m_pMem + ofs, chunk);
        } else {
          err = ZIP_EFREAD;
          goto cleanup;
        }

        for (i = 0; i < chunk; i++) {
          mz_uint8 plain = zip_pkware_decrypt(&orig_keys, re_buf[i]);
          re_buf[i] = zip_pkware_encrypt_byte(&fix_keys, plain);
        }
        if (pzip->m_pWrite(pzip->m_pIO_opaque, ofs, re_buf, chunk) != chunk) {
          err = ZIP_EFWRITE;
          goto cleanup;
        }
        ofs += chunk;
        remaining -= chunk;
      }
    }
  } else {
    MZ_WRITE_LE32(local_dir_footer + 0, MZ_ZIP_DATA_DESCRIPTOR_ID);
    MZ_WRITE_LE32(local_dir_footer + 4, zip->entry.uncomp_crc32);
    MZ_WRITE_LE64(local_dir_footer + 8, zip->entry.comp_size);
    MZ_WRITE_LE64(local_dir_footer + 16, zip->entry.uncomp_size);

    if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, local_dir_footer, local_dir_footer_size) !=
        local_dir_footer_size) {
      err = ZIP_EWRTHDR;
      goto cleanup;
    }
    zip->entry.dir_offset += local_dir_footer_size;
  }

  pExtra_data = extra_data;
  extra_size = mz_zip_writer_create_zip64_extra_data(
      extra_data, (zip->entry.uncomp_size >= MZ_UINT32_MAX) ? &zip->entry.uncomp_size : NULL,
      (zip->entry.comp_size >= MZ_UINT32_MAX) ? &zip->entry.comp_size : NULL,
      (zip->entry.header_offset >= MZ_UINT32_MAX) ? &zip->entry.header_offset : NULL);

  if ((entrylen) && ISSLASH(zip->entry.name[entrylen - 1]) && !zip->entry.uncomp_size) {
    /* Set DOS Subdirectory attribute bit. */
    zip->entry.external_attr |= MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG;
  }

  {
    mz_uint16 gen_flags = MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8;
    if (zip->password) {
      gen_flags |= MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_IS_ENCRYPTED;
    } else {
      gen_flags |= MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR;
    }

    if (!mz_zip_writer_add_to_central_dir(
            pzip, zip->entry.name, entrylen, pExtra_data, (mz_uint16)extra_size, "", 0,
            zip->entry.uncomp_size, zip->entry.comp_size, zip->entry.uncomp_crc32, zip->entry.method,
            gen_flags, dos_time, dos_date, zip->entry.header_offset, zip->entry.external_attr, NULL, 0)) {
      err = ZIP_EWRTDIR;
      goto cleanup;
    }
  }

  pzip->m_total_files++;
  pzip->m_archive_size = zip->entry.dir_offset;
#endif /* ZIP_ENABLE_DEFLATE */

cleanup:
  if (zip) {
    zip->entry.m_time = 0;
    zip->entry.index = -1;
    CLEANUP(zip->entry.name);
  }
  return err;
}

const char *zip_entry_name(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return NULL;
  }
  return zip->entry.name;
}

ssize_t zip_entry_index(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return (ssize_t)ZIP_ENOINIT;
  }

  return zip->entry.index;
}

int zip_entry_isdir(struct zip_t *zip) {
  mz_uint16 entrylen;
  if (!zip) {
    // zip_t handler is not initialized
    return ZIP_ENOINIT;
  }

  if (zip->entry.index < (ssize_t)0) {
    // zip entry is not opened
    return ZIP_EINVIDX;
  }

  if (!zip->entry.name) { return ZIP_EINVENTNAME; }

  entrylen = (mz_uint16)strlen(zip->entry.name);
  if (entrylen == 0) { return 0; }
  return ISSLASH(zip->entry.name[entrylen - 1]);
}

unsigned long long zip_entry_size(struct zip_t *zip) { return zip_entry_uncomp_size(zip); }

unsigned long long zip_entry_uncomp_size(struct zip_t *zip) { return zip ? zip->entry.uncomp_size : 0; }

unsigned long long zip_entry_comp_size(struct zip_t *zip) { return zip ? zip->entry.comp_size : 0; }

unsigned int zip_entry_crc32(struct zip_t *zip) { return zip ? zip->entry.uncomp_crc32 : 0; }

unsigned long long zip_entry_dir_offset(struct zip_t *zip) { return zip ? zip->entry.dir_offset : 0; }

unsigned long long zip_entry_header_offset(struct zip_t *zip) { return zip ? zip->entry.header_offset : 0; }

#if ZIP_ENABLE_DEFLATE

int zip_entry_write(struct zip_t *zip, const void *buf, size_t bufsize) {
  mz_uint level;
  mz_zip_archive *pzip = NULL;
  tdefl_status status;

  if (!zip) { return ZIP_ENOINIT; }

  pzip = &(zip->archive);
  if (buf && bufsize > 0) {
    zip->entry.uncomp_size += bufsize;
    zip->entry.uncomp_crc32 = (mz_uint32)mz_crc32(zip->entry.uncomp_crc32, (const mz_uint8 *)buf, bufsize);

    level = zip->level & 0xF;
    if (!level) {
      if (zip->password) {
        mz_uint8 *enc_buf = (mz_uint8 *)malloc(bufsize);
        size_t i;
        if (!enc_buf) { return ZIP_EOOMEM; }
        for (i = 0; i < bufsize; i++) {
          enc_buf[i] = zip_pkware_encrypt_byte(&zip->entry.enc_keys, ((const mz_uint8 *)buf)[i]);
        }
        if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, enc_buf, bufsize) != bufsize) {
          free(enc_buf);
          return ZIP_EWRTENT;
        }
        free(enc_buf);
      } else {
        if (pzip->m_pWrite(pzip->m_pIO_opaque, zip->entry.dir_offset, buf, bufsize) != bufsize) {
          return ZIP_EWRTENT;
        }
      }
      zip->entry.dir_offset += bufsize;
      zip->entry.comp_size += bufsize;
    } else {
      status = tdefl_compress_buffer(&(zip->entry.comp), buf, bufsize, TDEFL_NO_FLUSH);
      if (status != TDEFL_STATUS_DONE && status != TDEFL_STATUS_OKAY) { return ZIP_ETDEFLBUF; }
    }
  }

  return 0;
}

#ifndef MINIZ_NO_STDIO
int zip_entry_fwrite(struct zip_t *zip, const char *filename) {
  int err = 0;
  size_t n = 0;
  MZ_FILE *stream = NULL;
  mz_uint8 buf[MZ_ZIP_MAX_IO_BUF_SIZE];
  struct MZ_FILE_STAT_STRUCT file_stat;
  mz_uint16 modes;

  if (!zip) {
    // zip_t handler is not initialized
    return ZIP_ENOINIT;
  }

  memset((void *)&file_stat, 0, sizeof(struct MZ_FILE_STAT_STRUCT));
  if (MZ_FILE_STAT(filename, &file_stat) != 0) {
    // problem getting information - check errno
    return ZIP_ENOENT;
  }

#if defined(_WIN32) || defined(__WIN32__) || defined(DJGPP)
  (void)modes; // unused
#else
  /* Initialize with permission bits--which are not implementation-optional */
  modes = file_stat.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
  if (S_ISDIR(file_stat.st_mode)) modes |= UNX_IFDIR;
  if (S_ISREG(file_stat.st_mode)) modes |= UNX_IFREG;
  if (S_ISLNK(file_stat.st_mode)) modes |= UNX_IFLNK;
  if (S_ISBLK(file_stat.st_mode)) modes |= UNX_IFBLK;
  if (S_ISCHR(file_stat.st_mode)) modes |= UNX_IFCHR;
  if (S_ISFIFO(file_stat.st_mode)) modes |= UNX_IFIFO;
  if (S_ISSOCK(file_stat.st_mode)) modes |= UNX_IFSOCK;
  zip->entry.external_attr = (modes << 16) | !(file_stat.st_mode & S_IWUSR);
  if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
    zip->entry.external_attr |= MZ_ZIP_DOS_DIR_ATTRIBUTE_BITFLAG;
  }
#endif

  zip->entry.m_time = file_stat.st_mtime;

  if (!(stream = MZ_FOPEN(filename, "rb"))) {
    // Cannot open filename
    return ZIP_EOPNFILE;
  }

  while ((n = fread(buf, sizeof(mz_uint8), MZ_ZIP_MAX_IO_BUF_SIZE, stream)) > 0) {
    if (zip_entry_write(zip, buf, n) < 0) {
      err = ZIP_EWRTENT;
      break;
    }
  }
  fclose(stream);

  return err;
}
#endif /* MINIZ_NO_STDIO */

#endif /* ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_INFLATE

static ssize_t zip_entry_decrypt_and_read(struct zip_t *zip, void **buf, size_t *bufsize) {
  mz_zip_archive *pzip = &(zip->archive);
  mz_uint idx = (mz_uint)zip->entry.index;
  mz_zip_archive_file_stat stat;
  mz_uint8 local_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
  mz_uint64 cur_file_ofs;
  mz_uint16 fname_len, extra_len;
  mz_uint8 *enc_data = NULL;
  mz_uint8 *dec_data = NULL;
  void *out_buf = NULL;
  size_t enc_size, dec_size, i;
  struct zip_pkware_keys_t keys;

  if (!mz_zip_reader_file_stat(pzip, idx, &stat)) { return (ssize_t)ZIP_ENOENT; }

  cur_file_ofs = stat.m_local_header_ofs;
  if (pzip->m_pRead(pzip->m_pIO_opaque, cur_file_ofs, local_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) !=
      MZ_ZIP_LOCAL_DIR_HEADER_SIZE) {
    return (ssize_t)ZIP_EFREAD;
  }

  fname_len = MZ_READ_LE16(local_header + MZ_ZIP_LDH_FILENAME_LEN_OFS);
  extra_len = MZ_READ_LE16(local_header + MZ_ZIP_LDH_EXTRA_LEN_OFS);
  cur_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE + (mz_uint64)fname_len + extra_len;

  enc_size = (size_t)stat.m_comp_size;
  if (enc_size < ZIP_PKWARE_ENCRYPT_HEADER_SIZE) { return (ssize_t)ZIP_EPASSWD; }

  enc_data = (mz_uint8 *)malloc(enc_size);
  if (!enc_data) { return (ssize_t)ZIP_EOOMEM; }

  if (pzip->m_pRead(pzip->m_pIO_opaque, cur_file_ofs, enc_data, enc_size) != enc_size) {
    free(enc_data);
    return (ssize_t)ZIP_EFREAD;
  }

  zip_pkware_keys_init_password(&keys, zip->password);

  for (i = 0; i < ZIP_PKWARE_ENCRYPT_HEADER_SIZE; i++) {
    zip_pkware_decrypt(&keys, enc_data[i]);
  }

  dec_size = enc_size - ZIP_PKWARE_ENCRYPT_HEADER_SIZE;
  dec_data = enc_data + ZIP_PKWARE_ENCRYPT_HEADER_SIZE;

  for (i = 0; i < dec_size; i++) {
    dec_data[i] = zip_pkware_decrypt(&keys, dec_data[i]);
  }

  if (stat.m_method == MZ_DEFLATED) {
    size_t uncomp_size = (size_t)stat.m_uncomp_size;
    out_buf = malloc(uncomp_size + 1);
    if (!out_buf) {
      free(enc_data);
      return (ssize_t)ZIP_EOOMEM;
    }

    {
      tinfl_decompressor decomp;
      size_t in_pos = 0, out_pos = 0;
      tinfl_status tstatus;
      tinfl_init(&decomp);

      for (;;) {
        size_t in_avail = dec_size - in_pos;
        size_t out_avail = uncomp_size - out_pos;
        int flags = TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF;
        if (in_avail == 0) flags |= TINFL_FLAG_HAS_MORE_INPUT;

        tstatus = tinfl_decompress(&decomp, dec_data + in_pos, &in_avail, (mz_uint8 *)out_buf,
                                   (mz_uint8 *)out_buf + out_pos, &out_avail, flags);
        in_pos += in_avail;
        out_pos += out_avail;

        if (tstatus == TINFL_STATUS_DONE) break;
        if (tstatus < 0) {
          free(enc_data);
          free(out_buf);
          return (ssize_t)ZIP_EPASSWD;
        }
      }
    }

    free(enc_data);
    *buf = out_buf;
    if (bufsize) { *bufsize = uncomp_size; }
    return (ssize_t)uncomp_size;
  } else {
    size_t uncomp_size = (size_t)stat.m_uncomp_size;
    out_buf = malloc(uncomp_size + 1);
    if (!out_buf) {
      free(enc_data);
      return (ssize_t)ZIP_EOOMEM;
    }
    if (dec_size > uncomp_size) { dec_size = uncomp_size; }
    memcpy(out_buf, dec_data, dec_size);
    free(enc_data);
    *buf = out_buf;
    if (bufsize) { *bufsize = dec_size; }
    return (ssize_t)dec_size;
  }
}

ssize_t zip_entry_read(struct zip_t *zip, void **buf, size_t *bufsize) {
  mz_zip_archive *pzip = NULL;
  mz_uint idx;
  size_t size = 0;

  if (!zip) { return (ssize_t)ZIP_ENOINIT; }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return (ssize_t)ZIP_ENOENT;
  }

  idx = (mz_uint)zip->entry.index;
  if (mz_zip_reader_is_file_a_directory(pzip, idx)) { return (ssize_t)ZIP_EINVENTTYPE; }

  if (zip->password) { return zip_entry_decrypt_and_read(zip, buf, bufsize); }

  *buf = mz_zip_reader_extract_to_heap(pzip, idx, &size, 0);
  if (!*buf) { return (ssize_t)ZIP_EOOMEM; }
  if (bufsize) { *bufsize = size; }
  return (ssize_t)size;
}

ssize_t zip_entry_noallocread(struct zip_t *zip, void *buf, size_t bufsize) {
  mz_zip_archive *pzip = NULL;

  if (!zip) { return (ssize_t)ZIP_ENOINIT; }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return (ssize_t)ZIP_ENOENT;
  }

  if (zip->password) {
    void *heap_buf = NULL;
    size_t heap_size = 0;
    ssize_t n = zip_entry_decrypt_and_read(zip, &heap_buf, &heap_size);
    if (n < 0) { return n; }
    if (heap_size > bufsize) {
      free(heap_buf);
      return (ssize_t)ZIP_EMEMNOALLOC;
    }
    memcpy(buf, heap_buf, heap_size);
    free(heap_buf);
    return (ssize_t)heap_size;
  }

  if (!mz_zip_reader_extract_to_mem_no_alloc(pzip, (mz_uint)zip->entry.index, buf, bufsize, 0, NULL, 0)) {
    return (ssize_t)ZIP_EMEMNOALLOC;
  }

  return (ssize_t)zip->entry.uncomp_size;
}

ssize_t zip_entry_noallocreadwithoffset(struct zip_t *zip, size_t offset, size_t size, void *buf) {
  mz_zip_archive *pzip = NULL;

  if (!zip) { return (ssize_t)ZIP_ENOINIT; }

  if (offset >= (size_t)zip->entry.uncomp_size) { return (ssize_t)ZIP_EINVAL; }

  if ((offset + size) > (size_t)zip->entry.uncomp_size) {
    size = (size_t)(zip->entry.uncomp_size - (mz_uint64)offset);
  }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) {
    return (ssize_t)ZIP_ENOENT;
  }

  if (zip->password) {
    void *heap_buf = NULL;
    size_t heap_size = 0;
    ssize_t n = zip_entry_decrypt_and_read(zip, &heap_buf, &heap_size);
    if (n < 0) { return n; }
    if (offset + size > heap_size) { size = heap_size - offset; }
    memcpy(buf, (mz_uint8 *)heap_buf + offset, size);
    free(heap_buf);
    return (ssize_t)size;
  }

  {
    mz_zip_reader_extract_iter_state *iter =
        mz_zip_reader_extract_iter_new(pzip, (mz_uint)zip->entry.index, 0);
    mz_uint8 *writebuf = (mz_uint8 *)buf;
    size_t file_offset = 0;
    size_t write_cursor = 0;
    size_t to_read = size;

    if (!iter) { return (ssize_t)ZIP_ENORITER; }

    while (file_offset < zip->entry.uncomp_size && to_read > 0) {
      size_t nread = mz_zip_reader_extract_iter_read(iter, (void *)&writebuf[write_cursor], to_read);

      if (nread == 0) break;

      if (offset < (file_offset + nread)) {
        size_t read_cursor = offset - file_offset;
        size_t read_size = nread - read_cursor;
        MZ_ASSERT(read_cursor < size);

        if (to_read < read_size) read_size = to_read;
        MZ_ASSERT(read_size <= size);

        if (read_cursor != 0) { memmove(&writebuf[write_cursor], &writebuf[read_cursor], read_size); }

        write_cursor += read_size;
        offset += read_size;
        to_read -= read_size;
      }

      file_offset += nread;
    }

    mz_zip_reader_extract_iter_free(iter);
    return (ssize_t)write_cursor;
  }
}

#ifndef MINIZ_NO_STDIO
int zip_entry_fread(struct zip_t *zip, const char *filename) {
  mz_zip_archive *pzip = NULL;
  mz_uint idx;
  mz_uint32 xattr = 0;
  mz_zip_archive_file_stat info;

  if (!zip) { return ZIP_ENOINIT; }

  memset((void *)&info, 0, sizeof(mz_zip_archive_file_stat));
  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) { return ZIP_ENOENT; }

  idx = (mz_uint)zip->entry.index;
  if (mz_zip_reader_is_file_a_directory(pzip, idx)) { return ZIP_EINVENTTYPE; }

  if (zip->password) {
    void *heap_buf = NULL;
    size_t heap_size = 0;
    ssize_t n = zip_entry_decrypt_and_read(zip, &heap_buf, &heap_size);
    MZ_FILE *fp;
    if (n < 0) { return (int)n; }
    fp = MZ_FOPEN(filename, "wb");
    if (!fp) {
      free(heap_buf);
      return ZIP_EOPNFILE;
    }
    if (fwrite(heap_buf, 1, heap_size, fp) != heap_size) {
      fclose(fp);
      free(heap_buf);
      return ZIP_EFWRITE;
    }
    fclose(fp);
    free(heap_buf);
  } else {
    if (!mz_zip_reader_extract_to_file(pzip, idx, filename, 0)) { return ZIP_ENOFILE; }
  }

#if defined(_MSC_VER) || defined(PS4)
  (void)xattr; // unused
#else
  if (!mz_zip_reader_file_stat(pzip, idx, &info)) { return ZIP_ENOFILE; }

  xattr = (info.m_external_attr >> 16) & 0xFFFF;
  if (xattr > 0 && xattr <= MZ_UINT16_MAX) {
    if (CHMOD(filename, (mode_t)xattr) < 0) { return ZIP_ENOPERM; }
  }
#endif

  return 0;
}
#endif /* MINIZ_NO_STDIO */

int zip_entry_extract(struct zip_t *zip,
                      size_t (*on_extract)(void *arg, uint64_t offset, const void *buf, size_t bufsize),
                      void *arg) {
  mz_zip_archive *pzip = NULL;
  mz_uint idx;

  if (!zip) { return ZIP_ENOINIT; }

  pzip = &(zip->archive);
  if (pzip->m_zip_mode != MZ_ZIP_MODE_READING || zip->entry.index < (ssize_t)0) { return ZIP_ENOENT; }

  idx = (mz_uint)zip->entry.index;

  if (zip->password) {
    void *heap_buf = NULL;
    size_t heap_size = 0;
    ssize_t n = zip_entry_decrypt_and_read(zip, &heap_buf, &heap_size);
    if (n < 0) { return (int)n; }
    if (on_extract(arg, 0, heap_buf, heap_size) != heap_size) {
      free(heap_buf);
      return ZIP_EINVIDX;
    }
    free(heap_buf);
    return 0;
  }

  return (mz_zip_reader_extract_to_callback(pzip, idx, on_extract, arg, 0)) ? 0 : ZIP_EINVIDX;
}

#endif /* ZIP_ENABLE_INFLATE */

ssize_t zip_entries_total(struct zip_t *zip) {
  if (!zip) {
    // zip_t handler is not initialized
    return ZIP_ENOINIT;
  }

  return (ssize_t)zip->archive.m_total_files;
}

#if ZIP_ENABLE_DEFLATE

ssize_t zip_entries_delete(struct zip_t *zip, char *const entries[], size_t len) {
  ssize_t n = 0;
  ssize_t err = 0;
  struct zip_entry_mark_t *entry_mark = NULL;

  if (zip == NULL || (entries == NULL && len != 0)) { return ZIP_ENOINIT; }

  if (entries == NULL && len == 0) { return 0; }

  n = zip_entries_total(zip);
  if (n < 0) { return n; }

  entry_mark = (struct zip_entry_mark_t *)calloc((size_t)n, sizeof(struct zip_entry_mark_t));
  if (!entry_mark) { return ZIP_EOOMEM; }

  zip->archive.m_zip_mode = MZ_ZIP_MODE_READING;

  err = zip_entry_set(zip, entry_mark, (size_t)n, entries, len);
  if (err < 0) {
    CLEANUP(entry_mark);
    return err;
  }

  err = zip_entries_delete_mark(zip, entry_mark, (int)n);
  CLEANUP(entry_mark);
  return err;
}

ssize_t zip_entries_deletebyindex(struct zip_t *zip, size_t entries[], size_t len) {
  ssize_t n = 0;
  ssize_t err = 0;
  struct zip_entry_mark_t *entry_mark = NULL;

  if (zip == NULL || (entries == NULL && len != 0)) { return ZIP_ENOINIT; }

  if (entries == NULL && len == 0) { return 0; }

  n = zip_entries_total(zip);
  if (n < 0) { return n; }

  entry_mark = (struct zip_entry_mark_t *)calloc((size_t)n, sizeof(struct zip_entry_mark_t));
  if (!entry_mark) { return ZIP_EOOMEM; }

  zip->archive.m_zip_mode = MZ_ZIP_MODE_READING;

  err = zip_entry_setbyindex(zip, entry_mark, (size_t)n, entries, len);
  if (err < 0) {
    CLEANUP(entry_mark);
    return err;
  }

  err = zip_entries_delete_mark(zip, entry_mark, (int)n);
  CLEANUP(entry_mark);
  return err;
}

#endif /* ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_INFLATE

#ifndef MINIZ_NO_STDIO
int zip_stream_extract(const char *stream, size_t size, const char *dir,
                       int (*on_extract)(const char *filename, void *arg), void *arg) {
  mz_zip_archive zip_archive;
  if (!stream || !dir) {
    // Cannot parse zip archive stream
    return ZIP_ENOINIT;
  }
  memset(&zip_archive, 0, sizeof(mz_zip_archive));
  if (!mz_zip_reader_init_mem(&zip_archive, stream, size, 0)) {
    // Cannot initialize zip_archive reader
    return ZIP_ENOINIT;
  }

  return zip_archive_extract(&zip_archive, dir, on_extract, arg);
}
#endif /* MINIZ_NO_STDIO */

#endif /* ZIP_ENABLE_INFLATE */

struct zip_t *zip_stream_open(const char *stream, size_t size, int level, char mode) {
  int errnum = 0;
  return zip_stream_openwitherror(stream, size, level, mode, &errnum);
}

struct zip_t *zip_stream_open_with_password(const char *stream, size_t size, int level, char mode,
                                            const char *password) {
  int errnum = 0;
  struct zip_t *zip = zip_stream_openwitherror(stream, size, level, mode, &errnum);
  if (zip && password && strlen(password) > 0) { zip->password = zip_password_clone(password); }
  return zip;
}

struct zip_t *zip_stream_openwitherror(const char *stream, size_t size, int level, char mode, int *errnum) {
#if ZIP_ENABLE_DEFLATE
  mz_uint wflags = (mode == 'w') ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0;
#endif
  struct zip_t *zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
#if !ZIP_ENABLE_INFLATE && !ZIP_ENABLE_DEFLATE
  (void)stream;
  (void)size;
#endif
  if (!zip) {
    // out of memory
    *errnum = ZIP_EOOMEM;
    return NULL;
  }

  zip->entry.index = -1;

  if (level < 0) { level = MZ_DEFAULT_LEVEL; }
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    // Wrong compression level
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }
  zip->level = (mz_uint)level;

  switch (mode) {
#if ZIP_ENABLE_INFLATE
  case 'r':
  case ('r' - 64): {
    if (stream && size > 0) {
      if (!mz_zip_reader_init_mem(&(zip->archive), stream, size, 0)) {
        *errnum = ZIP_ERINIT;
        goto cleanup;
      }
    } else {
      *errnum = ZIP_EINVMODE;
      goto cleanup;
    }
  } break;
#endif

#if ZIP_ENABLE_DEFLATE
  case 'd':
  case ('d' - 64): {
    if (stream && size > 0) {
      if (!mz_zip_reader_init_mem(&(zip->archive), stream, size,
                                  zip->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
        *errnum = ZIP_ERINIT;
        goto cleanup;
      }
      if (!mz_zip_writer_init_from_reader_v2(&(zip->archive), NULL, 0)) {
        *errnum = ZIP_EWRINIT;
        mz_zip_reader_end(&(zip->archive));
        goto cleanup;
      }
      zip->archive.m_pWrite = zip_stream_delete_write_func;
    } else {
      *errnum = ZIP_EINVMODE;
      goto cleanup;
    }
  } break;

  case 'w':
  case ('w' - 64): {
    if (stream == NULL && size == 0) {
      if (!mz_zip_writer_init_heap_v2(&(zip->archive), 0, 1024, wflags)) {
        *errnum = ZIP_EWINIT;
        goto cleanup;
      }
    } else {
      *errnum = ZIP_EINVMODE;
      goto cleanup;
    }
  } break;
#endif

  default:
    *errnum = ZIP_EINVMODE;
    goto cleanup;
  }

  *errnum = 0;
  return zip;

cleanup:
  CLEANUP(zip);
  return NULL;
}

ssize_t zip_stream_copy(struct zip_t *zip, void **buf, size_t *bufsize) {
  size_t n;

  if (!zip || !buf) { return (ssize_t)ZIP_ENOINIT; }

#if ZIP_ENABLE_DEFLATE
  if (zip->archive.m_zip_mode == MZ_ZIP_MODE_WRITING) { zip_archive_finalize(&(zip->archive)); }
#endif

  if (!zip->archive.m_pState || !zip->archive.m_pState->m_pMem) { return (ssize_t)ZIP_ENOINIT; }

  n = (size_t)zip->archive.m_archive_size;
  if (bufsize != NULL) { *bufsize = n; }

  *buf = calloc(n, sizeof(unsigned char));
  if (!*buf) { return (ssize_t)ZIP_EOOMEM; }
  memcpy(*buf, zip->archive.m_pState->m_pMem, n);

  return (ssize_t)n;
}

void zip_stream_close(struct zip_t *zip) {
  if (zip) {
#if ZIP_ENABLE_DEFLATE
    mz_zip_writer_end(&(zip->archive));
#endif
#if ZIP_ENABLE_INFLATE
    mz_zip_reader_end(&(zip->archive));
#endif
    CLEANUP(zip->password);
    CLEANUP(zip);
  }
}

#ifndef MINIZ_NO_STDIO
struct zip_t *zip_cstream_open(FILE *stream, int level, char mode) {
  int errnum = 0;
  return zip_cstream_openwitherror(stream, level, mode, &errnum);
}

struct zip_t *zip_cstream_openwitherror(FILE *stream, int level, char mode, int *errnum) {
#if ZIP_ENABLE_DEFLATE
  mz_uint wflags = (mode == 'w') ? MZ_ZIP_FLAG_WRITE_ZIP64 : 0;
#endif
  struct zip_t *zip = NULL;
  *errnum = 0;
  if (!stream) {
    // zip archive stream is NULL
    *errnum = ZIP_ENOFILE;
    goto cleanup;
  }

  if (level < 0) level = MZ_DEFAULT_LEVEL;
  if ((level & 0xF) > MZ_UBER_COMPRESSION) {
    // Wrong compression level
    *errnum = ZIP_EINVLVL;
    goto cleanup;
  }

  zip = (struct zip_t *)calloc((size_t)1, sizeof(struct zip_t));
  if (!zip) {
    // out of memory
    *errnum = ZIP_EOOMEM;
    goto cleanup;
  }

  zip->level = (mz_uint)level;
  zip->entry.index = -1;
  switch (mode) {
#if ZIP_ENABLE_DEFLATE
  case 'w':
  case ('w' - 64): {
    // Create a new archive.
    if (!mz_zip_writer_init_cfile(&(zip->archive), stream, wflags)) {
      // Cannot initialize zip_archive writer
      *errnum = ZIP_EWINIT;
      goto cleanup;
    }
  } break;
#endif

#if ZIP_ENABLE_INFLATE
  case ('r' - 64):
  case 'r':
    if (!mz_zip_reader_init_cfile(&(zip->archive), stream, 0,
                                  zip->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
      // An archive file does not exist or cannot initialize
      // zip_archive reader
      *errnum = ZIP_ERINIT;
      goto cleanup;
    }
    break;
#endif

#if ZIP_ENABLE_DEFLATE
  case ('a' - 64):
  case ('d' - 64):
  case 'a':
  case 'd': {
    if (!mz_zip_reader_init_cfile(&(zip->archive), stream, 0,
                                  zip->level | MZ_ZIP_FLAG_DO_NOT_SORT_CENTRAL_DIRECTORY)) {
      // An archive file does not exist or cannot initialize
      // zip_archive reader
      *errnum = ZIP_ERINIT;
      goto cleanup;
    }
    if (!mz_zip_writer_init_from_reader_v2(&(zip->archive), NULL, 0)) {
      *errnum = ZIP_EWRINIT;
      mz_zip_reader_end(&(zip->archive));
      goto cleanup;
    }
  } break;
#endif

  default:
    *errnum = ZIP_EINVMODE;
    goto cleanup;
  }

  return zip;

cleanup:
  CLEANUP(zip);
  return NULL;
}

void zip_cstream_close(struct zip_t *zip) { zip_close(zip); }
#endif /* MINIZ_NO_STDIO */

#if ZIP_ENABLE_DEFLATE

#ifndef MINIZ_NO_STDIO
int zip_create(const char *zipname, const char *filenames[], size_t len) {
  int err = 0;
  size_t i;
  int open_err = 0;
  struct zip_t *zip;

  if (!zipname || !*zipname) { return ZIP_EINVZIPNAME; }

  zip = zip_openwitherror(zipname, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w', &open_err);
  if (!zip) { return open_err; }
  for (i = 0; i < len; ++i) {
    const char *name = filenames[i];
    if (!name) {
      err = ZIP_EINVENTNAME;
      break;
    }

    err = zip_entry_open(zip, zip_basename(name));
    if (err) { break; }

    err = zip_entry_fwrite(zip, name);
    if (err) {
      zip_entry_close(zip);
      break;
    }

    err = zip_entry_close(zip);
    if (err) { break; }
  }

  zip_close(zip);
  return err;
}
#endif /* MINIZ_NO_STDIO */

#endif /* ZIP_ENABLE_DEFLATE */

#if ZIP_ENABLE_INFLATE
#ifndef MINIZ_NO_STDIO

int zip_extract(const char *zipname, const char *dir, int (*on_extract)(const char *filename, void *arg),
                void *arg) {
  mz_zip_archive zip_archive;

  if (!zipname || !dir) {
    // Cannot parse zip archive name
    return ZIP_EINVZIPNAME;
  }

  memset(&zip_archive, 0, sizeof(mz_zip_archive));

  // Now try to open the archive.
  if (!mz_zip_reader_init_file_v2(&zip_archive, zipname, 0, 0, 0)) {
    // Cannot initialize zip_archive reader
    return ZIP_ENOINIT;
  }

  return zip_archive_extract(&zip_archive, dir, on_extract, arg);
}

#endif /* MINIZ_NO_STDIO */
#endif /* ZIP_ENABLE_INFLATE */

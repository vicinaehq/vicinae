add_library(sqlcipher STATIC sqlite3.c)

target_include_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_definitions(sqlcipher PRIVATE
    SQLITE_ENABLE_FTS5
    SQLITE_HAS_CODEC
    SQLITE_TEMP_STORE=2
    SQLITE_EXTRA_INIT=sqlcipher_extra_init
    SQLITE_EXTRA_SHUTDOWN=sqlcipher_extra_shutdown
)

# SQLCipher needs a crypto provider for the codec: OpenSSL on Linux, the
# CommonCrypto/Security framework on macOS.
if (APPLE)
    target_compile_definitions(sqlcipher PRIVATE SQLCIPHER_CRYPTO_CC)
    target_link_libraries(sqlcipher PRIVATE "-framework Security" "-framework CoreFoundation")
else()
    find_package(OpenSSL REQUIRED)
    target_compile_definitions(sqlcipher PRIVATE SQLCIPHER_CRYPTO_OPENSSL)
    target_link_libraries(sqlcipher PRIVATE OpenSSL::Crypto)
endif()

target_compile_options(sqlcipher PRIVATE -w)

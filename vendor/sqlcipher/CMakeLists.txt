add_library(sqlcipher STATIC sqlite3.c)

target_include_directories(sqlcipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_compile_definitions(sqlcipher PRIVATE
    SQLITE_ENABLE_FTS5
)

target_compile_options(sqlcipher PRIVATE -w)

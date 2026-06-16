cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME file-indexer)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FILE_INDEXER_BIN vicinae-file-indexer)
set(FILE_INDEXER_CORE ${FILE_INDEXER_BIN}-core)

find_package(glaze)

include("Figura")

set(FIGURA_DIR "${CMAKE_SOURCE_DIR}/figura")

figura_compile(
	SERVER
	PROTO "${FIGURA_DIR}/file-indexer.fig"
	LANG glaze
	NAMESPACE file_indexer_gen
	OUTPUT file-indexer-server.hpp
)


set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(PLATFORM_SRCS
		src/important-dir-watcher-linux.cpp
		src/background-thread-linux.cpp
	)
endif()

# core library used by the file indexer binary and the tests
add_library(${FILE_INDEXER_CORE} STATIC
	src/file-indexer.cpp
	src/file-indexer-db.cpp
	src/file-indexer-query-engine.cpp
	src/file-indexer-query-policy.cpp
	src/query-pool.cpp
	src/db-writer.cpp
	src/scan-dispatcher.cpp
	src/indexer-scanner.cpp
	src/incremental-scanner.cpp
	src/filesystem-walker.cpp
	src/entry-filter.cpp
	src/io-pacer.cpp
	src/home-directory-watcher.cpp
	${PLATFORM_SRCS}
	${VENDOR_DIR}/fuzzy-trigram/register.c
	${VENDOR_DIR}/spellfix/register.c
)

target_include_directories(${FILE_INDEXER_CORE} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${VENDOR_DIR}
	${VENDOR_DIR}/sqlcipher
)

target_compile_features(${FILE_INDEXER_CORE} PUBLIC cxx_std_23)

target_link_libraries(${FILE_INDEXER_CORE} PUBLIC
	sqlcipher
	vicinae::fuzzy
	vicinae::xdgpp
)

add_executable(${FILE_INDEXER_BIN}
	src/main.cpp
	src/indexer-service.cpp
	${SRCS}
)
target_include_directories(${FILE_INDEXER_BIN} PRIVATE ${GENOUT})
target_link_libraries(${FILE_INDEXER_BIN} PRIVATE
	${FILE_INDEXER_CORE}
	glaze::glaze
)

install(TARGETS ${FILE_INDEXER_BIN}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)

if (BUILD_TESTS)
	set(TEST_TARGET ${FILE_INDEXER_BIN}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET}
		tests/main.cpp
		tests/query-quality.cpp
	)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain ${FILE_INDEXER_CORE})
endif()

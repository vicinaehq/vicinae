cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME file-indexer)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(FILE_INDEXER_BIN vicinae-file-indexer)

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

add_executable(${FILE_INDEXER_BIN}
	src/main.cpp
	src/indexer-service.cpp
	src/file-indexer.cpp
	src/file-indexer-db.cpp
	src/db-writer.cpp
	src/writer-worker.cpp
	src/scan-dispatcher.cpp
	src/indexer-scanner.cpp
	src/incremental-scanner.cpp
	src/watcher-scanner.cpp
	src/filesystem-walker.cpp
	src/relevancy-scorer.cpp
	src/home-directory-watcher.cpp
	src/file-indexer-query-engine.cpp
	src/file-indexer-query-policy.cpp
	${CMAKE_SOURCE_DIR}/vendor/better-trigram/register.c
	${CMAKE_SOURCE_DIR}/vendor/spellfix/register.c
	${SRCS}
)
target_include_directories(${FILE_INDEXER_BIN} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${GENOUT}
	${CMAKE_SOURCE_DIR}/vendor
	${CMAKE_SOURCE_DIR}/vendor/sqlcipher
)
target_compile_features(${FILE_INDEXER_BIN} PRIVATE cxx_std_23)
target_link_libraries(${FILE_INDEXER_BIN} PRIVATE
	glaze::glaze
	sqlcipher
	vicinae::fuzzy
	vicinae::xdgpp
)

install(TARGETS ${FILE_INDEXER_BIN}
	RUNTIME DESTINATION ${VICINAE_LIBEXEC_DIR}
)

if (BUILD_TESTS)
	set(TEST_TARGET ${FILE_INDEXER_BIN}-tests)
	find_package(Catch2 3 REQUIRED)
	add_executable(${TEST_TARGET} tests/main.cpp src/file-indexer-query-policy.cpp)
	target_include_directories(${TEST_TARGET} PRIVATE include ${CMAKE_SOURCE_DIR}/vendor)
	target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2WithMain vicinae::fuzzy)
endif()

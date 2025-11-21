find_package(Catch2 3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Widgets)
find_package(Protobuf REQUIRED)

# Collect all test files
file(GLOB_RECURSE TEST_SOURCES
    "lib/*.cpp"
    "services/files-service/file-indexer/*.cpp"
)

# Add file-search source files needed for testing
set(FILE_SEARCH_SOURCES
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/relevancy-scorer.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/file-indexer-db.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/file-indexer.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/db-writer.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/scan-dispatcher.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/indexer-scanner.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/incremental-scanner.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/watcher-scanner.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/filesystem-walker.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/services/files-service/file-indexer/writer-worker.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/utils/utils.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/utils/migration-manager/migration-manager.cpp
    ${CMAKE_SOURCE_DIR}/vicinae/src/vicinae.cpp
)

# Add Qt resources for migrations
qt_add_resources(FILE_SEARCH_RESOURCES
    ${CMAKE_SOURCE_DIR}/vicinae/database/file-indexer/migrations.qrc
)

add_executable(all_tests
    ${TEST_SOURCES}
    ${FILE_SEARCH_SOURCES}
    ${FILE_SEARCH_RESOURCES}
)

set_target_properties(all_tests PROPERTIES AUTOMOC ON)

set(FIXTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fixtures)

target_include_directories(all_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/vicinae/src
    ${CMAKE_SOURCE_DIR}/vicinae/include
    ${CMAKE_BINARY_DIR}/generated
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/vicinae
)

target_compile_definitions(all_tests PRIVATE
	XDGPP_GROUP="[xdgpp]"
	FIXTURE_DIR="${FIXTURE_DIR}"
	XDGPP_FIXTURE_DIR="${FIXTURE_DIR}/xdgpp"
	FILE_SEARCH_GROUP="[file-search]"
	FILE_SEARCH_FIXTURE_DIR="${FIXTURE_DIR}/file-indexer"
)

target_link_libraries(all_tests
    PRIVATE
    xdgpp
	Qt::Core
	Qt::Gui
	Qt::Sql
	Qt::Widgets
	protobuf::libprotobuf
	Catch2::Catch2
	cap
)

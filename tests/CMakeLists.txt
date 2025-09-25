find_package(Catch2 3 REQUIRED)

# Collect all test files
file(GLOB_RECURSE TEST_SOURCES
    "lib/**/*.cpp"
)

add_executable(all_tests
    ${TEST_SOURCES}
)

set(FIXTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/fixtures)

target_compile_definitions(all_tests PRIVATE 
	XDGPP_GROUP="[xdgpp]"
	FIXTURE_DIR="${FIXTURE_DIR}"
	XDGPP_FIXTURE_DIR="${FIXTURE_DIR}/xdgpp"
)

target_link_libraries(all_tests 
    PRIVATE 
    xdgpp
	Catch2::Catch2WithMain
)

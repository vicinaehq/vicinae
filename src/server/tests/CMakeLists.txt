find_package(Catch2 3 REQUIRED)
include(Catch)

add_executable(vicinae-server-tests
	main.cpp
	services/clipboard.cpp
	services/screenshots.cpp
	../src/services/clipboard/clipboard-mime.cpp
)
target_include_directories(vicinae-server-tests PRIVATE ../src)
target_link_libraries(vicinae-server-tests PRIVATE
	Catch2::Catch2 vicinae::common vicinae::fuzzy Qt6::Gui)
catch_discover_tests(vicinae-server-tests
	TEST_PREFIX "server."
	DISCOVERY_MODE PRE_TEST
	PROPERTIES LABELS unit TIMEOUT 15)

if (APPLE)
	add_executable(vicinae-server-native-tests
		main.cpp
		native/macos/screenshots.cpp
		native/macos/thumbnails.cpp
		../src/services/screenshots/abstract-screenshot-provider.hpp
		../src/services/screenshots/macos/macos-screenshot-provider.hpp
		../src/services/screenshots/macos/macos-screenshot-provider.mm
		../src/ui/image/mac-file-thumbnail-loader.mm
	)
	target_include_directories(vicinae-server-native-tests PRIVATE ../src)
	target_compile_definitions(vicinae-server-native-tests PRIVATE
		VICINAE_TEST_GUI
		VICINAE_TEST_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures")
	set_source_files_properties(
		../src/services/screenshots/macos/macos-screenshot-provider.mm
		../src/ui/image/mac-file-thumbnail-loader.mm
		PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
	target_link_libraries(vicinae-server-native-tests PRIVATE
		Catch2::Catch2 vicinae::common vicinae::fuzzy Qt6::Gui Qt6::Concurrent
		"-framework Foundation" "-framework QuickLookThumbnailing" "-framework CoreGraphics")
	catch_discover_tests(vicinae-server-native-tests
		TEST_PREFIX "server.macos."
		DISCOVERY_MODE PRE_TEST
		PROPERTIES LABELS native TIMEOUT 30)
endif()

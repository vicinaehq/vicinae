find_package(Catch2 3 REQUIRED)
include(Catch)

if (APPLE)
	add_executable(vicinae-server-tests
		main.cpp
		clipboard-macos.mm
		screenshots-macos.cpp
		thumbnails-macos.cpp
		../src/services/clipboard/clipboard-server.hpp
		../src/services/clipboard/clipboard-mime.cpp
		../src/services/clipboard/macos/macos-clipboard-server.hpp
		../src/services/clipboard/macos/macos-clipboard-server.mm
		../src/services/screenshots/abstract-screenshot-provider.hpp
		../src/services/screenshots/macos/macos-screenshot-provider.hpp
		../src/services/screenshots/macos/macos-screenshot-provider.mm
		../src/ui/image/mac-file-thumbnail-loader.mm
	)
	target_include_directories(vicinae-server-tests PRIVATE ../src)
	target_compile_definitions(vicinae-server-tests PRIVATE
		VICINAE_TEST_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures")
	set_source_files_properties(
		clipboard-macos.mm
		../src/services/clipboard/macos/macos-clipboard-server.mm
		../src/services/screenshots/macos/macos-screenshot-provider.mm
		../src/ui/image/mac-file-thumbnail-loader.mm
		PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
	target_link_libraries(vicinae-server-tests PRIVATE
		Catch2::Catch2 vicinae::common vicinae::fuzzy Qt6::Gui Qt6::Concurrent
		"-framework AppKit" "-framework Foundation" "-framework CoreServices" "-framework QuickLookThumbnailing" "-framework CoreGraphics")
	catch_discover_tests(vicinae-server-tests
		TEST_PREFIX "server."
		DISCOVERY_MODE PRE_TEST
		PROPERTIES TIMEOUT 30)
endif()

function(import_mathtext)
	if (POLICY CMP0135)
		cmake_policy(SET CMP0135 NEW)
	endif()

	set(BUILD_SHARED_LIBS OFF)
	set(CMAKE_SKIP_INSTALL_RULES ON)
	set(JKQtPlotter_BUILD_LIB_JKQTMATH OFF)
	set(JKQtPlotter_BUILD_LIB_JKQTPLOTTER OFF)
	set(JKQtPlotter_BUILD_LIB_JKQTFASTPLOTTER OFF)
	set(JKQtPlotter_BUILD_INCLUDE_XITS_FONTS ON)
	set(JKQtPlotter_BUILD_INCLUDE_FIRAMATH_FONTS OFF)
	set(JKQtPlotter_BUILD_FORCE_NO_PRINTER_SUPPORT ON)

	# Scope git apply to the downloaded source instead of the enclosing checkout.
	FetchContent_Declare(jkqtmathtext
		URL https://github.com/jkriege2/JKQtPlotter/archive/d9b6f18eac22f5e9959a5415d7ba0d8780f24585.tar.gz
		URL_HASH SHA256=e64716932c2e56e97041bcb4dbabe7c9045ea7d60addc4edb38eed43425286e8
		PATCH_COMMAND ${CMAKE_COMMAND} -E env "GIT_CEILING_DIRECTORIES=<SOURCE_DIR>/.."
			git apply --ignore-whitespace "${CMAKE_CURRENT_LIST_DIR}/patches/jkqtmathtext-implication.patch"
			"${CMAKE_CURRENT_LIST_DIR}/patches/jkqtmathtext-font-discovery.patch"
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(jkqtmathtext)
endfunction()

import_mathtext()

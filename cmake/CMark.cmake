function(setup_vendored_cmark)
	# Suppress cmark-gfm's own install() rules (scoped to this function and the
	# add_subdirectory below) so `cmake --install` does not ship its libraries,
	# headers, pkg-config or cmake export files.
	set(CMAKE_SKIP_INSTALL_RULES ON)

	set(CMARK_TESTS OFF)
	set(CMARK_STATIC ON)
	set(CMARK_SHARED OFF)

	add_subdirectory(${VENDOR_DIR}/cmark-gfm EXCLUDE_FROM_ALL)

	# cmark-gfm's CMakeLists.txt uses the legacy include_directories() (directory
	# scope) instead of target_include_directories(), so its include path doesn't
	# propagate to consumers. Attach it manually.
	foreach(_target libcmark-gfm_static libcmark-gfm-extensions_static)
		if (TARGET ${_target})
			target_include_directories(${_target} INTERFACE
				${VENDOR_DIR}/cmark-gfm/src
				${VENDOR_DIR}/cmark-gfm/extensions
				${CMAKE_BINARY_DIR}/vendor/cmark-gfm/src
				${CMAKE_BINARY_DIR}/vendor/cmark-gfm/extensions
			)
		endif()
	endforeach()

	if (TARGET libcmark-gfm-extensions_static AND TARGET libcmark-gfm_static)
		target_link_libraries(libcmark-gfm-extensions_static INTERFACE libcmark-gfm_static)
	endif()
endfunction()

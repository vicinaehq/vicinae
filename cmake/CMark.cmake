include(FetchContent)

function(checkout_cmark)
	set(CMAKE_SKIP_INSTALL_RULES ON)
	set(FETCHCONTENT_QUIET OFF)

	FetchContent_Declare(
	  cmark-gfm
	  EXCLUDE_FROM_ALL
	  GIT_REPOSITORY https://github.com/github/cmark-gfm
	  GIT_TAG        0.29.0.gfm.13
	)

	set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
	set(CMARK_TESTS OFF)
	set(CMARK_STATIC ON)
	set(CMARK_SHARED OFF)

	
	FetchContent_MakeAvailable(cmark-gfm)

	# cmark-gfm's CMakeLists.txt uses the legacy include_directories() (directory
	# scope) instead of target_include_directories(), so its include path doesn't
	# propagate to consumers via FetchContent. Attach it manually.
	foreach(_target libcmark-gfm_static libcmark-gfm-extensions_static)
		if (TARGET ${_target})
			target_include_directories(${_target} INTERFACE
				${cmark-gfm_SOURCE_DIR}/src
				${cmark-gfm_BINARY_DIR}/src
				${cmark-gfm_SOURCE_DIR}/extensions
				${cmark-gfm_BINARY_DIR}/extensions
			)
		endif()
	endforeach()

	set(CMAKE_SKIP_INSTALL_RULES OFF)
endfunction()

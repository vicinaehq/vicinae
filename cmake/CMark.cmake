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
	set(CMAKE_SKIP_INSTALL_RULES OFF)
endfunction()

include(FetchContent)

function(checkout_minizip)
	set(FETCHCONTENT_QUIET OFF)

	FetchContent_Declare(
	  minizip
	  GIT_REPOSITORY https://github.com/zlib-ng/minizip-ng
	  GIT_TAG        4.0.10
	)

	set(CMARK_TESTS OFF)
	set(CMARK_STATIC ON)
	set(CMARK_SHARED OFF)
	
	FetchContent_MakeAvailable(minizip)
endfunction()

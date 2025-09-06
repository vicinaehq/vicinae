include(FetchContent)

function(checkout_rapidfuzz)
	set(FETCHCONTENT_QUIET OFF)

	FetchContent_Declare(
	  rapidfuzz
	  EXCLUDE_FROM_ALL
	  GIT_REPOSITORY https://github.com/rapidfuzz/rapidfuzz-cpp
	  GIT_TAG        v3.3.3
	)
	
	set(BUILD_SHARED_LIBS OFF)
	FetchContent_MakeAvailable(rapidfuzz)
endfunction()

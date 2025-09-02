include(FetchContent)

function(checkout_abseil)
	set(FETCHCONTENT_QUIET OFF)

	FetchContent_Declare(
	  abseil
	  EXCLUDE_FROM_ALL
	  GIT_REPOSITORY https://github.com/abseil/abseil-cpp
	  GIT_TAG        20250814.0
	)
	
	FetchContent_MakeAvailable(abseil)
endfunction()

include(FetchContent)

function(checkout_glaze)
	set(FETCHCONTENT_QUIET OFF)
	FetchContent_Declare(
	  glaze
	  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
	  GIT_TAG v7.0.2
	  GIT_SHALLOW TRUE
	  EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(glaze)

endfunction()

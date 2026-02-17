include(FetchContent)

function(import_kf6)
	set(BUILD_SHARED_LIBS OFF)
	set(FETCHCONTENT_QUIET OFF)
	FetchContent_Declare(
      KF6
	  GIT_REPOSITORY https://github.com/KDE/syntax-highlighting
	  GIT_TAG v6.20.0
	  GIT_SHALLOW TRUE
	  EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(KF6)
	set(BUILD_SHARED_LIBS ON)

	# kf6 does not create the alias by itself
	if (NOT TARGET KF6::SyntaxHighlighting)
		add_library(KF6::SyntaxHighlighting ALIAS KF6SyntaxHighlighting)
	endif()
endfunction()

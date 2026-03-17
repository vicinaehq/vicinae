include(FetchContent)

function(import_kf6)
	set(BUILD_SHARED_LIBS OFF)
	set(FETCHCONTENT_QUIET OFF)

	# ECM (extra-cmake-modules) 6.x is required by the KF6 syntax-highlighting
	# CMakeLists.txt but is not yet packaged as 6.x on Ubuntu 22/24 or Debian 12.
	# Fetch it from source so find_package(ECM 6.20.0) succeeds.
	FetchContent_Declare(
		ECM
		GIT_REPOSITORY https://invent.kde.org/frameworks/extra-cmake-modules
		GIT_TAG v6.20.0
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(ECM)
	set(ECM_DIR "${ecm_BINARY_DIR}" CACHE PATH "Path to ECMConfig.cmake" FORCE)

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

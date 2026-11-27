include(FetchContent)

set(KF6_VERSION 6.20.0)
set(ECM_CONFIG_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/ECMConfig.cmake.in")

# ECM's own package config only works installed
function(import_ecm)
	FetchContent_Declare(
	  ECM
	  GIT_REPOSITORY https://github.com/KDE/extra-cmake-modules
	  GIT_TAG v${KF6_VERSION}
	  GIT_SHALLOW TRUE
	  SOURCE_SUBDIR modules
	)
	FetchContent_MakeAvailable(ECM)

	set(config_dir ${CMAKE_BINARY_DIR}/ecm-config)
	configure_file(${ECM_CONFIG_TEMPLATE} ${config_dir}/ECMConfig.cmake @ONLY)
	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(${config_dir}/ECMConfigVersion.cmake
		VERSION ${KF6_VERSION}
		COMPATIBILITY AnyNewerVersion
	)
	set(ECM_DIR ${config_dir} PARENT_SCOPE)
endfunction()

function(import_kf6)
	find_package(ECM ${KF6_VERSION} QUIET NO_MODULE)
	if (NOT ECM_FOUND)
		import_ecm()
	endif()

	# KDECMakeSettings would otherwise create this cache entry as TRUE for the whole project
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
	# KDEInstallDirs would otherwise force CMAKE_INSTALL_PREFIX to ECM_PREFIX on a fresh cache
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT FALSE)
	set(BUILD_TESTING OFF)
	set(CMAKE_DISABLE_FIND_PACKAGE_XercesC ON)
	# KDECMakeSettings otherwise wires KDE translation fetching against our own checkout
	add_custom_target(fetch-translations)
	set(FETCHCONTENT_QUIET OFF)

	# ECM (extra-cmake-modules) 6.x is required by the KF6 syntax-highlighting
	# CMakeLists.txt but is not yet packaged as 6.x on Ubuntu 22/24 or Debian 12.
	# Fetch the source; then replace the generated ECMConfig.cmake (which
	# hard-codes install-tree paths that do not exist) with one that points
	# directly to the ECM source-tree module directories.
	FetchContent_Declare(
		ECM
		GIT_REPOSITORY https://invent.kde.org/frameworks/extra-cmake-modules
		GIT_TAG v6.20.0
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(ECM)

	# Overwrite the broken build-tree ECMConfig.cmake with one that resolves
	# module paths from the source directory (no installation required).
	set(_ecm_modules   "${ecm_SOURCE_DIR}/modules")
	set(_ecm_find      "${ecm_SOURCE_DIR}/find-modules")
	set(_ecm_kde       "${ecm_SOURCE_DIR}/kde-modules")
	file(WRITE "${ecm_BINARY_DIR}/ECMConfig.cmake"
		"set(ECM_VERSION \"6.20.0\")\n"
		"set(ECM_VERSION_MAJOR 6)\n"
		"set(ECM_VERSION_MINOR 20)\n"
		"set(ECM_VERSION_PATCH 0)\n"
		"set(ECM_MODULE_DIR \"${_ecm_modules}\")\n"
		"set(ECM_FIND_MODULE_DIR \"${_ecm_find}\")\n"
		"set(ECM_KDE_MODULE_DIR \"${_ecm_kde}\")\n"
		"list(APPEND CMAKE_MODULE_PATH"
		" \"${_ecm_modules}\""
		" \"${_ecm_find}\""
		" \"${_ecm_kde}\")\n"
	)
	set(ECM_DIR "${ecm_BINARY_DIR}" CACHE PATH "Path to ECMConfig.cmake" FORCE)

	FetchContent_Declare(
      KF6
	  GIT_REPOSITORY https://github.com/KDE/syntax-highlighting
	  GIT_TAG v${KF6_VERSION}
	  GIT_SHALLOW TRUE
	  EXCLUDE_FROM_ALL
	  OVERRIDE_FIND_PACKAGE
	)
	FetchContent_MakeAvailable(KF6)

	# kf6 does not create the alias by itself
	if (NOT TARGET KF6::SyntaxHighlighting)
		add_library(KF6::SyntaxHighlighting ALIAS KF6SyntaxHighlighting)
	endif()
endfunction()

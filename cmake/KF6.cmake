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

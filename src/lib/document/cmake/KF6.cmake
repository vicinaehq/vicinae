include(FetchContent)

set(DOCUMENT_KF6_VERSION 6.20.0)

# ECM's own package config only works installed
function(document_fetch_ecm)
	FetchContent_Declare(
	  ECM
	  GIT_REPOSITORY https://github.com/KDE/extra-cmake-modules
	  GIT_TAG v${DOCUMENT_KF6_VERSION}
	  GIT_SHALLOW TRUE
	  SOURCE_SUBDIR modules
	)
	FetchContent_MakeAvailable(ECM)

	set(config_dir ${CMAKE_CURRENT_BINARY_DIR}/ecm-config)
	configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ECMConfig.cmake.in" ${config_dir}/ECMConfig.cmake @ONLY)
	include(CMakePackageConfigHelpers)
	write_basic_package_version_file(${config_dir}/ECMConfigVersion.cmake
		VERSION ${DOCUMENT_KF6_VERSION}
		COMPATIBILITY AnyNewerVersion
	)
	set(ECM_DIR ${config_dir} PARENT_SCOPE)
endfunction()

function(document_fetch_kf6)
	find_package(ECM ${DOCUMENT_KF6_VERSION} QUIET NO_MODULE)
	if (NOT ECM_FOUND)
		document_fetch_ecm()
	endif()

	# KDECMakeSettings would otherwise create this cache entry as TRUE for the whole project
	set(BUILD_SHARED_LIBS OFF)
	# KDEInstallDirs would otherwise force CMAKE_INSTALL_PREFIX to ECM_PREFIX on a fresh cache
	set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT FALSE)
	set(BUILD_TESTING OFF)
	set(CMAKE_DISABLE_FIND_PACKAGE_XercesC ON)
	# KDECMakeSettings otherwise wires KDE translation fetching against our own checkout
	if (NOT TARGET fetch-translations)
		add_custom_target(fetch-translations)
	endif()
	set(FETCHCONTENT_QUIET OFF)
	FetchContent_Declare(
      KF6
	  GIT_REPOSITORY https://github.com/KDE/syntax-highlighting
	  GIT_TAG v${DOCUMENT_KF6_VERSION}
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

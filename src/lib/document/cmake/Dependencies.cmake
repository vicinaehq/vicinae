include(FetchContent)

function(document_fetch_cmark)
	set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
	set(CMAKE_SKIP_INSTALL_RULES ON)
	set(CMARK_TESTS OFF)
	set(CMARK_STATIC ON)
	set(CMARK_SHARED OFF)
	FetchContent_Declare(document_cmark
		URL https://github.com/github/cmark-gfm/archive/refs/tags/0.29.0.gfm.13.tar.gz
		URL_HASH SHA256=5abc61798ebd9de5660bc076443c07abad2b8d15dbc11094a3a79644b8ad243a
		DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(document_cmark)
	foreach(target libcmark-gfm_static libcmark-gfm-extensions_static)
		target_include_directories(${target} INTERFACE
			"${document_cmark_SOURCE_DIR}/src" "${document_cmark_SOURCE_DIR}/extensions"
			"${document_cmark_BINARY_DIR}/src" "${document_cmark_BINARY_DIR}/extensions"
		)
	endforeach()
	target_link_libraries(libcmark-gfm-extensions_static INTERFACE libcmark-gfm_static)
endfunction()

function(document_fetch_pugixml)
	set(BUILD_SHARED_LIBS OFF)
	set(CMAKE_SKIP_INSTALL_RULES ON)
	FetchContent_Declare(document_pugixml
		URL https://github.com/zeux/pugixml/archive/refs/tags/v1.14.tar.gz
		URL_HASH SHA256=610f98375424b5614754a6f34a491adbddaaec074e9044577d965160ec103d2e
		DOWNLOAD_EXTRACT_TIMESTAMP TRUE
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(document_pugixml)
endfunction()

if (NOT TARGET KF6::SyntaxHighlighting)
	if (USE_SYSTEM_KF6)
		find_package(KF6SyntaxHighlighting REQUIRED)
	else()
		include("${CMAKE_CURRENT_LIST_DIR}/KF6.cmake")
		document_fetch_kf6()
	endif()
endif()
if (NOT TARGET JKQTMathText6)
	include("${CMAKE_CURRENT_LIST_DIR}/MathText.cmake")
	document_fetch_mathtext()
endif()
if (NOT TARGET pugixml)
	find_package(pugixml QUIET)
	if (NOT TARGET pugixml::pugixml)
		document_fetch_pugixml()
	endif()
	set(DOCUMENT_PUGIXML pugixml::pugixml)
else()
	set(DOCUMENT_PUGIXML pugixml)
endif()
if (TARGET libcmark-gfm_static)
	set(DOCUMENT_CMARK libcmark-gfm_static libcmark-gfm-extensions_static)
elseif (USE_SYSTEM_CMARK_GFM)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(DOCUMENT_CMARK REQUIRED IMPORTED_TARGET libcmark-gfm)
	set(DOCUMENT_CMARK PkgConfig::DOCUMENT_CMARK)
else()
	find_package(PkgConfig QUIET)
	if (PkgConfig_FOUND)
		pkg_check_modules(DOCUMENT_CMARK QUIET IMPORTED_TARGET libcmark-gfm)
	endif()
	if (TARGET PkgConfig::DOCUMENT_CMARK)
		set(DOCUMENT_CMARK PkgConfig::DOCUMENT_CMARK)
	else()
		document_fetch_cmark()
		set(DOCUMENT_CMARK libcmark-gfm_static libcmark-gfm-extensions_static)
	endif()
endif()


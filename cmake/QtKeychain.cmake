include(FetchContent)

function(checkout_qt_keychain)
	set(FETCHCONTENT_QUIET OFF)

	FetchContent_Declare(
	  qt-keychain
	  EXCLUDE_FROM_ALL
	  GIT_REPOSITORY https://github.com/frankosterfeld/qtkeychain
	  GIT_TAG v0.14.0
	)

	set(BUILD_SHARED_LIBS OFF)
	set(BUILD_WITH_QT6 ON)
	set(BUILD_TRANSLATIONS OFF)
	FetchContent_MakeAvailable(qt-keychain)

	# expose the vendored headers under the qt6keychain/ prefix that installed
	# packages use, so #include <qt6keychain/keychain.h> works either way
	set(shim_dir "${CMAKE_BINARY_DIR}/qtkeychain-include")
	file(MAKE_DIRECTORY "${shim_dir}/qt6keychain")
	file(CREATE_LINK "${qt-keychain_SOURCE_DIR}/keychain.h"
	     "${shim_dir}/qt6keychain/keychain.h" COPY_ON_ERROR SYMBOLIC)
	file(CREATE_LINK "${qt-keychain_BINARY_DIR}/qkeychain_export.h"
	     "${shim_dir}/qt6keychain/qkeychain_export.h" COPY_ON_ERROR SYMBOLIC)
	target_include_directories(qt6keychain INTERFACE "$<BUILD_INTERFACE:${shim_dir}>")
endfunction()

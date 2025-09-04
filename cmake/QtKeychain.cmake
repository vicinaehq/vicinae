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
	set(LIBSECRET_SUPPORT OFF)
	FetchContent_MakeAvailable(qt-keychain)
endfunction()

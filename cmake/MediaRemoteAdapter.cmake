# The adapter enables Objective-C in its own project(), which only sets the language up
# properly when it is already known at our level.
enable_language(OBJC)

function(import_mediaremote_adapter)
	if (POLICY CMP0135)
		cmake_policy(SET CMP0135 NEW)
	endif()

	FetchContent_Declare(mediaremoteadapter
		URL https://github.com/ungive/mediaremote-adapter/archive/0e82bf1c44ec39554fbfc402ec88a48c6ae2a0fd.tar.gz
		URL_HASH SHA256=9e31b154dc7be207a42b7bb1ba1b161bfdee541a15d692cba1c37740f13b23e2
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(mediaremoteadapter)
endfunction()

import_mediaremote_adapter()

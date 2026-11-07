# both need to be found on the system to use local libs, otherwise everything is getting fetched
find_package(whisper QUIET)
find_package(parakeet QUIET)

if (whisper_FOUND AND NOT WHISPER_VERSION VERSION_LESS MIN_WHISPER_VERSION AND parakeet_FOUND)
	set(SYSTEM_INFERENCE_ENGINES_FOUND ON PARENT_SCOPE)
else()
	set(SYSTEM_INFERENCE_ENGINES_FOUND OFF PARENT_SCOPE)
endif()

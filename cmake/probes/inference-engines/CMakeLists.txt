# all three need to be found on the system to use local libs, otherwise everything is getting fetched
find_package(whisper ${MIN_WHISPER_VERSION} QUIET)
find_package(parakeet QUIET)
find_package(llama QUIET)

if (whisper_FOUND AND parakeet_FOUND AND llama_FOUND)
	set(SYSTEM_INFERENCE_ENGINES_FOUND ON PARENT_SCOPE)
else()
	set(SYSTEM_INFERENCE_ENGINES_FOUND OFF PARENT_SCOPE)
endif()

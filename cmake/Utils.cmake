function(get_cxx_compiler_name CXX_COMPILER_NAME)
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		set(${CXX_COMPILER_NAME} "GCC" PARENT_SCOPE)
	else()
		set(${CXX_COMPILER_NAME} "${CMAKE_CXX_COMPILER_ID}" PARENT_SCOPE)
	endif()
endfunction()

# Embeds the UTF-8 activeCodePage manifest into an executable so it runs with a UTF-8
# process code page (Windows 10 1903+). Pair with vicinae::enableUtf8() at runtime.
# Added as a source so CMake merges it into the linker-generated manifest via mt.exe,
# rather than embedding a second, conflicting MANIFEST resource.
function(vicinae_enable_utf8 target)
	if(WIN32)
		target_sources(${target} PRIVATE "${CMAKE_SOURCE_DIR}/extra/windows/vicinae.manifest")
	endif()
endfunction()


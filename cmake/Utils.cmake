function(get_cxx_compiler_name CXX_COMPILER_NAME)
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		set(${CXX_COMPILER_NAME} "GCC" PARENT_SCOPE)
	else()
		set(${CXX_COMPILER_NAME} "${CMAKE_CXX_COMPILER_ID}" PARENT_SCOPE)
	endif()
endfunction()


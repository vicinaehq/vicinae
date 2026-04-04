include(CMakeParseArguments)

set(CODEGEN_BIN ${CMAKE_BINARY_DIR}/bin/figura)
set(GENOUT ${CMAKE_BINARY_DIR}/generated)

set(FIGURA_CC ${CODEGEN_BIN})

function(figura_compile)
	cmake_parse_arguments(
        ARG
		"CLIENT;SERVER"
		"LANG;PROTO;NAMESPACE;OUTPUT"
		""
        ${ARGN}
    )

	set(GENFILE ${GENOUT}/${ARG_OUTPUT})

	set(CMD_ARGS "${FIGURA_CC}" compile "${ARG_PROTO}" --output "${GENFILE}")

	if (ARG_CLIENT)
		list(APPEND CMD_ARGS --client ${ARG_LANG})
	endif()

	if (ARG_SERVER)
		list(APPEND CMD_ARGS --server ${ARG_LANG})
	endif()

	if (DEFINED ARG_NAMESPACE)
		list(APPEND CMD_ARGS --namespace ${ARG_NAMESPACE})
	endif()

	add_custom_command(
	  OUTPUT ${GENFILE}
	  COMMAND ${CMD_ARGS}
	  DEPENDS ${ARG_PROTO} ${CODEGEN_BIN}
	  COMMENT "figura codegen client"
	)

	set(SRCS ${SRCS} ${GENFILE} PARENT_SCOPE)
endfunction()

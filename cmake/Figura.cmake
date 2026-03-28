set(CODEGEN_BIN ${CMAKE_BINARY_DIR}/bin/figura)
set(GENOUT ${CMAKE_BINARY_DIR}/generated)

function(figura_codegen_client proto language out)
	set(GENFILE ${GENOUT}/${out})
	add_custom_command(
	  OUTPUT ${GENFILE}
	  COMMAND ${CODEGEN_BIN} compile ${proto} --client ${language} --output ${GENFILE}
	  DEPENDS ${proto} ${CODEGEN_BIN}
	  COMMENT "figura codegen client"
	)

	set(SRCS ${SRCS} ${GENFILE} PARENT_SCOPE)
endfunction()

function(figura_codegen_server proto language out)
	set(GENFILE ${GENOUT}/${out})
	add_custom_command(
	  OUTPUT ${GENFILE}
	  COMMAND ${CODEGEN_BIN} compile ${proto} --server ${language} --output ${GENFILE}
	  DEPENDS ${proto} ${CODEGEN_BIN}
	  COMMENT "figura codegen server"
	)

	set(SRCS ${SRCS} ${GENFILE} PARENT_SCOPE)
endfunction()

set(CODEGEN_BIN ${CMAKE_BINARY_DIR}/bin/codegen)
set(GENOUT ${CMAKE_BINARY_DIR}/generated)
set(GENFILE ${GENOUT}/codegen.hpp)

function(codegen_do proto)
	add_custom_command(
	  OUTPUT ${GENFILE}
	  COMMAND ${CODEGEN_BIN} ${proto} glaze-qt ${GENFILE}
	  DEPENDS ${proto} ${CODEGEN_BIN}
	  COMMENT "codegen glaze-qt"
	)

	set(SRCS ${SRCS} ${GENFILE} PARENT_SCOPE)
endfunction()

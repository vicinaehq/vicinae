set(CODEGEN_BIN ${CMAKE_BINARY_DIR}/bin/codegen)
set(GENOUT ${CMAKE_BINARY_DIR}/generated)
set(GENFILE ${GENOUT}/codegen.hpp)

function(figura_codegen proto)
	add_custom_command(
	  OUTPUT ${GENFILE}
	  COMMAND ${CODEGEN_BIN} ${proto} glaze-qt ${GENFILE}
	  DEPENDS ${proto} ${CODEGEN_BIN}
	  COMMENT "figura codegen"
	)

	set(SRCS ${SRCS} ${GENFILE} PARENT_SCOPE)
endfunction()

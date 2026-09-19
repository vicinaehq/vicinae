function(embed_binary TARGET FILE_PATH VAR_NAME)
	set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/${VAR_NAME}.cpp")
	add_custom_command(
		OUTPUT "${OUTPUT_FILE}"
		COMMAND ${CMAKE_COMMAND}
			-DINPUT="${FILE_PATH}" -DOUTPUT="${OUTPUT_FILE}" -DVAR="${VAR_NAME}"
			-P "${CMAKE_SOURCE_DIR}/cmake/embed-binary-script.cmake"
		DEPENDS "${FILE_PATH}" "${CMAKE_SOURCE_DIR}/cmake/embed-binary-script.cmake"
		COMMENT "Embedding ${FILE_PATH}"
	)
	target_sources(${TARGET} PRIVATE "${OUTPUT_FILE}")
endfunction()

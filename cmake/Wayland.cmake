set(WAYLAND_SCANNER_EXECUTABLE "wayland-scanner")

function(wayland_generate_protocol protocol)
	set(protocol_file ${CMAKE_SOURCE_DIR}/src/wayland-protocols/${protocol}.xml)
	get_filename_component(protocol_name ${protocol_file} NAME_WE)
    
    set(client_header "${CMAKE_CURRENT_BINARY_DIR}/${protocol_name}-client-protocol.h")
    set(private_code "${CMAKE_CURRENT_BINARY_DIR}/${protocol_name}-protocol.c")
    
    # Generate client header
    add_custom_command(
        OUTPUT ${client_header}
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE} client-header ${protocol_file} ${client_header}
        DEPENDS ${protocol_file}
        COMMENT "Generating ${protocol_name} client header"
    )
    
    # Generate private code
    add_custom_command(
        OUTPUT ${private_code}
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE} private-code ${protocol_file} ${private_code}
        DEPENDS ${protocol_file}
        COMMENT "Generating ${protocol_name} private code"
    )
    
    # Add generated files to sources
    set(SRCS ${SRCS} ${client_header} ${private_code} PARENT_SCOPE)
endfunction()

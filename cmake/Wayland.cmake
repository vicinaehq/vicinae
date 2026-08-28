set(WAYLAND_SCANNER_EXECUTABLE "wayland-scanner")

if(NOT TARGET Wayland::Scanner)
	find_program(WAYLAND_SCANNER_PATH wayland-scanner REQUIRED)
	add_executable(Wayland::Scanner IMPORTED)
	set_target_properties(Wayland::Scanner PROPERTIES IMPORTED_LOCATION "${WAYLAND_SCANNER_PATH}")
endif()

# Official protocols come from the system wayland-protocols collection. Only protocols
if(NOT WAYLAND_PROTOCOLS_DATADIR)
	find_package(PkgConfig REQUIRED)
	pkg_get_variable(WAYLAND_PROTOCOLS_DATADIR wayland-protocols pkgdatadir)
	if(NOT WAYLAND_PROTOCOLS_DATADIR)
		message(FATAL_ERROR "wayland-protocols not found (looked up with pkg-config)")
	endif()
endif()

# Resolves a protocol XML from the system wayland-protocols
function(wayland_system_protocol outvar relpath)
	set(xml "${WAYLAND_PROTOCOLS_DATADIR}/${relpath}")
	if(NOT EXISTS "${xml}")
		message(FATAL_ERROR "${relpath} not found in ${WAYLAND_PROTOCOLS_DATADIR}: wayland-protocols >= 1.45 is required")
	endif()
	set(${outvar} "${xml}" PARENT_SCOPE)
endfunction()

# C-only generation, for when we need a protocol's interface symbols without any
# client-side logic. Takes a full path to the protocol XML.
function(wayland_generate_protocol protocol_file)
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

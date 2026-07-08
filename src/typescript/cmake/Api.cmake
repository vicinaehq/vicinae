include("Figura")
set(EXT_API_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/api")
set(EXT_API_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/api/dist")
set(API_DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/api/dist")
set(API_PROTO_OUT "${EXT_API_SRC_DIR}/src/api/proto")
set(API_NODE_MODULES "${EXT_API_SRC_DIR}/node_modules")

set(API_FIG_FILE "${CMAKE_SOURCE_DIR}/figura/tsapi.fig")
set(API_PROTO_GENERATED "${API_PROTO_OUT}/api.ts")

set(IPC_FIG_FILE "${CMAKE_SOURCE_DIR}/figura/ipc.fig")
set(IPC_PROTO_OUT "${EXT_API_SRC_DIR}/src/proto")
set(IPC_PROTO_GENERATED "${IPC_PROTO_OUT}/ipc.ts")

file(GLOB_RECURSE API_TS_FILES
    "${EXT_API_SRC_DIR}/src/api/**/*"
    "${EXT_API_SRC_DIR}/src/api/*"
)

set(EXT_API_TS_FILES)

foreach(file ${API_TS_FILES})
    if(NOT file MATCHES ".*proto.*")
		list(APPEND EXT_API_TS_FILES "${file}")
    endif()
endforeach()

file(MAKE_DIRECTORY ${API_PROTO_OUT})
file(MAKE_DIRECTORY ${IPC_PROTO_OUT})

set(API_STAMP "${CMAKE_CURRENT_BINARY_DIR}/api.stamp")

if (INSTALL_NODE_MODULES)
	add_custom_command(
		OUTPUT ${API_NODE_MODULES}
		COMMAND npm ci
		WORKING_DIRECTORY ${EXT_API_SRC_DIR}
		COMMENT "Install API node_modules"
	)
endif()

# Step 1: generate TS protos from .fig + figura binary
add_custom_command(
    OUTPUT ${API_PROTO_GENERATED}
    COMMAND ${FIGURA_CC} compile ${API_FIG_FILE} --client typescript --output ${API_PROTO_GENERATED}
    DEPENDS ${FIGURA_CC} ${API_FIG_FILE}
    COMMENT "Figura codegen: API client (typescript)"
)

add_custom_command(
    OUTPUT ${IPC_PROTO_GENERATED}
    COMMAND ${FIGURA_CC} compile ${IPC_FIG_FILE} --client typescript --output ${IPC_PROTO_GENERATED}
    DEPENDS ${FIGURA_CC} ${IPC_FIG_FILE}
    COMMENT "Figura codegen: IPC client (typescript)"
)

# Step 2: build the API package (depends on generated protos + source files)
add_custom_command(
    OUTPUT ${API_STAMP}
    COMMAND npm run build
    COMMAND ${CMAKE_COMMAND} -E touch ${API_STAMP}
    WORKING_DIRECTORY ${EXT_API_SRC_DIR}
    DEPENDS ${API_PROTO_GENERATED} ${IPC_PROTO_GENERATED} ${EXT_API_TS_FILES} ${API_NODE_MODULES}
    COMMENT "Build API package"
)

add_custom_target(api DEPENDS ${API_STAMP})

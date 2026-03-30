include(cmake/Api.cmake)
include(cmake/RaycastCompat.cmake)
include("Figura")

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(Protobuf)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)

set(EXT_MGR_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extension-manager")
set(EXT_MGR_OUT "${CMAKE_BINARY_DIR}/extension-runtime.js")
set(EXT_PROTO_OUT "${EXT_MGR_SRC_DIR}/src/proto")
set(EXT_MGR_NODE_MODULES "${EXT_MGR_SRC_DIR}/node_modules")

file(GLOB_RECURSE EXT_MGR_TS_FILES
    "${EXT_MGR_SRC_DIR}/src/*.ts"
)

file(GLOB_RECURSE EXT_PROTO_FILES
	"${PROTO_DIR}/*.proto"
)

list(APPEND EXT_MGR_TS_FILES
    "${EXT_MGR_SRC_DIR}/tsconfig.json"
    "${EXT_MGR_SRC_DIR}/package.json"
)

set(FILTERED_TS_FILES "")

foreach(file ${EXT_MGR_TS_FILES})
    if(NOT file MATCHES "/proto/")
		list(APPEND FILTERED_TS_FILES "${file}")
    endif()
endforeach()

file(MAKE_DIRECTORY ${EXT_PROTO_OUT})

if (INSTALL_NODE_MODULES)
	add_custom_command(
		OUTPUT ${EXT_MGR_NODE_MODULES}
		COMMAND npm install
		WORKING_DIRECTORY ${EXT_MGR_SRC_DIR}
		COMMENT "Installing extension manager node_modules"
	)
endif()

add_custom_command(
    OUTPUT ${EXT_MGR_OUT}
	COMMAND  ${FIGURA_CC} compile ${CMAKE_SOURCE_DIR}/figura/manager.fig --server typescript --output ${EXT_PROTO_OUT}/manager.ts
	COMMAND  ${FIGURA_CC} compile ${CMAKE_SOURCE_DIR}/figura/manager-extension.fig --client typescript --output ${EXT_PROTO_OUT}/manager-extension.ts
	COMMAND  ${FIGURA_CC} compile ${CMAKE_SOURCE_DIR}/figura/manager-extension.fig --server typescript --output ${EXT_PROTO_OUT}/extension-manager.ts
	COMMAND  ${CMAKE_BINARY_DIR}/bin/figura compile ${CMAKE_SOURCE_DIR}/figura/tsapi.fig --client typescript --output ${EXT_PROTO_OUT}/api.ts
    COMMAND npm run build
    COMMAND ${CMAKE_COMMAND} -E copy
            ${EXT_MGR_SRC_DIR}/dist/runtime.js
            ${EXT_MGR_OUT}
    WORKING_DIRECTORY ${EXT_MGR_SRC_DIR}
	DEPENDS ${FIGURA_CC} ${CMAKE_SOURCE_DIR}/figura/tsapi.fig ${FILTERED_TS_FILES} ${API_STAMP} ${RAY_COMPAT_STAMP} ${EXT_MGR_NODE_MODULES}
    COMMENT "Building extension manager JavaScript bundle"
)

add_custom_target(extension-manager ALL
	DEPENDS ${EXT_MGR_OUT}
)


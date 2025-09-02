set(EXT_API_SRC_DIR "${CMAKE_SOURCE_DIR}/api")
set(EXT_API_OUT_DIR "${CMAKE_SOURCE_DIR}/api/dist")
set(API_DIST_DIR "${CMAKE_SOURCE_DIR}/api/dist")
set(API_PROTO_PATH "${CMAKE_SOURCE_DIR}/proto/extensions")
set(API_PROTO_OUT "${EXT_API_SRC_DIR}/src/proto")

message(STATUS ${API_PROTO_PATH})

file(GLOB_RECURSE API_PROTO_FILES
	"${API_PROTO_PATH}/*.proto"
)

file(GLOB_RECURSE API_TS_FILES
    "${EXT_API_SRC_DIR}/src/api/**/*"
)

set(EXT_API_TS_FILES)

foreach(file ${API_TS_FILES})
    if(NOT file MATCHES ".*proto.*")
		list(APPEND EXT_API_TS_FILES "${file}")
    endif()
endforeach()

file(MAKE_DIRECTORY ${API_PROTO_OUT})

set(API_STAMP "${CMAKE_CURRENT_BINARY_DIR}/api.stamp")

#foreach(file ${EXT_API_TS_FILES})
#    file(RELATIVE_PATH rel_file "${EXT_API_SRC_DIR}" "${file}")
#    message(STATUS "  ${rel_file}")
#endforeach()

add_custom_command(
    OUTPUT ${API_STAMP}
    COMMAND npm install
    COMMAND protobuf::protoc --plugin=./node_modules/.bin/protoc-gen-ts_proto -I ${protobuf_SOURCE_DIR}/src -I ${API_PROTO_PATH} ${API_PROTO_FILES} --ts_proto_out ${API_PROTO_OUT}
    COMMAND npm run build
    COMMAND ${CMAKE_COMMAND} -E touch ${API_STAMP}
    WORKING_DIRECTORY ${EXT_API_SRC_DIR}
    DEPENDS ${EXT_API_TS_FILES}
    COMMENT "Build API"
)

add_custom_target(api DEPENDS ${API_STAMP})

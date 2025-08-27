# cmake/ExtensionApi.cmake

set(EXT_API_SRC_DIR "${CMAKE_SOURCE_DIR}/api")
set(EXT_API_OUT_DIR "${CMAKE_SOURCE_DIR}/api/dist")
set(API_DIST_DIR "${CMAKE_SOURCE_DIR}/api/dist")
set(API_PROTO_PATH "${CMAKE_SOURCE_DIR}/proto/extensions")
set(API_PROTO_OUT "${EXT_API_SRC_DIR}/src/proto")

message(STATUS ${API_PROTO_PATH})

file(GLOB_RECURSE API_PROTO_FILES
	"${API_PROTO_PATH}/*.proto"
)

file(GLOB_RECURSE EXT_API_TS_FILES
    "${EXT_API_SRC_DIR}/src/*.ts"
)

set(FILTERED_TS_FILES "")

foreach(file ${EXT_API_TS_FILES})
    if(NOT file MATCHES "/proto/")
		list(APPEND FILTERED_TS_FILES "${file}")
    endif()
endforeach()

set(API_OUT "")

foreach(ts_file IN LISTS FILTERED_TS_FILES)
    file(RELATIVE_PATH rel_path "${EXT_API_SRC_DIR}/src" "${ts_file}")
    string(REPLACE ".ts" ".js" js_path "${rel_path}")
    list(APPEND API_OUT "${EXT_API_OUT_DIR}/${js_path}")
endforeach()

file(MAKE_DIRECTORY ${API_PROTO_OUT})

add_custom_command(
	OUTPUT ${API_OUT}
    COMMAND npm install
	COMMAND protobuf::protoc --plugin=./node_modules/.bin/protoc-gen-ts_proto -I ${protobuf_SOURCE_DIR}/src -I ${API_PROTO_PATH} ${API_PROTO_FILES} --ts_proto_out ${API_PROTO_OUT}
	COMMAND npm run build
    WORKING_DIRECTORY ${EXT_API_SRC_DIR}
	DEPENDS ${FILTERED_TS_FILES}
	COMMENT "Build API"
)

add_custom_target(api DEPENDS ${API_OUT})

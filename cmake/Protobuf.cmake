include(FetchContent)

function(checkout_protobuf)
	set(FETCHCONTENT_QUIET OFF)
	FetchContent_Declare(
	  protobuf
	  EXCLUDE_FROM_ALL
	  GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
	  GIT_TAG        v32.0  #
	)

	set(protobuf_INSTALL OFF)
	set(protobuf_BUILD_TESTS OFF)
	set(protobuf_BUILD_PROTOBUF_BINARIES ON)
	set(protobuf_BUILD_PROTOC_BINARIES ON)
	set(protobuf_BUILD_SHARED_LIBS OFF)
	FetchContent_MakeAvailable(protobuf)
endfunction()


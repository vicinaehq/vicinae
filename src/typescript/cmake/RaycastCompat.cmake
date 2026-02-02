# The only goal of this is to trigger an extension manager rebuild if any of its files
# change.

set(RAY_COMPAT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raycast-api-compat")

file(GLOB_RECURSE RAY_COMPAT_TS_FILES
	"${RAY_COMPAT_SRC_DIR}/src/**/*"
	"${RAY_COMPAT_SRC_DIR}/src/*"
)

set(RAY_COMPAT_STAMP "${CMAKE_CURRENT_BINARY_DIR}/ray-compat.stamp")

add_custom_command(
	OUTPUT ${RAY_COMPAT_STAMP}
	COMMAND ${CMAKE_COMMAND} -E touch ${RAY_COMPAT_STAMP}
	WORKING_DIRECTORY ${RAY_COMPAT_SRC_DIR}
	DEPENDS ${RAY_COMPAT_TS_FILES}
)

add_custom_target(raycast-compat DEPENDS ${RAY_COMPAT_STAMP})

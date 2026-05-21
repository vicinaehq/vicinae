# opts:
#   FFF_VERSION            (string)                  - release tag / git ref.
#   FFF_LIBC               (glibc|musl|auto)         - only for prebuilt Linux.
#   FFF_BUILD_FROM_SOURCE  (BOOL, default OFF)       - cargo build fff locally.
#   FFF_CARGO_FEATURES     (string, default "")       - comma list for --features.
#   FFF_CARGO_PROFILE      (release|dev, default release)
#
# outputs of ${CMAKE_BINARY_DIR}/_fff:
#   _fff/lib/libfff_c.<ext>                 (prebuilt mode)
#   _fff/include/fff.h                      (prebuilt mode)
#   _fff/.stamp-<FFF_VERSION>-<triple>      (prebuilt cache invalidator)
#   _fff/src/                               (source-build clone)
#   _fff/cargo/<profile>/libfff_c.<ext>     (source-build output)

set(FFF_VERSION "v0.8.1" CACHE STRING "fff release tag / git ref to use")
set(FFF_LIBC "auto" CACHE STRING "Linux C library variant for fff: glibc | musl | auto")
set_property(CACHE FFF_LIBC PROPERTY STRINGS "auto" "glibc" "musl")
option(FFF_BUILD_FROM_SOURCE "Build libfff_c locally with cargo (requires Rust toolchain)" OFF)
set(FFF_CARGO_FEATURES "" CACHE STRING "fff feature flags, provide 'zlob' if you have zig toolchain installed")
set(FFF_CARGO_PROFILE "release" CACHE STRING "Cargo profile for fff-c (release | dev)")
set_property(CACHE FFF_CARGO_PROFILE PROPERTY STRINGS "release" "dev")

function(_fff_detect_libc out_libc)
	# probe ldd --version if output 
	execute_process(
		COMMAND ldd --version
		OUTPUT_VARIABLE _ldd_out
		ERROR_VARIABLE _ldd_err
		TIMEOUT 5)

	if ("${_ldd_out}${_ldd_err}" MATCHES "musl")
		set(${out_libc} "musl" PARENT_SCOPE)
	else()
		set(${out_libc} "glibc" PARENT_SCOPE)
	endif()
endfunction()

function(_fff_detect_triple out_triple out_ext)
	# Normalize processor
	set(_proc "${CMAKE_SYSTEM_PROCESSOR}")
	if (_proc STREQUAL "AMD64" OR _proc STREQUAL "x64")
		set(_proc "x86_64")
	elseif (_proc STREQUAL "arm64")
		set(_proc "aarch64")
	endif()

	if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
		set(_libc "${FFF_LIBC}")
		if (_libc STREQUAL "auto")
			_fff_detect_libc(_libc)
			message(STATUS "fff: auto-detected libc=${_libc}")
		endif()

		if (_libc STREQUAL "musl")
			set(${out_triple} "${_proc}-unknown-linux-musl" PARENT_SCOPE)
		else()
			set(${out_triple} "${_proc}-unknown-linux-gnu" PARENT_SCOPE)
		endif()
		set(${out_ext} "so" PARENT_SCOPE)
	elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin") # is vicinae even compiled for macos? would be fun lol
		set(${out_triple} "${_proc}-apple-darwin" PARENT_SCOPE)
		set(${out_ext} "dylib" PARENT_SCOPE)
	else()
		message(FATAL_ERROR "fff does not publish a prebuilt C library for ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}. Build from source with -DFFF_BUILD_FROM_SOURCE=ON.")
	endif()
endfunction()

function(_fff_download url dst)
	message(STATUS "fff: fetching ${url}")
	file(DOWNLOAD "${url}" "${dst}"
		TLS_VERIFY ON
		STATUS _status)
	list(GET _status 0 _rc)
	if (NOT _rc EQUAL 0)
		list(GET _status 1 _err)
		message(FATAL_ERROR "fff: failed to download ${url}: ${_err}")
	endif()
endfunction()

function(_fff_configure_prebuilt)
	_fff_detect_triple(_triple _ext)

	set(_root "${CMAKE_BINARY_DIR}/_fff")
	set(_lib_dir "${_root}/lib")
	set(_inc_dir "${_root}/include")
	set(_libfile "${_lib_dir}/libfff_c.${_ext}")
	set(_hdrfile "${_inc_dir}/fff.h")
	set(_stamp "${_root}/.stamp-${FFF_VERSION}-${_triple}")

	if (NOT EXISTS "${_stamp}")
		# Version or triple changed. Wipe any prior prebuilt cache.
		if (EXISTS "${_lib_dir}")
			file(REMOVE_RECURSE "${_lib_dir}")
		endif()
		if (EXISTS "${_inc_dir}")
			file(REMOVE_RECURSE "${_inc_dir}")
		endif()
		file(GLOB _old_stamps "${_root}/.stamp-*")
		if (_old_stamps)
			file(REMOVE ${_old_stamps})
		endif()
		file(MAKE_DIRECTORY "${_lib_dir}" "${_inc_dir}")

		set(_base "https://github.com/dmtrKovalenko/fff/releases/download/${FFF_VERSION}")
		set(_asset "c-lib-${_triple}.${_ext}")
		_fff_download("${_base}/${_asset}" "${_libfile}")
		_fff_download(
			"https://raw.githubusercontent.com/dmtrKovalenko/fff/${FFF_VERSION}/crates/fff-c/include/fff.h"
			"${_hdrfile}")

		file(WRITE "${_stamp}" "${FFF_VERSION} ${_triple}\n")
	endif()

	add_library(vicinae::fff SHARED IMPORTED GLOBAL)
	set_target_properties(vicinae::fff PROPERTIES
		IMPORTED_LOCATION "${_libfile}"
		IMPORTED_NO_SONAME TRUE
		INTERFACE_INCLUDE_DIRECTORIES "${_inc_dir}")

	set(FFF_RUNTIME_LIBRARY "${_libfile}" CACHE INTERNAL "" FORCE)
	message(STATUS "fff: prebuilt ${FFF_VERSION} (${_triple}) at ${_libfile}")
endfunction()

function(_fff_configure_source)
	find_program(CARGO cargo)
	if (NOT CARGO)
		message(FATAL_ERROR
			"fff: cargo not found but -DFFF_BUILD_FROM_SOURCE=ON was requested.\n"
			"Install the Rust toolchain (https://rustup.rs) or unset "
			"-DFFF_BUILD_FROM_SOURCE to use the prebuilt binary.")
	endif()

	# Library extension still comes from the triple detector; we only use the
	# extension part for source builds.
	_fff_detect_triple(_ignore_triple _ext)

	include(FetchContent)
	FetchContent_Declare(
		fff_src
		GIT_REPOSITORY https://github.com/dmtrKovalenko/fff.git
		GIT_TAG ${FFF_VERSION}
		GIT_SHALLOW TRUE
		EXCLUDE_FROM_ALL
		SOURCE_DIR "${CMAKE_BINARY_DIR}/_fff/src"
	)

	# do not invoke subdir becuase fff is a rust project without cmake
	FetchContent_GetProperties(fff_src)
	if (NOT fff_src_POPULATED)
		message(STATUS "fff: cloning source tree ${FFF_VERSION}")
		# FetchContent_Populate is deprecated in 3.30+ but still the supported
		# way to populate without add_subdirectory. Quiet the warning.
		if (POLICY CMP0169)
			cmake_policy(PUSH)
			cmake_policy(SET CMP0169 OLD)
		endif()
		FetchContent_Populate(fff_src)
		if (POLICY CMP0169)
			cmake_policy(POP)
		endif()
	endif()

	set(_src_dir "${fff_src_SOURCE_DIR}")
	set(_cargo_dir "${CMAKE_BINARY_DIR}/_fff/cargo")
	set(_profile_dir_name "${FFF_CARGO_PROFILE}")
	if (FFF_CARGO_PROFILE STREQUAL "dev")
		# cargo's `dev` profile outputs into `debug/`.
		set(_profile_dir_name "debug")
	endif()
	set(_libfile "${_cargo_dir}/${_profile_dir_name}/libfff_c.${_ext}")
	set(_hdrdir "${_src_dir}/crates/fff-c/include")

	set(_cargo_args build -p fff-c
		--manifest-path "${_src_dir}/Cargo.toml"
		--target-dir "${_cargo_dir}")

	if (FFF_CARGO_PROFILE STREQUAL "release")
		list(APPEND _cargo_args --release)
	elseif (NOT FFF_CARGO_PROFILE STREQUAL "dev")
		message(FATAL_ERROR "fff: FFF_CARGO_PROFILE must be 'release' or 'dev' (got '${FFF_CARGO_PROFILE}')")
	endif()

	if (FFF_CARGO_FEATURES)
		string(REPLACE " " "," _features "${FFF_CARGO_FEATURES}")
		list(APPEND _cargo_args --features "${_features}")
	endif()

	# cargo manages it's own compilation, so we invalicate it on version or rust code change
	file(GLOB_RECURSE _fff_src_glob
		CONFIGURE_DEPENDS
		"${_src_dir}/crates/fff-c/src/*.rs"
		"${_src_dir}/crates/fff-c/build.rs"
		"${_src_dir}/crates/fff-c/Cargo.toml")

	add_custom_command(
		OUTPUT "${_libfile}"
		COMMAND ${CARGO} ${_cargo_args}
		WORKING_DIRECTORY "${_src_dir}"
		DEPENDS
			"${_src_dir}/Cargo.toml"
			"${_src_dir}/Cargo.lock"
			${_fff_src_glob}
		COMMENT "fff: cargo build (${FFF_CARGO_PROFILE}, features=${FFF_CARGO_FEATURES})"
		VERBATIM
		USES_TERMINAL)

	add_custom_target(fff_c_build ALL DEPENDS "${_libfile}")

	add_library(vicinae::fff SHARED IMPORTED GLOBAL)
	set_target_properties(vicinae::fff PROPERTIES
		IMPORTED_LOCATION "${_libfile}"
		IMPORTED_NO_SONAME TRUE
		INTERFACE_INCLUDE_DIRECTORIES "${_hdrdir}")
	add_dependencies(vicinae::fff fff_c_build)

	set(FFF_RUNTIME_LIBRARY "${_libfile}" CACHE INTERNAL "" FORCE)
	message(STATUS "fff: building from source (${FFF_VERSION}, profile=${FFF_CARGO_PROFILE}, features=${FFF_CARGO_FEATURES}) -> ${_libfile}")
endfunction()

function(fff_configure)
	if (TARGET vicinae::fff)
		return()
	endif()

	if (FFF_BUILD_FROM_SOURCE)
		_fff_configure_source()
	else()
		_fff_configure_prebuilt()
	endif()
endfunction()

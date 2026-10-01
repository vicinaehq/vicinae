include(FetchContent)

# Strips glaze's unconditional defaulted tuple comparisons; without this, structs holding
# vector<variant<incomparable...>> fail to compile under MSVC's unconstrained std::variant.
# https://github.com/stephenberry/glaze/pull/2101
set(GLAZE_MSVC_VARIANT_PATCH "${CMAKE_CURRENT_LIST_DIR}/patches/glaze-msvc-variant.patch")

function(import_glaze)
	set(FETCHCONTENT_QUIET OFF)
	set(_glaze_patch "")
	if (MSVC)
		set(_glaze_patch PATCH_COMMAND git checkout -- . COMMAND git apply --ignore-whitespace "${GLAZE_MSVC_VARIANT_PATCH}")
	endif()
	FetchContent_Declare(
	  glaze
	  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
	  GIT_TAG v7.2.0
	  GIT_SHALLOW TRUE
	  EXCLUDE_FROM_ALL
	  OVERRIDE_FIND_PACKAGE
	  ${_glaze_patch}
	)
	FetchContent_MakeAvailable(glaze)
endfunction()

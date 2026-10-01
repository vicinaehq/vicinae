function(get_git_commit OUT_COMMIT)
    # Get commit hash
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _git_commit
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_VARIABLE _git_error
    )
    if(_git_commit STREQUAL "")
        string(STRIP "${_git_error}" _git_error)
        message(WARNING "Could not determine git commit, stamping 'unknown'. Pass -DVICINAE_GIT_COMMIT_HASH to override. ${_git_error}")
        set(_git_commit "unknown")
    endif()

    # Export variables to parent scope
    set(${OUT_COMMIT} "${_git_commit}" PARENT_SCOPE)
endfunction()

function(get_git_tag OUT_TAG)
	execute_process(
        COMMAND git describe --tags --abbrev=0
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _git_tag
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_VARIABLE _git_error
    )
    if(_git_tag STREQUAL "")
        string(STRIP "${_git_error}" _git_error)
        message(WARNING "Could not determine git tag, stamping 'v0.0.0'. Pass -DVICINAE_GIT_TAG to override. ${_git_error}")
        set(_git_tag "v0.0.0")
    endif()

	set(${OUT_TAG} "${_git_tag}" PARENT_SCOPE)
endfunction()

function(get_git_commit_count OUT_COUNT)
    execute_process(
        COMMAND git rev-list --count HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE _git_count
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(_git_count STREQUAL "")
        set(_git_count "0")
    endif()

    set(${OUT_COUNT} "${_git_count}" PARENT_SCOPE)
endfunction()

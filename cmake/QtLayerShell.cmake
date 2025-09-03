include(FetchContent)

function(checkout_layer_shell_qt)
	set(FETCHCONTENT_QUIET OFF)

	FetchContent_Declare(
	  layer-shell-qt
	  #EXCLUDE_FROM_ALL
	  GIT_REPOSITORY https://github.com/vicinaehq/layer-shell-qt
	)

	set(BUILD_SHARED_LIBS OFF)
	set(LAYER_SHELL_QT_DECLARATIVE OFF)
	FetchContent_MakeAvailable(layer-shell-qt)
endfunction()

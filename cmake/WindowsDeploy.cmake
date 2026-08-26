function(vicinae_windows_deploy target)
	set_target_properties(${target} PROPERTIES WIN32_EXECUTABLE ON)

	if (VICINAE_GIT_TAG MATCHES "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+)")
		set(VICINAE_RC_MAJOR ${CMAKE_MATCH_1})
		set(VICINAE_RC_MINOR ${CMAKE_MATCH_2})
		set(VICINAE_RC_PATCH ${CMAKE_MATCH_3})
	else()
		set(VICINAE_RC_MAJOR 0)
		set(VICINAE_RC_MINOR 0)
		set(VICINAE_RC_PATCH 0)
	endif()
	set(VICINAE_WIN_ICO "${CMAKE_SOURCE_DIR}/extra/windows/vicinae.ico")
	configure_file(
		${CMAKE_SOURCE_DIR}/extra/windows/vicinae.rc.in
		${CMAKE_BINARY_DIR}/generated/vicinae.rc
		@ONLY
	)
	target_sources(${target} PRIVATE ${CMAKE_BINARY_DIR}/generated/vicinae.rc)

	install(TARGETS ${target} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	file(RELATIVE_PATH VICINAE_DATA_ROOT_REL
		"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}" "${CMAKE_INSTALL_PREFIX}")
	target_compile_definitions(${target} PRIVATE VICINAE_DATA_ROOT_REL="${VICINAE_DATA_ROOT_REL}")
	qt_generate_deploy_qml_app_script(
		TARGET ${target}
		OUTPUT_SCRIPT VICINAE_QT_DEPLOY_SCRIPT
		NO_TRANSLATIONS
		NO_COMPILER_RUNTIME
		DEPLOY_TOOL_OPTIONS
			--no-opengl-sw
			--no-system-d3d-compiler
			--no-system-dxc-compiler
			--skip-plugin-types qmltooling
	)
	install(SCRIPT ${VICINAE_QT_DEPLOY_SCRIPT})

	install(CODE [[
		foreach(style Fusion Imagine Material Universal FluentWinUI3 Windows)
			file(REMOVE
				"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Qt6QuickControls2${style}.dll"
				"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Qt6QuickControls2${style}StyleImpl.dll")
			file(REMOVE_RECURSE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/qml/QtQuick/Controls/${style}")
		endforeach()
		file(REMOVE_RECURSE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/qml/QtQuick/NativeStyle")
	]])
endfunction()

cmake_minimum_required(VERSION 3.24)
project(VicinaeDocument VERSION 0.1.0 LANGUAGES C CXX)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick QuickControls2 Concurrent Svg Widgets Xml GLOBAL)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
include(cmake/Dependencies.cmake)

set(DOCUMENT_HEADERS
	include/document/document-controller.hpp
	include/document/document-highlighter.hpp
	include/document/document-layout.hpp
	include/document/document-model.hpp
	include/document/document-scope.hpp
	include/document/document-search-highlighter.hpp
	include/document/document-search.hpp
	include/document/document-selection.hpp
	include/document/document-style.hpp
	include/document/document-table-model.hpp
	include/document/document-text-images.hpp
	include/document/index-model.hpp
	include/document/markdown-document.hpp
	include/document/markdown-model.hpp
	include/document/math-renderer.hpp
	include/document/text-document-edit.hpp
)
set(DOCUMENT_SOURCES
	src/document-controller.cpp
	src/document-highlighter.cpp
	src/document-layout.cpp
	src/document-scope.cpp
	src/document-search-highlighter.cpp
	src/document-search.cpp
	src/document-selection.cpp
	src/document-style.cpp
	src/document-table-model.cpp
	src/document-text-images.cpp
	src/markdown-document.cpp
	src/markdown-math.cpp
	src/markdown-math.hpp
	src/markdown-parser.cpp
	src/markdown-parser.hpp
	src/markdown-model.cpp
	src/math-renderer.cpp
	src/syntax-highlighter.cpp
	src/syntax-highlighter.hpp
	src/text-document-edit.cpp
)
set(DOCUMENT_QML
	qml/Document.qml
	qml/DocumentScrollBar.qml
	qml/DocumentSelectionMenu.qml
	qml/DocumentText.qml
	qml/DocumentView.qml
	qml/MarkdownBlock.qml
	qml/MarkdownInline.qml
	qml/MarkdownText.qml
	qml/MarkdownView.qml
	qml/MdBlockquote.qml
	qml/MdCallout.qml
	qml/MdCodeBlock.qml
	qml/MdHeading.qml
	qml/MdHorizontalRule.qml
	qml/MdHtmlBlock.qml
	qml/MdImage.qml
	qml/MdList.qml
	qml/MdMath.qml
	qml/MdParagraph.qml
	qml/MdTable.qml
)
foreach(file IN LISTS DOCUMENT_QML)
	get_filename_component(name "${file}" NAME)
	set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${name}")
endforeach()
qt_policy(SET QTP0005 NEW)
qt_add_qml_module(vicinae-document
	STATIC
	PLUGIN_TARGET vicinae-document
	URI Vicinae.Documents
	VERSION 1.0
	OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Vicinae/Documents"
	RESOURCE_PREFIX /qt/qml
	QML_FILES ${DOCUMENT_QML}
	SOURCES ${DOCUMENT_HEADERS} ${DOCUMENT_SOURCES}
	DEPENDENCIES QtQuick QtQuick.Controls QtQuick.Shapes TARGET vicinae-wheelhandler
)
add_library(vicinae::document ALIAS vicinae-document)
target_include_directories(vicinae-document PUBLIC include PRIVATE include/document src)
target_compile_features(vicinae-document PUBLIC cxx_std_23)
target_link_libraries(vicinae-document
	PUBLIC Qt6::Quick Qt6::QuickControls2
	PRIVATE vicinae::wheelhandler Qt6::Concurrent Qt6::Svg ${DOCUMENT_CMARK} ${DOCUMENT_PUGIXML} KF6::SyntaxHighlighting JKQTMathText6
)

FetchContent_GetProperties(jkqtmathtext SOURCE_DIR MATHTEXT_SOURCE_DIR)
if (MATHTEXT_SOURCE_DIR)
	set_source_files_properties("${MATHTEXT_SOURCE_DIR}/LICENSE" PROPERTIES QT_RESOURCE_ALIAS "JKQTMathText.txt")
	set_source_files_properties("${MATHTEXT_SOURCE_DIR}/lib/jkqtmathtext/resources/xits/OFL.txt" PROPERTIES QT_RESOURCE_ALIAS "XITS.txt")
	qt_add_resources(vicinae-document math_licenses
		PREFIX "/licenses"
		FILES "${MATHTEXT_SOURCE_DIR}/LICENSE" "${MATHTEXT_SOURCE_DIR}/lib/jkqtmathtext/resources/xits/OFL.txt"
	)
endif()

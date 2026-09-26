cmake_minimum_required(VERSION 3.24)
project(QmlDocumentExample LANGUAGES C CXX)

find_package(Qt6 6.9 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.9)
add_subdirectory(../../../../../vendor/kirigami-wheelhandler scrolling)
add_subdirectory(../.. document)

qt_add_executable(qml-document-example MACOSX_BUNDLE main.cpp)
set_target_properties(qml-document-example PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER org.vicinae.document-example
	MACOSX_BUNDLE_BUNDLE_NAME "QML Document"
)
qt_add_qml_module(qml-document-example
	URI DocumentExample
	QML_FILES Main.qml BuildCard.qml
	DEPENDENCIES TARGET vicinae-document
)
target_link_libraries(qml-document-example PRIVATE vicinae::document)

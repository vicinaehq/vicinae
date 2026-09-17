cmake_minimum_required(VERSION 3.24)
project(VicinaeViewportTests LANGUAGES CXX)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick QuickControls2 QuickTest)
qt_standard_project_setup(REQUIRES 6.8)
qt_add_executable(viewport-tests main.cpp)
target_link_libraries(viewport-tests PRIVATE Qt6::QuickTest Qt6::QuickControls2)
target_compile_definitions(viewport-tests PRIVATE QUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(viewport-tests PRIVATE cxx_std_23)

set(qml_root "${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/qml")
set(repo_root "${CMAKE_CURRENT_SOURCE_DIR}/../../../..")
target_include_directories(viewport-tests PRIVATE "${qml_root}/../.."
    "${qml_root}/../quick"
    "${repo_root}/src/lib/fuzzy/include" "${repo_root}/vendor/kirigami-wheelhandler")
set(qml_files
    "${qml_root}/launcher/LauncherPage.qml"
    "${qml_root}/controls/ScrollViewport.qml"
    "${qml_root}/controls/StatusBarInset.qml"
    "${qml_root}/controls/ViciScrollBar.qml"
    "${qml_root}/form/FormView.qml"
    "${qml_root}/detail/TextViewer.qml"
    "${qml_root}/views/ScriptOutputText.qml"
    fixtures/LauncherWindow.qml
    fixtures/Theme.qml
)
foreach(file IN LISTS qml_files)
    get_filename_component(alias "${file}" NAME)
    set_source_files_properties("${file}" PROPERTIES QT_RESOURCE_ALIAS "${alias}")
endforeach()
set_source_files_properties(fixtures/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(viewport-tests URI Vicinae VERSION 1.0
    RESOURCE_PREFIX /qt/qml QML_FILES ${qml_files}
    SOURCES
        "${qml_root}/../quick/document-highlighter.hpp"
        "${qml_root}/../quick/document-highlighter.cpp"
        "${qml_root}/../quick/match-highlighter.hpp"
        "${qml_root}/../quick/match-highlighter.cpp"
        "${repo_root}/vendor/kirigami-wheelhandler/wheelhandler.h"
        "${repo_root}/vendor/kirigami-wheelhandler/wheelhandler.cpp")

enable_testing()
add_test(NAME viewport COMMAND viewport-tests)

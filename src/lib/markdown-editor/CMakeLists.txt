qt_add_library(vicinae-editor STATIC)
add_library(vicinae::editor ALIAS vicinae-editor)
qt_add_qml_module(vicinae-editor
    URI Vicinae.Editor
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Vicinae/Editor
    QML_FILES MarkdownEditor.qml SelectionToolbar.qml
    SOURCES editor-document.hpp editor-document.cpp editor-drop-area.hpp editor-drop-area.cpp syntax-palette.hpp
)
target_include_directories(vicinae-editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vicinae-editor PUBLIC Qt6::Quick KF6::SyntaxHighlighting)
target_compile_features(vicinae-editor PUBLIC cxx_std_23)

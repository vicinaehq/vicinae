add_library(everything-sdk3 STATIC src/Everything3.c)
target_include_directories(everything-sdk3 PUBLIC include)
target_compile_definitions(everything-sdk3 PRIVATE EVERYTHING3_LIB INTERFACE EVERYTHING3_USERAPI=)
# the source redefines WIN32_LEAN_AND_MEAN which we already define globally
target_compile_options(everything-sdk3 PRIVATE /wd4005)

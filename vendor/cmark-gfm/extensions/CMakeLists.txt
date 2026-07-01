set(LIBRARY "libcmark-gfm-extensions")
set(STATICLIBRARY "libcmark-gfm-extensions_static")
set(LIBRARY_SOURCES
    core-extensions.c
    table.c
    strikethrough.c
    autolink.c
    tagfilter.c
    ext_scanners.c
    ext_scanners.re
    ext_scanners.h
    tasklist.c
   )

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}/src
)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE} -pg")
set(CMAKE_LINKER_PROFILE "${CMAKE_LINKER_FLAGS_RELEASE} -pg")

if (CMARK_SHARED)
  add_library(${LIBRARY} SHARED ${LIBRARY_SOURCES})

  set_target_properties(${LIBRARY} PROPERTIES
    OUTPUT_NAME "cmark-gfm-extensions"
    DEFINE_SYMBOL "cmark-gfm"
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.gfm.${PROJECT_VERSION_GFM}
    VERSION ${PROJECT_VERSION})

  set_property(TARGET ${LIBRARY}
    APPEND PROPERTY MACOSX_RPATH true)

  # Avoid name clash between PROGRAM and LIBRARY pdb files.
  set_target_properties(${LIBRARY} PROPERTIES PDB_NAME cmark-gfm-extensions_dll)

  list(APPEND CMARK_INSTALL ${LIBRARY})
  target_link_libraries(${LIBRARY} libcmark-gfm)

endif()

if (CMARK_STATIC)
  add_library(${STATICLIBRARY} STATIC ${LIBRARY_SOURCES})

  set_target_properties(${STATICLIBRARY} PROPERTIES
    COMPILE_FLAGS "-DCMARK_GFM_STATIC_DEFINE -DCMARK_GFM_EXTENSIONS_STATIC_DEFINE"
    DEFINE_SYMBOL "cmark-gfm"
    POSITION_INDEPENDENT_CODE ON)

  if (MSVC)
    set_target_properties(${STATICLIBRARY} PROPERTIES
      OUTPUT_NAME "cmark-gfm-extensions_static"
      VERSION ${PROJECT_VERSION})
  else()
    set_target_properties(${STATICLIBRARY} PROPERTIES
      OUTPUT_NAME "cmark-gfm-extensions"
      VERSION ${PROJECT_VERSION})
  endif(MSVC)

  list(APPEND CMARK_INSTALL ${STATICLIBRARY})
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)

include (InstallRequiredSystemLibraries)
install(TARGETS ${CMARK_INSTALL}
  EXPORT cmark-gfm-extensions
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib${LIB_SUFFIX}
  ARCHIVE DESTINATION lib${LIB_SUFFIX}
  )

if (CMARK_SHARED OR CMARK_STATIC)
  install(FILES
  cmark-gfm-core-extensions.h
  DESTINATION include
  )

  install(EXPORT cmark-gfm-extensions DESTINATION lib${LIB_SUFFIX}/cmake-gfm-extensions)
endif()

# Feature tests
include(CheckIncludeFile)
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckSymbolExists)
CHECK_INCLUDE_FILE(stdbool.h HAVE_STDBOOL_H)
CHECK_C_SOURCE_COMPILES(
  "int main() { __builtin_expect(0,0); return 0; }"
  HAVE___BUILTIN_EXPECT)
CHECK_C_SOURCE_COMPILES("
  int f(void) __attribute__ (());
  int main() { return 0; }
" HAVE___ATTRIBUTE__)

# Always compile with warnings
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX /wd4706 /wd4204 /wd4221 /wd4100 /D_CRT_SECURE_NO_WARNINGS")
elseif(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -std=c99 -pedantic")
endif()

# Compile as C++ under MSVC older than 12.0
if(MSVC AND MSVC_VERSION LESS 1800)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Ubsan")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
endif()

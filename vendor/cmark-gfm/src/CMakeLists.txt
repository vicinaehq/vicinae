if(${CMAKE_VERSION} VERSION_GREATER "3.3")
    cmake_policy(SET CMP0063 NEW)
endif()

include(GNUInstallDirs)

set(LIBRARY "libcmark-gfm")
set(STATICLIBRARY "libcmark-gfm_static")
set(HEADERS
  cmark-gfm.h
  cmark-gfm-extension_api.h
  parser.h
  buffer.h
  node.h
  iterator.h
  chunk.h
  references.h
  footnotes.h
  map.h
  utf8.h
  scanners.h
  inlines.h
  houdini.h
  cmark_ctype.h
  render.h
  registry.h
  syntax_extension.h
  plugin.h
  )
set(LIBRARY_SOURCES
  cmark.c
  node.c
  iterator.c
  blocks.c
  inlines.c
  scanners.c
  scanners.re
  utf8.c
  buffer.c
  references.c
  footnotes.c
  map.c
  render.c
  man.c
  xml.c
  html.c
  commonmark.c
  plaintext.c
  latex.c
  houdini_href_e.c
  houdini_html_e.c
  houdini_html_u.c
  cmark_ctype.c
  arena.c
  linked_list.c
  syntax_extension.c
  registry.c
  plugin.c
  ${HEADERS}
  )

set(PROGRAM "cmark-gfm")
set(PROGRAM_SOURCES main.c)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR})
include_directories(
  ${PROJECT_BINARY_DIR}/extensions
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmark-gfm_version.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm_version.h)

include (GenerateExportHeader)

include("../CheckFileOffsetBits.cmake")
CHECK_FILE_OFFSET_BITS()

add_executable(${PROGRAM} ${PROGRAM_SOURCES})

if(CMARK_SHARED)
  target_link_libraries(${PROGRAM} libcmark-gfm-extensions libcmark-gfm)
elseif(CMARK_STATIC)
  target_link_libraries(${PROGRAM} libcmark-gfm-extensions_static libcmark-gfm_static)
endif()

# Disable the PUBLIC declarations when compiling the executable:
set_target_properties(${PROGRAM} PROPERTIES
  COMPILE_FLAGS "-DCMARK_GFM_STATIC_DEFINE -DCMARK_GFM_EXTENSIONS_STATIC_DEFINE")

# Check integrity of node structure when compiled as debug:
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCMARK_DEBUG_NODES -DDEBUG")
set(CMAKE_LINKER_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG}")

set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE} -pg")
set(CMAKE_LINKER_PROFILE "${CMAKE_LINKER_FLAGS_RELEASE} -pg")

# -fvisibility=hidden
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if (CMARK_SHARED)
  add_library(${LIBRARY} SHARED ${LIBRARY_SOURCES})
  # Include minor version and patch level in soname for now.
  set_target_properties(${LIBRARY} PROPERTIES
    OUTPUT_NAME "cmark-gfm"
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.gfm.${PROJECT_VERSION_GFM}
    VERSION ${PROJECT_VERSION})

  set_property(TARGET ${LIBRARY}
    APPEND PROPERTY MACOSX_RPATH true)

  # Avoid name clash between PROGRAM and LIBRARY pdb files.
  set_target_properties(${LIBRARY} PROPERTIES PDB_NAME cmark-gfm_dll)

  generate_export_header(${LIBRARY}
    BASE_NAME ${PROJECT_NAME})

  list(APPEND CMARK_INSTALL ${LIBRARY})
endif()

if (CMARK_STATIC)
  add_library(${STATICLIBRARY} STATIC ${LIBRARY_SOURCES})
  set_target_properties(${STATICLIBRARY} PROPERTIES
    COMPILE_FLAGS -DCMARK_GFM_STATIC_DEFINE
    POSITION_INDEPENDENT_CODE ON)

  if (MSVC)
    set_target_properties(${STATICLIBRARY} PROPERTIES
      OUTPUT_NAME "cmark-gfm_static"
      VERSION ${PROJECT_VERSION})
  else()
    set_target_properties(${STATICLIBRARY} PROPERTIES
      OUTPUT_NAME "cmark-gfm"
      VERSION ${PROJECT_VERSION})
  endif(MSVC)

  if (NOT CMARK_SHARED)
    generate_export_header(${STATICLIBRARY}
      BASE_NAME ${PROJECT_NAME})
  endif()

  list(APPEND CMARK_INSTALL ${STATICLIBRARY})
endif()

if (MSVC)
  set_property(TARGET ${PROGRAM}
    APPEND PROPERTY LINK_FLAGS /INCREMENTAL:NO)
endif(MSVC)

if(NOT MSVC OR CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  include(InstallRequiredSystemLibraries)
endif()

set(libdir lib${LIB_SUFFIX})

install(TARGETS ${PROGRAM} ${CMARK_INSTALL}
  EXPORT cmark-gfm
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${libdir}
  ARCHIVE DESTINATION ${libdir}
  )

if(CMARK_SHARED OR CMARK_STATIC)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libcmark-gfm.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/libcmark-gfm.pc @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcmark-gfm.pc
    DESTINATION ${libdir}/pkgconfig)

  install(FILES
    cmark-gfm.h
    cmark-gfm-extension_api.h
    ${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm_version.h
    DESTINATION include
    )

  install(EXPORT cmark-gfm DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)
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

CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

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

if(CMARK_LIB_FUZZER)
  set(FUZZ_HARNESS "cmark-fuzz")
  add_executable(${FUZZ_HARNESS} ../test/cmark-fuzz.c ${LIBRARY_SOURCES})
  target_link_libraries(${FUZZ_HARNESS} "${CMAKE_LIB_FUZZER_PATH}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize-coverage=trace-pc-guard")

  # cmark is written in C but the libFuzzer runtime is written in C++ which
  # needs to link against the C++ runtime. Explicitly link it into cmark-fuzz
  set_target_properties(${FUZZ_HARNESS} PROPERTIES LINK_FLAGS "-lstdc++")
endif()

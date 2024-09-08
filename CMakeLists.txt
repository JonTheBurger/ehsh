################################################################################
# Project
################################################################################
cmake_minimum_required(VERSION 3.23...3.28)
set(PROJECT_VERSION 0.1.0) # Make this easy to grep for
project(ehsh
  VERSION      ${PROJECT_VERSION}
  DESCRIPTION  "World's okay-est shell for embedded systems"
  HOMEPAGE_URL "https://github.com/JonTheBurger/ehsh"
  LANGUAGES    C
)
include(GNUInstallDirs)

option(EHSH_BUILD_TESTS    "Build tests"         ${PROJECT_IS_TOP_LEVEL})
option(EHSH_BUILD_EXAMPLES "Build examples"      ${PROJECT_IS_TOP_LEVEL})
option(EHSH_BUILD_WARNINGS "Build with warnings" ${PROJECT_IS_TOP_LEVEL})

if (PROJECT_IS_TOP_LEVEL)
  set(CMAKE_COLOR_DIAGNOSTICS        ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
endif()
set(CPACK_PACKAGE_VENDOR    jontheburger)
set(CPACK_PACKAGE_DIRECTORY "${PROJECT_BINARY_DIR}/dist")
set(CPACK_PACKAGE_CHECKSUM  SHA256)

################################################################################
# Library
################################################################################
add_library(ehsh)
add_library(ehsh::ehsh ALIAS ehsh)
target_sources(ehsh
  PRIVATE
    src/ehsh.c
  PUBLIC
    FILE_SET ehsh
    TYPE HEADERS
    BASE_DIRS
      src
    FILES
      src/ehsh/ehsh.h
      src/ehsh/platform/eh.linux.h
      src/ehsh/platform/eh.stdc.h
      src/ehsh/platform/eh.win32.h
)
target_compile_options(ehsh
  PRIVATE
    # Warnings
    $<$<C_COMPILER_ID:GNU>:-Wall -Wextra -pedantic -Wcast-align -Werror=return-type>
    $<$<C_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
    $<$<C_COMPILER_ID:MSVC>:/W4 /permissive->
)
set_target_properties(
  ehsh
  PROPERTIES
    C_STANDARD 99
    VERSION    ${PROJECT_VERSION}
    SOVERSION  ${PROJECT_VERSION_MAJOR}
)
install(
  TARGETS  ehsh
  EXPORT   ehsh
  FILE_SET ehsh
)
install(
  EXPORT      ehsh
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/ehsh"
  NAMESPACE   ehsh::
)

################################################################################
# Tests
################################################################################
if (EHSH_BUILD_TESTS)
  enable_testing()
  add_executable(unit test/unit.c)
  target_link_libraries(unit
    PRIVATE
      ehsh::ehsh
  )
  add_test(NAME unit COMMAND unit)
endif()

################################################################################
# Examples
################################################################################
if (EHSH_BUILD_EXAMPLES)
  add_executable(main example/main.c)
  target_link_libraries(main
    PRIVATE
      ehsh::ehsh
  )
endif()

################################################################################
# Tools
################################################################################
include(CPack)
if (PROJECT_IS_TOP_LEVEL)
  # Variables
  file(GLOB_RECURSE EHSH_SOURCES  CONFIGURE_DEPENDS src/*.c)
  file(GLOB_RECURSE EHSH_HEADERS  CONFIGURE_DEPENDS src/*.h)
  file(GLOB_RECURSE EHSH_EXAMPLES CONFIGURE_DEPENDS example/*.c)
  file(GLOB_RECURSE EHSH_TESTS    CONFIGURE_DEPENDS test/*.c)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Actions
  find_program(CppCheck_EXECUTABLE cppcheck)
  add_custom_target(cppcheck
    COMMAND
      ${CppCheck_EXECUTABLE}
      --project=${PROJECT_BINARY_DIR}/compile_commands.json
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
  )

  find_program(ClangTidy_EXECUTABLE clang-tidy)
  add_custom_target(clang-tidy
    COMMAND
      ${ClangTidy_EXECUTABLE}
      --fix
      --format-style=file
      -p ${PROJECT_BINARY_DIR}
      ${EHSH_SOURCES} ${EHSH_HEADERS} ${EHSH_EXAMPLES} ${EHSH_TESTS}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
  )

  find_program(ClangFormat_EXECUTABLE clang-format)
  add_custom_target(clang-format
    COMMAND
      ${ClangFormat_EXECUTABLE}
      -i # inplace edit <file>s
      ${EHSH_SOURCES} ${EHSH_HEADERS} ${EHSH_EXAMPLES} ${EHSH_TESTS}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    VERBATIM
  )

  # Convenience Targets
  add_custom_target(lint)
  add_dependencies(lint cppcheck clang-tidy)
  add_custom_target(format)
  add_dependencies(format clang-format)
endif()

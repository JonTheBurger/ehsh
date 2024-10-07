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
    "${PROJECT_SOURCE_DIR}/README.md"
    "${PROJECT_SOURCE_DIR}/CMakePresets.json"
    "${PROJECT_SOURCE_DIR}/LICENSE.txt"
    src/ehsh.c
  PUBLIC
    FILE_SET ehsh
    TYPE HEADERS
    BASE_DIRS
      src
    FILES
      src/ehsh/ehsh.h
      src/ehsh/ehsh.cfg.h
      src/ehsh/extra/ehcmd.h
      src/ehsh/platform/eh.fptr.h
      src/ehsh/platform/eh.linux.h
      src/ehsh/platform/eh.platform.h
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
  enable_language(CXX)

  # GoogleTest
  find_package(GTest)
  if (NOT TARGET GTest::gmock_main)
    message(STATUS "Fetching GoogleTest from github...")
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.11.0
      GIT_SHALLOW    1
      EXCLUDE_FROM_ALL
    )
    FetchContent_MakeAvailable(googletest)
    if (NOT TARGET GTest::gmock_main)
      add_library(GTest::gmock_main ALIAS gmock_main)
    endif()
  endif()
  include(GoogleTest)

  add_executable(unit test/unit.cpp)
  target_link_libraries(unit PRIVATE ehsh::ehsh GTest::gmock_main)
  target_compile_features(unit PRIVATE cxx_std_20)
  add_test(NAME unit COMMAND unit)
  set_tests_properties(unit PROPERTIES TIMEOUT 5)

  # Coverage
  if (CMAKE_C_COMPILER_ID MATCHES Clang)
    find_program(LLVM_COV_EXECUTABLE llvm-cov)
    set(GCov_EXECUTABLE  "${GCov_EXECUTABLE} gcov")
  else()
    find_program(GCov_EXECUTABLE gcov)
  endif()
  find_program(GCovr_EXECUTABLE gcovr)
  add_custom_target(
    cov.html
    COMMAND
    cmake -E make_directory "${PROJECT_BINARY_DIR}/cov"
    COMMAND
      "${GCovr_EXECUTABLE}"
      --root   "${PROJECT_SOURCE_DIR}"
      --filter "${PROJECT_SOURCE_DIR}/src"
      --exclude ".*platform.*"
      --exclude-unreachable-branches
      --txt-summary
      --html-details "${PROJECT_BINARY_DIR}/cov/cov.html"
      --html-theme github.dark-blue
      --gcov-executable "${GCov_EXECUTABLE}"
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
    VERBATIM
  )

  # IDE Layout
  set_target_properties(
    unit
    cov.html
    PROPERTIES
      FOLDER test
  )
endif()

################################################################################
# Examples
################################################################################
if (EHSH_BUILD_EXAMPLES)
  add_executable(main example/main.c)
  target_link_libraries(main PRIVATE ehsh::ehsh)
  set_directory_properties(PROPERTIES VS_STARTUP_PROJECT main)
endif()

################################################################################
# Tools
################################################################################
include(CPack)
if (PROJECT_IS_TOP_LEVEL)
  add_subdirectory(tools)
endif()

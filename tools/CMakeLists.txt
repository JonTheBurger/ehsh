# Variables
file(GLOB_RECURSE EHSH_SOURCES  CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE EHSH_HEADERS  CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE EHSH_EXAMPLES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/example/*.c")
file(GLOB_RECURSE EHSH_TESTS    CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/test/*.cpp")
set(EHSH_ALL_CODE
  ${EHSH_SOURCES}
  ${EHSH_HEADERS}
  ${EHSH_EXAMPLES}
  ${EHSH_TESTS}
)

# Lint
find_program(CppCheck_EXECUTABLE cppcheck)
add_custom_target(cppcheck
  COMMAND
    ${CppCheck_EXECUTABLE}
    --project=${PROJECT_BINARY_DIR}/compile_commands.json
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  USES_TERMINAL
  VERBATIM
)

find_program(ClangTidy_EXECUTABLE clang-tidy)
add_custom_target(clang-tidy
  SOURCES
    "${PROJECT_SOURCE_DIR}/.clang-tidy"
  COMMAND
    ${ClangTidy_EXECUTABLE}
    --fix
    --format-style=file
    -p ${PROJECT_BINARY_DIR}
    ${EHSH_ALL_CODE}
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  USES_TERMINAL
  VERBATIM
)

# Format
find_program(ClangFormat_EXECUTABLE clang-format)
add_custom_target(clang-format
  SOURCES
    "${PROJECT_SOURCE_DIR}/.clang-format"
  COMMAND
    ${ClangFormat_EXECUTABLE}
    -i # inplace edit <file>s
    ${EHSH_ALL_CODE}
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  USES_TERMINAL
  VERBATIM
)

# Docs
find_package(Doxygen)
find_program(Sed_EXECUTABLE sed PATHS "C:/Program Files/Git/usr/bin")
find_package(Python3 COMPONENTS Interpreter)
if (Doxygen_FOUND AND Sed_EXECUTABLE AND Python3_FOUND)
  set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/docs")
  set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
  set(DOXYGEN_GENERATE_XML YES)
  set(DOXYGEN_GENERATE_HTML YES)
  set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
  set(DOXYGEN_EXAMPLE_PATH
    "example"
    "test"
  )
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
  set(DOXYGEN_IMAGE_PATH "docs")
  set(DOXYGEN_EXCLUDE_PATTERNS
    */tools/*
    */mcss/*
  )
  set(DOXYGEN_FILTER_SOURCE_FILES YES)
  set(DOXYGEN_INPUT_FILTER [[-e \"s#SPDX-License-Identifier: \([^[:space:] ]\+\)#@copyright <a href=\\\"https://spdx.org/licenses/\\1.html\\\">\\1</a>#g\"]])
  set(DOXYGEN_INPUT_FILTER "${Sed_EXECUTABLE} ${DOXYGEN_INPUT_FILTER}")
  set(DOXYGEN_CALL_GRAPH YES)
  set(DOXYGEN_CALLER_GRAPH YES)
  set(DOXYGEN_DOT_IMAGE_FORMAT svg)
  set(DOXYGEN_INTERACTIVE_SVG YES)
  set(DOXYGEN_XML_PROGRAMLISTING NO)
  doxygen_add_docs(
    doxygen
    "${PROJECT_SOURCE_DIR}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  )
  include(ExternalProject)
  ExternalProject_Add(
    mcss
    PREFIX            "${PROJECT_BINARY_DIR}/src/mcss"
    GIT_REPOSITORY    https://github.com/mosra/m.css.git
    GIT_TAG           df894269c25f368405742bc16fcb4bc3ca56384d
    GIT_PROGRESS      ON
    EXCLUDE_FROM_ALL  TRUE
    PATCH_COMMAND     "${Sed_EXECUTABLE}" -e "s/assert/pass #/g" -i "documentation/doxygen.py"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
    INSTALL_COMMAND   ""
  )
  ExternalProject_Get_property(mcss SOURCE_DIR)
  set(MCSS_DIR ${SOURCE_DIR})
  set(Python3_ROOT_DIR "${PROJECT_SOURCE_DIR}/.venv/Scripts")
  set(Python3_FIND_VIRTUALENV FIRST)
  set(VENV_BIN_DIR bin)
  if (WIN32)
    set(VENV_BIN_DIR Scripts)
  endif()
  configure_file(conf.py conf.py)
  add_custom_target(docs
    SOURCES
      "${PROJECT_SOURCE_DIR}/.readthedocs.yaml"
      "${PROJECT_SOURCE_DIR}/tools/conf.py"
      "${PROJECT_SOURCE_DIR}/tools/requirements.txt"
    COMMAND
      Python3::Interpreter
      -m venv
      "${CMAKE_CURRENT_BINARY_DIR}/.venv"
    COMMAND
      "${CMAKE_CURRENT_BINARY_DIR}/.venv/${VENV_BIN_DIR}/python"
      -m pip
      install
      -r "${PROJECT_SOURCE_DIR}/tools/requirements.txt"
    COMMAND
      "${CMAKE_CURRENT_BINARY_DIR}/.venv/${VENV_BIN_DIR}/python"
      "${MCSS_DIR}/documentation/doxygen.py"
      "${CMAKE_CURRENT_BINARY_DIR}/conf.py"
      --no-doxygen
      # --debug
    WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}"
    USES_TERMINAL
    VERBATIM
  )
  add_dependencies(docs doxygen mcss)
else()
  # Add dummy targets
  set(warn_MSG "ERROR: Cannot generate docs on this platform: 'doxygen', 'sed', and 'python' must be installed.")
  set(warn_CMD
    COMMAND "${CMAKE_COMMAND}" -E echo
    COMMAND "${CMAKE_COMMAND}" -E echo ${warn_MSG}
    COMMAND "${CMAKE_COMMAND}" -E echo
    COMMAND "${CMAKE_COMMAND}" -E false
    VERBATIM
  )
  add_custom_target(doxygen ${warn_CMD})
  add_custom_target(mcss ${warn_CMD})
  add_custom_target(docs ${warn_CMD})
  message(WARNING "${warn_MSG}")
endif()

# Convenience Targets
add_custom_target(lint)
add_dependencies(lint cppcheck clang-tidy)
add_custom_target(format)
add_dependencies(format clang-format)

# IDE Layout
set_target_properties(
  clang-format
  clang-tidy
  cppcheck
  docs
  doxygen
  format
  lint
  mcss
  PROPERTIES
    FOLDER tools
)

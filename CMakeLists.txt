cmake_minimum_required(VERSION 3.20)
project(ehsh LANGUAGES C)

# TODO presets
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ehsh
  src/ehsh.c
  src/ehsh/ehsh.h
  src/ehsh/platform/eh.linux.h
  src/ehsh/platform/eh.stdc.h
  src/ehsh/platform/eh.win32.h
)
target_include_directories(ehsh
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
# TODO header sets
# TODO install
# TODO alias

add_executable(main main.c)
target_link_libraries(main
  PRIVATE
    ehsh
)

cmake_minimum_required(VERSION 3.15)
project(PackageTest C)

find_package(ehsh CONFIG REQUIRED)

add_executable(example src/main.c)
target_link_libraries(example ehsh::ehsh)

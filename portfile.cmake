# This is a hack to use the repo in-place. Generally prefer to make a proper port:
# https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE SHA
)
vcpkg_from_git(
    OUT_SOURCE_PATH SOURCE_PATH
    URL "file://${CMAKE_CURRENT_LIST_DIR}"
    REF "${SHA}"
    HEAD_REF main
    LFS https://github.com/JonTheBurger/ehsh.git
)
vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)
vcpkg_cmake_install()

# vcpkg_cmake_config_fixup(PACKAGE_NAME "ehsh")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.txt")
# https://learn.microsoft.com/en-us/vcpkg/maintainers/functions/vcpkg_from_github

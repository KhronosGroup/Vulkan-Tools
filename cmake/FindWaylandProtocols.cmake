# Wayland protocols Defines:
#
# * WAYLAND_PROTOCOLS_FOUND    True if the wayland-protocols data path is found
# * WAYLAND_PROTOCOLS_PATH     Path to the wayland-scanner executable
#

if(NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(WAYLAND_PROTOCOLS_PATH REQUIRED wayland-protocols)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=pkgdatadir wayland-protocols
       OUTPUT_VARIABLE WAYLAND_PROTOCOLS_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(FPHSA_NAME_MISMATCHED ON)
    find_package_handle_standard_args(WAYLAND DEFAULT_MSG WAYLAND_PROTOCOLS_PATH)
    unset(FPHSA_NAME_MISMATCHED)

    mark_as_advanced(WAYLAND_PROTOCOLS_PATH)
endif()

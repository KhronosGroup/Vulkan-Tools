# Wayland protocols Defines:
#
# * WAYLAND_PROTOCOLS_FOUND    True if the wayland-protocols data path is found
# * WAYLAND_PROTOCOLS_PATH     Path to the wayland-scanner executable
#

if(NOT WIN32)
    find_package(PkgConfig)
    pkg_check_modules(PKG_WAYLAND_PROTOCOLS QUIET wayland-protocols)
    set(WAYLAND_PROTOCOLS_PATH ${PKG_WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols)
    find_package_handle_standard_args(WAYLAND DEFAULT_MSG WAYLAND_PROTOCOLS_PATH)
    mark_as_advanced(WAYLAND_PROTOCOLS_PATH)
endif()

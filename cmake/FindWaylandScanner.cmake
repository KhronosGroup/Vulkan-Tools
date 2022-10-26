# Wayland scanner Defines:
#
# * WAYLAND_SCANNER_FOUND         True if wayland-scanner is found
# * WAYLAND_SCANNER_EXECUTABLE    Path to the wayland-scanner executable
#

if(NOT WIN32)
    # Delegate to pkg-config for our first guess
    find_package(PkgConfig)
    pkg_check_modules(PKG_WAYLAND_SCANNER QUIET wayland-scanner)
    find_program(WAYLAND_SCANNER_EXECUTABLE wayland-scanner ${PKG_WAYLAND_SCANNER_PREFIX}/bin/wayland-scanner)

    set(FPHSA_NAME_MISMATCHED ON)
    find_package_handle_standard_args(WAYLAND DEFAULT_MSG WAYLAND_SCANNER_EXECUTABLE)
    unset(FPHSA_NAME_MISMATCHED)

    mark_as_advanced(WAYLAND_SCANNER_EXECUTABLE)
endif()

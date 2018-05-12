# CMakeLists.txt file for building Vulkaninfo

if(WIN32)
    add_executable(vulkaninfo vulkaninfo.c vulkaninfo.rc)
elseif(APPLE)
    add_executable(vulkaninfo vulkaninfo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo/metal_view.m
        ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo/metal_view.h
        )
else()
    add_executable(vulkaninfo vulkaninfo.c)
endif()

if(APPLE)
    target_link_libraries(${API_LOWERCASE}info ${LIBRARIES} "-framework AppKit -framework QuartzCore")
    target_include_directories(${API_LOWERCASE}info PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo)
else()
    if(DEFINED LOADER_REPO_ROOT)
        message(STATUS "Using user-supplied path to locate Vulkan")
        if(WIN32)
            set (LOADER_SEARCH_PATHS
                "${LOADER_REPO_ROOT}/${BUILDTGT_DIR}/loader/${DEBUG_DECORATION}"
                "${LOADER_REPO_ROOT}/${BUILDTGT_DIR}/loader/${RELEASE_DECORATION}"
                )
        elseif(UNIX)
            set (LOADER_SEARCH_PATHS
                "${LOADER_REPO_ROOT}/${BUILDTGT_DIR}/loader"
                )
        endif()

        find_library(LIBVK NAMES vulkan vulkan-1
            HINTS ${LOADER_SEARCH_PATHS}
            )
        message(STATUS "Found Vulkan: ${LIBVK}")
        target_link_libraries(vulkaninfo ${LIBVK})
    else()
        message(STATUS "Using find_package to locate Vulkan")
        find_package(Vulkan)
        set (LIBVK "Vulkan::Vulkan")
        target_link_libraries(vulkaninfo Vulkan::Vulkan)
    endif()
endif()

if(APPLE)
    set_target_properties(vulkaninfo PROPERTIES
        INSTALL_RPATH "@loader_path/../lib"
    )
    install(TARGETS vulkaninfo DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Create vulkaninfo application bundle for MacOS
if(APPLE)
    include(macOS/vulkaninfo/vulkaninfo.cmake)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(INSTALL_LVL_FILES)
        install(TARGETS vulkaninfo DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()

if(WIN32)
    target_compile_definitions(vulkaninfo PUBLIC -DVK_USE_PLATFORM_WIN32_KHR -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS)

    # Use static MSVCRT libraries
    foreach(configuration in CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
                             CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${configuration} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${configuration} "${${configuration}}")
        endif()
    endforeach()

    file(COPY vulkaninfo.vcxproj.user DESTINATION ${CMAKE_BINARY_DIR}/demos)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    if(INSTALL_LVL_FILES)
        install(TARGETS vulkaninfo DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()


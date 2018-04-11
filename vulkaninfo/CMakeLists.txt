# CMakeLists.txt file for building Vulkaninfo

add_executable(vulkaninfo vulkaninfo.c)
target_link_libraries(vulkaninfo Vulkan::Vulkan)

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
        install(TARGETS ${API_LOWERCASE}info DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()
endif()


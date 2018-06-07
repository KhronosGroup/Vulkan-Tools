# Vulkaninfo Application Bundle

# We already have a "vulkaninfo" target, so create a new target with a different name
# and use the OUTPUT_NAME property to rename the target to the desired name.
# The standalone binary is called "vulkaninfo" and the bundle is called "vulkaninfo.app".
# Note that the executable is a script that launches Terminal to see the output.
add_executable(vulkaninfo-bundle MACOSX_BUNDLE
    vulkaninfo.c
    ${CMAKE_BINARY_DIR}/staging-json/MoltenVK_icd.json
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo.sh
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Resources/LunarGIcon.icns
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo/metal_view.m
    ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo/metal_view.h
)
set_target_properties(vulkaninfo-bundle PROPERTIES
    OUTPUT_NAME vulkaninfo
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Info.plist
)
# We do this so vulkaninfo is linked to an individual library and NOT a framework.
target_link_libraries(vulkaninfo-bundle ${Vulkan_LIBRARY} "-framework AppKit -framework QuartzCore")
target_include_directories(vulkaninfo-bundle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo ${Vulkan_INCLUDE_DIR})
add_dependencies(vulkaninfo-bundle MoltenVK_icd-staging-json)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/macOS/vulkaninfo.sh PROPERTIES
    MACOSX_PACKAGE_LOCATION "MacOS"
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/macOS/Resources/LunarGIcon.icns PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources"
)
set_source_files_properties(${CMAKE_BINARY_DIR}/staging-json/MoltenVK_icd.json PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources/vulkan/icd.d"
)

# Direct the MoltenVK library to the right place.
install(FILES "${MOLTENVK_DIR}/MoltenVK/macOS/libMoltenVK.dylib"
        DESTINATION "demos/vulkaninfo.app/Contents/Frameworks"
        COMPONENT Runtime
)
# Xcode projects need some extra help with what would be install steps.
if(${CMAKE_GENERATOR} MATCHES "^Xcode.*")
    add_custom_command(TARGET vulkaninfo-bundle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/macOS/libMoltenVK.dylib"
            ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/vulkaninfo.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan
    )
else()
    add_custom_command(TARGET vulkaninfo-bundle POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${MOLTENVK_DIR}/MoltenVK/macOS/libMoltenVK.dylib"
            ${CMAKE_CURRENT_BINARY_DIR}/vulkaninfo.app/Contents/Frameworks/libMoltenVK.dylib
        DEPENDS vulkan
    )
endif()

# Fix up the library search path in the executable to find (loader) libraries in the bundle.
# When fixup_bundle() is passed a bundle in the first argument, it looks at the Info.plist file
# to determine the BundleExecutable.  In this case, the executable is a script, which can't be fixed up.
# Instead pass it the explicit name of the executable.
install(CODE "
    include(BundleUtilities)
    fixup_bundle(${CMAKE_INSTALL_PREFIX}/vulkaninfo/vulkaninfo.app/Contents/MacOS/vulkaninfo \"\" \"\")
    " COMPONENT Runtime
)

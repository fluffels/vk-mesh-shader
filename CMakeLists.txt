cmake_minimum_required (VERSION 3.7)
set (CMAKE_CXX_STANDARD 17)

project (vk-template)

find_package(Vulkan REQUIRED)

set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/Bin/glslc.exe")
file(GLOB_RECURSE GLSL_FILES "shaders/*.vert" "shaders/*.frag" "shaders/*.mesh")
foreach(GLSL_FILE ${GLSL_FILES})
    set(SPIRV_FILE "${GLSL_FILE}.spv")
    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSL_VALIDATOR} ${GLSL_FILE} -o ${SPIRV_FILE}
        DEPENDS ${GLSL_FILE}
    )
    list(APPEND SPIRV_FILES ${SPIRV_FILE})
endforeach(GLSL_FILE)
add_custom_target(Shaders ALL DEPENDS ${SPIRV_FILES})

include_directories(${CMAKE_HOME_DIRECTORY}/src)
include_directories(${CMAKE_HOME_DIRECTORY}/lib)
include_directories(${CMAKE_HOME_DIRECTORY}/lib/easyloggingpp/src)
include_directories(${CMAKE_HOME_DIRECTORY}/lib/jcwk)
include_directories(${glm_INCLUDE_DIRS})
include_directories(${Vulkan_INCLUDE_DIRS})

set(PLATFORM_INDEPENDENT_FILES 
    lib/jcwk/FileSystem.cpp
    lib/jcwk/Vulkan.cpp
    lib/SPIRV-Reflect/spirv_reflect.c
    lib/easyloggingpp/src/easylogging++.cc
    src/RenderMesh.cpp
    src/RenderText.cpp
    src/Render.cpp
    src/State.cpp
)

IF (WIN32)
    add_executable(
        main
        WIN32
        Shaders
        src/MainWin32.cpp
        lib/jcwk/Win32/DirectInput.cpp
        lib/jcwk/Win32/Controller.cpp
        lib/jcwk/Win32/Mouse.cpp
        ${PLATFORM_INDEPENDENT_FILES}
    )
ENDIF()

target_link_libraries(
    main
    ${Vulkan_LIBRARIES}
    dinput8.lib
    dxguid.lib
)

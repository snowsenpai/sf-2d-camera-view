cmake_minimum_required(VERSION 3.28)
project(camera_view LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(camera_view
    src/main.cpp
    src/Vec2.h
    src/Vec2.cpp
    src/CameraTypes.h
    src/Camera.h
    src/CameraManager.h
    src/CameraManager.cpp
    src/Components.h
    src/FixedCamera.h
    src/FixedCamera.cpp
    src/Entity.h
    src/Entity.cpp
    src/EntityManager.h
    src/EntityManager.cpp
    src/Game.h
    src/Game.cpp)

target_compile_features(camera_view PRIVATE cxx_std_17)
target_link_libraries(camera_view PRIVATE SFML::Graphics)

set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT camera_view
)

# copy assets beside the executable for building
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET camera_view POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:camera_view>/assets"
    )
endif()
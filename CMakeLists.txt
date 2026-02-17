cmake_minimum_required(VERSION 3.15)
project(NurbsDraw CXX)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

set(FETCHCONTENT_FULLY_DISCONNECTED ON)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)

FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
    EXCLUDE_FROM_ALL
)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)

FetchContent_MakeAvailable(imgui sdl3)


add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PRIVATE SDL3::SDL3)

# ---- Your app ----

file(GLOB_RECURSE sources ${CMAKE_SOURCE_DIR}/src/**/*.cpp ${CMAKE_SOURCE_DIR}/src/**/*.h)

add_executable(${PROJECT_NAME} main.cpp ${sources})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    imgui_lib
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends ${sdl3_SOURCE_DIR}/include)

# target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/build/compile_commands.json
    )
endif()
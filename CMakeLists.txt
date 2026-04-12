cmake_minimum_required(VERSION 3.15)
project(NurbsDraw CXX)
set(CMAKE_CXX_STANDARD 23)

# ---- Qt6 ----
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# ---- FetchContent ----
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_Declare(
  msft_proxy4
  GIT_TAG 4.0.0 # or above
  GIT_REPOSITORY https://github.com/ngcpp/proxy.git
)

FetchContent_MakeAvailable(googletest msft_proxy4)

enable_testing()
# ---- Your app ----

file(GLOB sources 
    src/math/*.cpp
    src/ui/*.cpp    
)

# ---- Enable Qt automoc ----
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${PROJECT_NAME} main.cpp ${sources})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    msft_proxy4::proxy
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

# ---- Tests ----
add_subdirectory(tests)
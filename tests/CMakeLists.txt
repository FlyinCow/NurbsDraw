include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB testsources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(NurbsDrawTest
    ${testsources}
)

target_compile_features(NurbsDrawTest PUBLIC cxx_std_23)
target_include_directories(NurbsDrawTest PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(NurbsDrawTest PRIVATE
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(NurbsDrawTest)
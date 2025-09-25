include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    tests
    test_game.cpp
    ../src/game.cpp
    ../src/input.cpp
    ../src/render.cpp
    ../src/terminal.cpp
)

target_include_directories(tests PUBLIC ../include)

target_link_libraries(
    tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)

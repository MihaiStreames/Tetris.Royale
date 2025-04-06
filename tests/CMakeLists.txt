
# CMakeLists.txt for testing purpose
# This file is made to compile tests for the Tetris Royale project
# It is not meant to be used in production, but only for development and testing
cmake_minimum_required(VERSION 3.22...4.0)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# function to add a test executable and link it with gtest and the Tetris libraries
function(add_tetris_test TEST_NAME)
  add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
  target_include_directories(${TEST_NAME} PRIVATE ${TETRIS_INCLUDE_DIR})
  target_link_libraries(${TEST_NAME}
    gtest_main
    TetrisRoyaleCommon
    TetrisRoyaleServerLogic
  )
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# actual tests
add_tetris_test(BagTest)

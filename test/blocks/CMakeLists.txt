add_library(
  block_test_deps SHARED
  util.cpp
  util.h
)

target_link_libraries(
  block_test_deps
  GTest::gtest_main
  scratchcpp
  scratchcpp-render
)

# pen_blocks_test
add_executable(
  pen_blocks_test
  pen_blocks_test.cpp
)

target_link_libraries(
  pen_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp-render
  scratchcpprender_mocks
  qnanopainter
  block_test_deps
  ${QT_LIBS}
)

add_test(pen_blocks_test)
gtest_discover_tests(pen_blocks_test)

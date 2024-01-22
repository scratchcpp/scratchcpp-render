# pen_blocks_test
add_executable(
  pen_blocks_test
  pen_blocks_test.cpp
)

target_link_libraries(
  pen_blocks_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
)

add_test(pen_blocks_test)
gtest_discover_tests(pen_blocks_test)

# penextension_test
add_executable(
  penextension_test
  penextension_test.cpp
)

target_link_libraries(
  penextension_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
)

add_test(penextension_test)
gtest_discover_tests(penextension_test)

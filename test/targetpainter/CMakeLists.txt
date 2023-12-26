add_executable(
  targetpainter_test
  targetpainter_test.cpp
)

target_link_libraries(
  targetpainter_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  qnanopainter
)

add_test(targetpainter_test)
gtest_discover_tests(targetpainter_test)

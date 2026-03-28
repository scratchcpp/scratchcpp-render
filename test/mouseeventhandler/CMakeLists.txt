add_executable(
  mouseeventhandler_test
  mouseeventhandler_test.cpp
)

target_link_libraries(
  mouseeventhandler_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp
  scratchcpp-render
  scratchcpprender_mocks
  qnanopainter
  ${QT_LIBS}
  Qt6::Test
)

add_test(mouseeventhandler_test)
gtest_discover_tests(mouseeventhandler_test)

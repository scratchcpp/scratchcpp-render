add_executable(
  mouseeventhandler_test
  mouseeventhandler_test.cpp
  fakerepeater.cpp
  fakerepeater.h
)

target_link_libraries(
  mouseeventhandler_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcppgui_mocks
  ${QT_LIBS}
  Qt6::Test
)

add_test(mouseeventhandler_test)
gtest_discover_tests(mouseeventhandler_test)

add_executable(
  keyeventhandler_test
  keyeventhandler_test.cpp
)

target_link_libraries(
  keyeventhandler_test
  GTest::gtest_main
  libscratchcpp-gui
  ${QT_LIBS}
  Qt6::Test
)

add_test(keyeventhandler_test)
gtest_discover_tests(keyeventhandler_test)

add_executable(
  global_functions_test
  global_functions_test.cpp
)

target_link_libraries(
  global_functions_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  ${QT_LIBS}
)

add_test(global_functions_test)
gtest_discover_tests(global_functions_test)

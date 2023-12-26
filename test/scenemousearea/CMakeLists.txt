add_executable(
  scenemousearea_test
  scenemousearea_test.cpp
)

target_link_libraries(
  scenemousearea_test
  GTest::gtest_main
  GTest::gmock_main
  scratchcpp-render
  scratchcpprender_mocks
  ${QT_LIBS}
  Qt6::Test
)

add_test(scenemousearea_test)
gtest_discover_tests(scenemousearea_test)

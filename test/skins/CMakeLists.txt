# bitmapskin
add_executable(
  bitmapskin_test
  bitmapskin_test.cpp
)

target_link_libraries(
  bitmapskin_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
)

add_test(bitmapskin_test)
gtest_discover_tests(bitmapskin_test)

# svgskin
add_executable(
  svgskin_test
  svgskin_test.cpp
)

target_link_libraries(
  svgskin_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
)

add_test(svgskin_test)
gtest_discover_tests(svgskin_test)

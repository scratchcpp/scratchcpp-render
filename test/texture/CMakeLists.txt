add_executable(
  texture_test
  texture_test.cpp
)

target_link_libraries(
  texture_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
)

add_test(texture_test)
gtest_discover_tests(texture_test)

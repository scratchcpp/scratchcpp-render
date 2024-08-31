add_executable(
  textbubbleshape_test
  textbubbleshape_test.cpp
)

target_link_libraries(
  textbubbleshape_test
  GTest::gtest_main
  scratchcpp-render
  ${QT_LIBS}
  Qt6::Test
)

add_test(textbubbleshape_test)
gtest_discover_tests(textbubbleshape_test)

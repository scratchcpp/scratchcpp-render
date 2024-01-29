add_executable(
  graphicseffect_test
  graphicseffect_test.cpp
)

target_link_libraries(
  graphicseffect_test
  GTest::gtest_main
  scratchcpp-render
  qnanopainter
)

add_test(graphicseffect_test)
gtest_discover_tests(graphicseffect_test)
